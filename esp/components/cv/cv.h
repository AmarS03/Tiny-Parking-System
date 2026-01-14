/**
 * @file cv.h
 * 
 * Header file for the computer vision API interface module
 * 
 */

#ifndef CV_H
#define CV_H

#include "esp_err.h"
#include "esp_http_client.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Performs the HTTPS request to the CV API, to send the image data
esp_err_t perform_cv_api_request(const char *payload, size_t payload_len);

// Prepares the image payload for the CV API request
esp_err_t prepare_image_payload(const uint8_t *image_data, size_t image_len);

// Extracts recognized plate string from last API response (caller must free)
char* extract_plate_from_response(void);

// Extracts view image URL from last API response (caller must free)
char* extract_image_link_from_response(void);

#endif /* CV_H */