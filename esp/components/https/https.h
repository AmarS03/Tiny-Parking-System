/**
 * @file https.h
 * 
 * Header file for the HTTPS interface module
 * 
 */

#ifndef HTTPS_H
#define HTTPS_H

#include "esp_err.h"
#include "esp_http_client.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Generic method used to perform a REST API request to a specific URL
esp_err_t perform_https_request(const char *url, esp_http_client_method_t method, const char *payload);

// Initializes the HTTPS client (by testing a simple GET request)
esp_err_t https_init(void);

// Performs a GET request to /status
esp_err_t https_get_status(void);

// Performs a PUT request to /status with a JSON payload
esp_err_t https_put_status(const char *json_payload);

// Performs a POST to /entry with a JSON payload and returns if the entry was allowed or not
bool https_post_entry(const char *json_payload);

// Performs a POST to /exit with a JSON payload
esp_err_t https_post_exit(const char *json_payload);

// Sends an image to the external API for plate recognition and returns the plate string
char* plate_recognition_api(const uint8_t *image_data, size_t image_len);

void https_task(void *arg);
#endif /* HTTPS_H */