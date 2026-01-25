/**
 * @file http_task.h
 * 
 * Interface for the various https calls
 * 
 */

#ifndef HTTPS_TASK_H
#define HTTPS_TASK_H

#include "esp_err.h"

void put_status_task(void *arg);

void set_status_variables(esp_err_t camera_status, esp_err_t ultrasonic_status, esp_err_t weight_status, esp_err_t servo_status, esp_err_t wifi_status, esp_err_t oled_status);

void send_system_status_to_api();

void post_entry_task(void *arg);

void send_entry_to_api(void);

void set_license_plate_data(char * license_plate, char * image_url);

void set_weight_data(float * recorded_weight);

void post_exit_task(void *arg);

void send_exit_to_api(void);

void set_exit_variables(char * license_plate);

#endif