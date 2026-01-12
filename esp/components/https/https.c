/**
 * @file https.c
 * 
 * Interface used to manage HTTPS operations and API calls
 * 
 */

#include "https.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_crt_bundle.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/param.h>

#define SERVER_URL "https://tinyparkingsystem-api.vercel.app/"
#define MAX_HTTP_OUTPUT_BUFFER 1024

static const char *TAG = "HTTPS Module";
static char response_buffer[MAX_HTTP_OUTPUT_BUFFER];
static int response_len = 0;

// The event handler, which collects and saves 1KB chunks of response data for each HTTPS request
static esp_err_t http_event_handler(esp_http_client_event_handle_t evt)
{
    if (evt -> event_id == HTTP_EVENT_ON_DATA && evt -> data_len > 0) {
        size_t copy_len = evt->data_len;

        if (response_len + copy_len >= MAX_HTTP_OUTPUT_BUFFER) {
            copy_len = MAX_HTTP_OUTPUT_BUFFER - response_len - 1;
            ESP_LOGW(TAG, "Response buffer full, truncating");
        }

        memcpy(response_buffer + response_len, evt -> data, copy_len);
        response_len += copy_len;
    }

    return ESP_OK;
}

// Formats the response buffer as pretty JSON and prints it to console
void print_response_buffer() {
    printf("\n---------- Response content: -------------\n\n");

    cJSON *root = cJSON_Parse(response_buffer);

    if (root == NULL) {
        printf("%s", response_buffer); // fallback
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
 * @brief Generic method used to perform all of the needed GET/POST/PUT requests
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t perform_https_request(const char *url, esp_http_client_method_t method, const char *payload)
{
    // Reset the response buffer
    response_len = 0;
    memset(response_buffer, 0, sizeof(response_buffer));

    // Configures the HTTPS client
    esp_http_client_config_t config = {
        .url = url,
        .method = method,
        .event_handler = http_event_handler,
        .timeout_ms = 5000,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    // Initializes the HTTPS client
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Setting headers and request body if payload is provided
    if (payload != NULL) {
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_post_field(client, payload, strlen(payload));
    }

    // Performs the HTTPS request
    esp_err_t err = esp_http_client_perform(client);

    // Truncate response buffer to a null-terminated string
    if (response_len < sizeof(response_buffer)) {
        response_buffer[response_len] = '\0';
    } else {
        response_buffer[sizeof(response_buffer) - 1] = '\0';
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
 * @brief Initializes the HTTPS client (by testing a simple GET request)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t https_init(void)
{
    ESP_LOGI(TAG, "called https_init(), performing test GET request...");
    
    esp_err_t err = perform_https_request(
        "https://httpbin.org/get",
        HTTP_METHOD_GET,
        NULL
    );
    
    print_response_buffer();

    return err;
}

/**
 * @brief Performs a GET request to /status
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t https_get_status(void)
{
    ESP_LOGI(TAG, "performing GET request to /status...");

    // Defining the URL for the request
    char url[128];
    snprintf(url, sizeof(url), "%sstatus", SERVER_URL);
    
    esp_err_t err = perform_https_request(
        url,
        HTTP_METHOD_GET,
        NULL
    );
    
    print_response_buffer();

    return err;
}

/**
 * @brief Performs a PUT request to /status with a JSON payload
 * @param json_payload JSON formatted string to send in the request body
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t https_put_status(const char *json_payload) {
    ESP_LOGI(TAG, "performing PUT request to /status with payload: %s", json_payload);

    // Defining the URL for the request
    char url[128];
    snprintf(url, sizeof(url), "%sstatus", SERVER_URL);
    
    esp_err_t err = perform_https_request(
        url,
        HTTP_METHOD_PUT,
        json_payload
    );
    
    print_response_buffer();

    return err;
}

/**
 * @brief Performs a POST to /entry with a JSON payload. On success this returns the entryId
 * @param json_payload JSON formatted string to send in the request body
 * @return The entryId on success, NULL otherwise
 */
char* https_post_entry(const char *json_payload) {
    ESP_LOGI(TAG, "performing POST request to /entry with payload: %s", json_payload);

    // Defining the URL for the request
    char url[128];
    snprintf(url, sizeof(url), "%sentry", SERVER_URL);
    
    esp_err_t err = perform_https_request(
        url,
        HTTP_METHOD_POST,
        json_payload
    );
    
    print_response_buffer();

    char *entry_id = NULL;

    // Response handling
    if (err == ESP_OK) {
        // TO-DO: write a proper JSON parsing logic to extract entryId!
        entry_id = strdup(response_buffer);
    }
    
    return entry_id;
}

/**
 * @brief Performs a POST to /entry/{entryID} with a JSON payload, passing the entryId in the URL path
 * @param entry_id The entry ID to include in the URL path
 * @param json_payload JSON formatted string to send in the request body
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t https_post_entry_id(const char *entry_id, const char *json_payload) {
    ESP_LOGI(TAG, "performing POST request to /entry/entryId:%s with payload: %s", entry_id, json_payload);

    // Defining the URL for the request
    char url[128];
    snprintf(url, sizeof(url), "%sentry/%s", SERVER_URL, entry_id);
    
    esp_err_t err = perform_https_request(
        url,
        HTTP_METHOD_POST,
        json_payload
    );
    
    print_response_buffer();

    return err;
}

/**
 * @brief Performs a POST to /exit with a JSON payload
 * @param json_payload JSON formatted string to send in the request body
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t https_post_exit(const char *json_payload) {
    ESP_LOGI(TAG, "performing POST request to /exit with payload: %s", json_payload);

    // Defining the URL for the request
    char url[128];
    snprintf(url, sizeof(url), "%sexit", SERVER_URL);
    
    esp_err_t err = perform_https_request(
        url,
        HTTP_METHOD_POST,
        json_payload
    );
    
    print_response_buffer();

    return err;
}