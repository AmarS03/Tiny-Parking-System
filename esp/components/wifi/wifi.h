/**
 * @file wifi.h
 * 
 * Header file for WiFi service interface
 * 
 */
#ifndef WIFI_H
#define WIFI_H

#include <stdbool.h>

void wifi_init_service();

bool wifi_is_connected(); 

#endif /* WIFI_H */