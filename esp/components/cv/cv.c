/**
 * @file cv.c
 * 
 * Interface used to manage the computer vision API call and data
 * 
 */

#include "../../main/fsm.h"
#include "../https/https_task.h"

#include "cv.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_crt_bundle.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/param.h>
#include <stdbool.h>

#ifndef CONFIG_USE_MOCK_CAMERA
    #include "esp_camera.h"
#endif

// Mock image embedded (for Wokwi)
#ifdef CONFIG_USE_MOCK_CAMERA
extern const uint8_t mock_image_start[] asm("_binary_mock_plate_jpg_start");
extern const uint8_t mock_image_end[]   asm("_binary_mock_plate_jpg_end");
#endif

#define YIELD() vTaskDelay(pdMS_TO_TICKS(10))

// License plate API
#define CV_API_URL "https://www.circuitdigest.cloud/api/v1/readnumberplate"
#define CV_API_KEY CONFIG_API_KEY
#define MAX_HTTP_OUTPUT_BUFFER 1024

static const char *TAG = "CV Module";
static char api_response_buffer[MAX_HTTP_OUTPUT_BUFFER];
static int response_len = 0;
static TaskHandle_t recognition_task_handle = NULL;

// The event handler, which collects and saves 1KB chunks of response data for each HTTPS request
static esp_err_t http_event_handler(esp_http_client_event_handle_t evt)
{
    if (evt -> event_id == HTTP_EVENT_ON_DATA && evt -> data_len > 0) {
        size_t copy_len = evt->data_len;

        if (response_len + copy_len >= MAX_HTTP_OUTPUT_BUFFER) {
            copy_len = MAX_HTTP_OUTPUT_BUFFER - response_len - 1;
            ESP_LOGW(TAG, "Response buffer full, truncating");
        }

        memcpy(api_response_buffer + response_len, evt -> data, copy_len);
        response_len += copy_len;
    }

    return ESP_OK;
}

// Formats the response buffer as pretty JSON and prints it to console
void print_api_response_buffer() {
    printf("\n---------- Response content: -------------\n\n");

    cJSON *root = cJSON_Parse(api_response_buffer);

    if (root == NULL) {
        printf("%s", api_response_buffer); // fallback
    } else {
        char *pretty = cJSON_Print(root);

        if (pretty) {
            // Replaces tabs with 2 spaces for better readability
            for (char *p = pretty; *p; p++) {
                if (*p == '\t') {
                    printf("  ");
                } else {
                    putchar(*p);
                }
            }
            
            free(pretty);
        }

        cJSON_Delete(root);
    }
    
    printf("\n\n------------------------------------------\n\n");
}

