#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "ultrasonic.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "esp_heap_caps.h"
#include "esp_crt_bundle.h"

#ifndef CONFIG_USE_MOCK_CAMERA
    #include "esp_camera.h"
#endif

#include "../components/weight/weight.h"
#include "fsm.h"


//variables
Event_t event = NONE;

State_t curr_state = INIT;

Fsm_t fsm[] = {
               {INIT, init_fn},
               {IDLE, idle_fn},
               {VEHICLE_ENTRY, entry_fn},
               {ENTRY_REFUSED, refuse_fn},
               {ENTRY_ALLOWED, allow_fn},
               {VEHICLE_EXIT, exit_fn},
               {CLOSED, closed_fn}
};


#define TRIG_GPIO   GPIO_NUM_42
#define ECHO_GPIO   GPIO_NUM_41
static ultrasonic_sensor_t sensor = {
  .trigger_pin = TRIG_GPIO,
  .echo_pin = ECHO_GPIO
};

#define LED_R_GPIO  GPIO_NUM_6
#define LED_G_GPIO  GPIO_NUM_7
#define LED_B_GPIO  GPIO_NUM_8

#define MAX_DISTANCE_CM 500

// API Configuration
#define SERVER_URL "https://www.circuitdigest.cloud/readnumberplate"
#define API_KEY    CONFIG_API_KEY

static const char *TAG = "NPR_PROJECT";

// Mock image embedded (for Wokwi)
#ifdef CONFIG_USE_MOCK_CAMERA
extern const uint8_t mock_image_start[] asm("_binary_mock_plate_jpg_start");
extern const uint8_t mock_image_end[]   asm("_binary_mock_plate_jpg_end");
#endif

/* RGB LED Functions */
static void rgb_off(void) {
    gpio_set_level(LED_R_GPIO, 1);
    gpio_set_level(LED_G_GPIO, 1);
    gpio_set_level(LED_B_GPIO, 1);
}

static void rgb_green(void) {
    gpio_set_level(LED_R_GPIO, 1);
    gpio_set_level(LED_G_GPIO, 0);
    gpio_set_level(LED_B_GPIO, 1);
}

static void rgb_blue(void) {
    gpio_set_level(LED_R_GPIO, 1);
    gpio_set_level(LED_G_GPIO, 1);
    gpio_set_level(LED_B_GPIO, 0);
}

static void rgb_init(void) {
    gpio_config_t io = {0};
    io.pin_bit_mask = (1ULL << LED_R_GPIO) | (1ULL << LED_G_GPIO) | (1ULL << LED_B_GPIO);
    io.mode = GPIO_MODE_OUTPUT;
    io.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io);
    rgb_off();
}

/* HTTP Event Handler */
static esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // Print response
                char *response = malloc(evt->data_len + 1);
                if (response) {
                    memcpy(response, evt->data, evt->data_len);
                    response[evt->data_len] = '\0';
                    ESP_LOGI(TAG, "Response: %s", response);
                    
                    // Parse number plate (simple string search)
                    char *plate_start = strstr(response, "\"number_plate\":\"");
                    if (plate_start) {
                        plate_start += 16;  // Skip "number_plate":"
                        char *plate_end = strchr(plate_start, '"');
                        if (plate_end) {
                            int len = plate_end - plate_start;
                            char plate[32] = {0};
                            if (len < 32) {
                                memcpy(plate, plate_start, len);
                                ESP_LOGI(TAG, "===== PLATE DETECTED: %s =====", plate);
                            }
                        }
                    }
                    free(response);
                }
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

