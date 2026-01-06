/**
 * @file wifi.h
 * 
 * WiFi initialization header file
 * 
 */
#ifndef WIFI_H
#define WIFI_H

#include <stdbool.h>

void wifi_init_service();

bool wifi_is_connected(); 

#endif /* WIFI_H */