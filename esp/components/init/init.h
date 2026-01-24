/**
 * @file init.h
 * 
 * Initilization functions header file
 * 
 */



#ifndef INIT_H
#define INIT_H

#include "esp_err.h"

void system_init();

esp_err_t camera_init();

esp_err_t ultrasonic_sensor_init();

esp_err_t weight_sensor_init();

esp_err_t wifi_init();

esp_err_t servo_init(void);

void send_system_status_to_api(esp_err_t camera_status, esp_err_t ultrasonic_status, esp_err_t weight_status, esp_err_t servo_status, esp_err_t wifi_status);

#endif /* INIT_H */