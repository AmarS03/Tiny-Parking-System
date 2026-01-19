/**
 * @file wifi.h
 * 
 * Header file for WiFi service interface
 * 
 */
#ifndef WIFI_H
#define WIFI_H

#include <stdbool.h>

// Initializes the WiFi service and connects to the configured AP
void wifi_init_service();

// Checks if the WiFi is currently connected
bool wifi_is_connected(); 

#endif /* WIFI_H */