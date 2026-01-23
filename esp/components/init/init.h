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

void hw_init();

void camera_init();

void ultrasonic_sensor_init();

void weight_sensor_init();

void wifi_init();

void servo_init(void);

esp_err_t i2c_ng_init(void);

#endif /* INIT_H */