/* Send image to API */
static esp_err_t send_image_to_api(const uint8_t *image_data, size_t image_len) {
    if (!image_data || image_len == 0) {
        ESP_LOGE(TAG, "Invalid image data");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "Sending image to API (%d bytes)", image_len);
    ESP_LOGI(TAG, "DEBUG1 reached");
    rgb_blue();  // Blue LED = processing
    ESP_LOGI(TAG, "DEBUG3 reached");
    fflush(stdout);
    // Prepare multipart form data
    const char *boundary = "----ESP32Boundary";
    char *header_part1 = NULL;
    char *header_part2 = NULL;
    const char *footer = "\r\n------ESP32Boundary--\r\n";
    ESP_LOGI(TAG, "DEBUG4 reached");
    asprintf(&header_part1,
        "------ESP32Boundary\r\n"
        "Content-Disposition: form-data; name=\"imageFile\"; filename=\"%s.jpeg\"\r\n"
        "Content-Type: image/jpeg\r\n\r\n",
        API_KEY);
    
    if (!header_part1) {
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "DEBUG5 reached");
    size_t header1_len = strlen(header_part1);
    size_t footer_len  = strlen(footer);

    if (image_len > SIZE_MAX - header1_len - footer_len) {
        ESP_LOGE(TAG, "Overflow computing total_len");
        free(header_part1);
        return ESP_ERR_INVALID_SIZE;
    }

    size_t total_len = header1_len + image_len + footer_len;

    // DEBUG heap: quanto è libero e qual è il blocco massimo contiguo
    size_t free8    = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t largest8 = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    ESP_LOGI(TAG, "Need=%u free=%u largest=%u",
            (unsigned)total_len, (unsigned)free8, (unsigned)largest8);

    uint8_t *post_data = malloc(total_len);
    if (!post_data) {
        ESP_LOGE(TAG, "malloc(%u) failed", (unsigned)total_len);
        free(header_part1);
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "DEBUG6 reached");
    // Build complete POST body
    memcpy(post_data, header_part1, header1_len);
    memcpy(post_data + header1_len, image_data, image_len);
    memcpy(post_data + header1_len + image_len, footer, footer_len);
    
    // HTTP Client configuration
    char content_type[128];
    snprintf(content_type, sizeof(content_type), "multipart/form-data; boundary=----ESP32Boundary");
    ESP_LOGI(TAG, "DEBUG7 reached");
    esp_http_client_config_t config = {
        .url = SERVER_URL,
        .method = HTTP_METHOD_POST,
        .event_handler = http_event_handler,
        .timeout_ms = 30000,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .skip_cert_common_name_check = true, // opzionale
        };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    esp_http_client_set_header(client, "Content-Type", content_type);
    esp_http_client_set_header(client, "Authorization", API_KEY);
    esp_http_client_set_post_field(client, (char*)post_data, total_len);
    
    ESP_LOGI(TAG, "Before perform()");
    esp_err_t err = esp_http_client_perform(client);
    ESP_LOGI(TAG, "After perform()");
    ESP_LOGI(TAG, "perform() -> %s", esp_err_to_name(err));
    ESP_LOGI(TAG, "status=%d", esp_http_client_get_status_code(client));
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP POST Status = %d", status);
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
    free(post_data);
    free(header_part1);
    
    return err;
}

/* Capture and send image */
static void capture_and_recognize_plate(void) {
    ESP_LOGI(TAG, "Starting plate recognition...");
    
#ifdef CONFIG_USE_MOCK_CAMERA
    // MOCK VERSION: Use embedded image
    size_t image_size = mock_image_end - mock_image_start;
    ESP_LOGI(TAG, "Using MOCK image (%d bytes)", image_size);
    send_image_to_api(mock_image_start, image_size);
#else
    // REAL VERSION: Capture from camera
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        return;
    }
    
    ESP_LOGI(TAG, "Camera captured %d bytes", fb->len);
    send_image_to_api(fb->buf, fb->len);
    
    esp_camera_fb_return(fb);
#endif
    
    rgb_off();
}

void app_main(void) {

    while (1) {
        if (curr_state < STATES_NUM) {
            fsm[curr_state].state_function();
            /**
             * Transition logic (simplified for illustration)
             * TODO: Implement proper event detection and state transition logic
             */
            if (curr_state == INIT) {

                /**
                 * Example code to demonstrate the system operation
                 */
                #ifdef CONFIG_USE_MOCK_CAMERA
                ESP_LOGI(TAG, "Running in MOCK CAMERA mode (Wokwi simulation)");
                #endif

                ESP_LOGI(TAG, "System ready. Waiting for detection...");
                int32_t weight_in_g = 0;
                int32_t previous_weight = 0;

                while (1)
                {   
                    
                    weight_in_g = weight_read();

                    if (weight_in_g < 1000 || abs((int32_t)weight_in_g - (int32_t)previous_weight) < 200)  
                    {
                        vTaskDelay(pdMS_TO_TICKS(500));
                    } else {
                        ESP_LOGI(TAG, "Vehicle detected! Weight: %d g", (int32_t)weight_in_g);
                        rgb_green();  // Green LED = vehicle detected
                        vTaskDelay(pdMS_TO_TICKS(500)); // Debounce delay
                        capture_and_recognize_plate();
                    }

                    vTaskDelay(pdMS_TO_TICKS(200));
                    previous_weight = weight_in_g;
                }

                curr_state = IDLE;
            }

        } else {
            ESP_LOGE("FSM", "Invalid state: %d", curr_state);
            break;
        }
    }
    

}
