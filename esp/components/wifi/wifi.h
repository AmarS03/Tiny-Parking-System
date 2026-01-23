/**
 * @file wifi.h
 * 
 * Header file for WiFi service interface
 * 
 */
#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"
#include <stdbool.h>

// Initializes the WiFi service and connects to the configured AP
esp_err_t wifi_init_service();

// Checks if the WiFi is currently connected
bool wifi_is_connected(); 

#endif /* WIFI_H */