/**
 * @brief Performs the HTTPS request to the CV API, to send the image data
 * @param payload The payload to send in the request body (image data)
 * @param payload_len Length of the payload in bytes
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t perform_cv_api_request(const char *payload, size_t payload_len)
{
    // Reset the response buffer
    response_len = 0;
    memset(api_response_buffer, 0, sizeof(api_response_buffer));
    
    // HTTP client configuration
    char content_type[128];
    snprintf(content_type, sizeof(content_type), "multipart/form-data; boundary=----ESP32Boundary");

    esp_http_client_config_t config = {
        .url = CV_API_URL,
        .method = HTTP_METHOD_POST,
        .event_handler = http_event_handler,
        .timeout_ms = 30000,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .skip_cert_common_name_check = true,
    };
    
    vTaskDelay(pdMS_TO_TICKS(10));

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", content_type);
    esp_http_client_set_header(client, "Authorization", CV_API_KEY);
    esp_http_client_set_post_field(client, (char*) payload, payload_len);

    // Perform the HTTP POST request and prints the response
    esp_err_t err = esp_http_client_perform(client);

    vTaskDelay(pdMS_TO_TICKS(10));
    // Truncate response buffer to a null-terminated string
    if (response_len < sizeof(api_response_buffer)) {
        api_response_buffer[response_len] = '\0';
    } else {
        api_response_buffer[sizeof(api_response_buffer) - 1] = '\0';
    }

    // Response handling
    if (err == ESP_OK) {
        ESP_LOGI(
            TAG,
            "Response ESP_OK - Status %d with content_length = %" PRId64,
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client)
        );
    } else {
        ESP_LOGE(TAG, "request failed: %s", esp_err_to_name(err));
    }
    // Cleans up the HTTPS client
    esp_http_client_cleanup(client);
    
    return err;
}

/**
 * @brief Prepares the image payload for the CV API request
 * @param image_data Pointer to the image data buffer
 * @param image_len Length of the image data in bytes
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t prepare_image_payload(const uint8_t *image_data, size_t image_len) {
    if (!image_data || image_len == 0) {
        ESP_LOGE(TAG, "invalid image data passed to plate_recognition_api()");
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "sending image to API (%d bytes)...", image_len);
    fflush(stdout);

    // Prepare multipart form data (header and footer)
    char *data_header = NULL;
    const char *data_footer = "\r\n------ESP32Boundary--\r\n";

    asprintf(
        &data_header,
        "------ESP32Boundary\r\n"
        "Content-Disposition: form-data; name=\"imageFile\"; filename=\"%s.jpeg\"\r\n"
        "Content-Type: image/jpeg\r\n\r\n",
        CV_API_KEY
    );
    
    if (!data_header) {
        ESP_LOGE(TAG, "invalid data_header");
        return ESP_ERR_NO_MEM;
    }
    
    size_t data_header_len = strlen(data_header);
    size_t data_footer_len  = strlen(data_footer);
    
    // Calculate total length of POST body and checks for overflow
    size_t total_len = data_header_len + image_len + data_footer_len;

    if (image_len > SIZE_MAX - data_header_len - data_footer_len) {
        ESP_LOGE(TAG, "total_len overflow in send_image_to_api()");
        free(data_header);
        return ESP_ERR_NO_MEM;
    }
    
    size_t largest_free_size = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    ESP_LOGI(
        TAG, "payload size = %u, available size = %u", 
        (unsigned) total_len,
        (unsigned) largest_free_size
    );

    uint8_t *post_data = malloc(total_len);

    if (!post_data) {
        ESP_LOGE(TAG, "malloc(%u) failed, aborting...", (unsigned) total_len);
        free(data_header);
        return ESP_ERR_NO_MEM;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    // Build payload with header, image data, and footer
    memcpy(post_data, data_header, data_header_len);
    memcpy(post_data + data_header_len, image_data, image_len);
    memcpy(post_data + data_header_len + image_len, data_footer, data_footer_len);
    
    // Perform the HTTP POST request and prints the response
    esp_err_t err = perform_cv_api_request((const char *) post_data, total_len);
    print_api_response_buffer();

    free(post_data);
    free(data_header);
    
    return err;
}

char* extract_plate_from_response(void) {
    char *plate = NULL;

    cJSON *root = cJSON_Parse(api_response_buffer);
    if (root == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON response");
        return NULL;
    }

    cJSON *data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) {
        ESP_LOGE(TAG, "\"data\" object not found in response");
        cJSON_Delete(root);
        return NULL;
    }

    cJSON *plate_item = cJSON_GetObjectItem(data, "number_plate");

    if (cJSON_IsString(plate_item) && (plate_item->valuestring != NULL)) {
        plate = strdup(plate_item->valuestring);
    } else {
        ESP_LOGE(TAG, "number_plate not found in response");
    }

    cJSON_Delete(root);
    return plate;
}

char* extract_image_link_from_response(void) {
    char *image_link = NULL;

    cJSON *root = cJSON_Parse(api_response_buffer);
    if (root == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON response");
        return NULL;
    }

    cJSON *data = cJSON_GetObjectItem(root, "data");
    if (data == NULL) {
        ESP_LOGE(TAG, "\"data\" object not found in response");
        cJSON_Delete(root);
        return NULL;
    }

    cJSON *image_item = cJSON_GetObjectItem(data, "view_image");

    if (cJSON_IsString(image_item) && (image_item->valuestring != NULL)) {
        image_link = strdup(image_item->valuestring);
    } else {
        ESP_LOGE(TAG, "view_image not found in response");
    }

    cJSON_Delete(root);
    return image_link;
}

////////////////////////////////////////////////
/////////////// CV task ////////////////////////
////////////////////////////////////////////////


void cv_task_creator(void) {
    xTaskCreatePinnedToCore(
        recognition_task,
        "recognition_task",
        12288,
        NULL,
        tskIDLE_PRIORITY + 1,
        &recognition_task_handle,
        0
    );
}

void unblock_recognition_task(void) {
    if (recognition_task_handle != NULL) {
        xTaskNotifyGive(recognition_task_handle);
    }
}


/**
 * Plate recognition task
 * Waits for notification from weight task,
 * then captures image and sends to CV API.
 * The use of task notification allows to
 * efficiently block/unblock the task without
 * busy waiting and is generally more lightweight
 * than using semaphores or event groups.
 * Based on the result of the recognition, it
 * triggers the appropriate FSM event.
 */
void recognition_task(void *arg)
{   
    while (1) {
        // Block until weight detection signals entry
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        ESP_LOGI(TAG, "Plate recognition task started...");
        
    #ifdef CONFIG_USE_MOCK_CAMERA
        // MOCK VERSION: Use embedded image
        size_t image_size = mock_image_end - mock_image_start;
        ESP_LOGI(TAG, "Using MOCK image (%d bytes)", image_size);
        prepare_image_payload(mock_image_start, image_size);
    #else
        // REAL VERSION: Capture from camera
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            return;
        }
        
        ESP_LOGI(TAG, "Camera captured %d bytes", fb->len);
        prepare_image_payload(fb->buf, fb->len);
    #endif
        YIELD();
        const char *plate = extract_plate_from_response();
        YIELD();
        const char *image_link = extract_image_link_from_response();

        if (plate != NULL && image_link != NULL) {
            ESP_LOGI(TAG, "===== PLATE DETECTED: %s =====", plate);
            ESP_LOGI(TAG, "===== IMAGE LINK: %s =====", image_link);
            set_license_plate_data((char*) plate, (char*) image_link);

            fsm_handle_event(PLATE_RECOGNIZED);
        } else {
            ESP_LOGE(TAG, "Plate recognition failed");

            fsm_handle_event(PLATE_REFUSED);
        }
        
        free((void*) plate);
        free((void*) image_link);

    #ifndef CONFIG_USE_MOCK_CAMERA
        esp_camera_fb_return(fb);
    #endif
    }
}