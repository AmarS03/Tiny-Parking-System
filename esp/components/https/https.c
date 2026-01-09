/**
 * @file https.c
 * 
 * Interface used to manage HTTPS operations and API calls
 * 
 */

#include "https.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_err.h"
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define SERVER_URL "https://192.168.1.74:5000/"
#define MAX_HTTP_OUTPUT_BUFFER 1024

static const char *TAG = "HTTPS Module:";

// The event handler, which collects and saves 1KB chunks of response data for each HTTP request
static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt -> event_id) {
        case HTTP_EVENT_ON_DATA:
            if (evt -> user_data && evt -> data_len > 0) {
                strncat(
                    (char *) evt -> user_data,
                    (char *) evt -> data,
                    MAX_HTTP_OUTPUT_BUFFER - strlen(evt -> user_data) - 1
                );
            }
            break;
        default:
            break;
    }

    return ESP_OK;
}

/**
 * @brief Generic method used to perform all of the needed GET/POST/PUT requests
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t perform_https_request(const char *url, esp_http_client_method_t method, const char *payload, char *response_buffer)
{
    // Configures the HTTP client
    esp_http_client_config_t config = {
        .url = url,
        .method = method,
        .event_handler = _http_event_handler,
        .user_data = response_buffer,
        .timeout_ms = 5000,
        .skip_cert_common_name_check = true,  // Skips certificate verification
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
    };

    // Initializes the HTTP client
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Setting headers and request body if payload is provided
    if (payload != NULL) {
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_post_field(client, payload, strlen(payload));
    }

    // Performs the HTTP request
    esp_err_t err = esp_http_client_perform(client);

    // Response handling
    if (err == ESP_OK) {
        ESP_LOGI(
            TAG,
            " response with status %d, content_length = %" PRId64,
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client)
        );
    } else {
        ESP_LOGE(TAG, "request failed: %s", esp_err_to_name(err));
    }

    // Cleans up the HTTP client
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
    char response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    
    esp_err_t err = perform_https_request(
        "https://jsonplaceholder.typicode.com/todos/1",
        HTTP_METHOD_GET,
        NULL,
        response_buffer
    );
    
    ESP_LOGV(TAG, "response content: %s", response_buffer);

    return err;
}

/**
 * @brief Performs a GET request to /status
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t https_get_status(void)
{
    ESP_LOGI(TAG, "performing GET request to /status...");

    // Buffer to store the response of the HTTP request
    char response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    // Defining the URL for the request
    char url[128];
    snprintf(url, sizeof(url), "%sstatus", SERVER_URL);
    
    esp_err_t err = perform_https_request(
        url,
        HTTP_METHOD_GET,
        NULL,
        response_buffer
    );

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
        json_payload,
        NULL
    );

    return err;
}

/**
 * @brief Performs a POST to /entry with a JSON payload. On success this returns the entryId
 * @param json_payload JSON formatted string to send in the request body
 * @return The entryId on success, NULL otherwise
 */
char* https_post_entry(const char *json_payload) {
    ESP_LOGI(TAG, "performing POST request to /entry with payload: %s", json_payload);

    // Buffer to store the response of the HTTP request
    char response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    // Defining the URL for the request
    char url[128];
    snprintf(url, sizeof(url), "%sentry", SERVER_URL);
    
    esp_err_t err = perform_https_request(
        url,
        HTTP_METHOD_POST,
        json_payload,
        response_buffer
    );

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
        json_payload,
        NULL
    );

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
        json_payload,
        NULL
    );

    return err;
}