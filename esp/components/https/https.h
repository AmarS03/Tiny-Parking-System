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

// Generic method used to perform all of the needed GET/POST/PUT requests
esp_err_t perform_https_request(const char *url, esp_http_client_method_t method, const char *payload);

// Initializes the HTTPS client (by testing a simple GET request)
esp_err_t https_init(void);

// Performs a GET request to /status
esp_err_t https_get_status(void);

// Performs a PUT request to /status with a JSON payload
esp_err_t https_put_status(const char *json_payload);

// Performs a POST to /entry with a JSON payload and returns the entryId
char* https_post_entry(const char *json_payload);

// Performs a POST to /entry/{entryID} with a JSON payload, passing the entryId in the URL path
esp_err_t https_post_entry_id(const char *entry_id, const char *json_payload);

// Performs a POST to /exit with a JSON payload
esp_err_t https_post_exit(const char *json_payload);

#endif /* HTTPS_H */