/**
 * @file ultrasonic.c
 * 
 * Interface used to manage the ultrasonic sensor
 * 
 */

#include "ultrasonic_sensor.h"
#include "ultrasonic.h"
#include "esp_log.h"
#include "esp_err.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/param.h>

// Ultrasonic sensor pin definitions
#define TRIG_GPIO   GPIO_NUM_3
#define ECHO_GPIO   GPIO_NUM_20

#define MAX_DISTANCE 50
#define THRESHOLD_DISTANCE 10

static const char *TAG = "Ultrasonic Module";
static ultrasonic_sensor_t ultrasonic = {
    .trigger_pin = TRIG_GPIO,
    .echo_pin = ECHO_GPIO
};
static uint32_t last_distance = -1;

esp_err_t ultrasonic_sensor_init()
{
    ESP_LOGI(TAG, "Initializing ultrasonic sensor...");

    esp_err_t err = ultrasonic_init(&ultrasonic);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Ultrasonic sensor init failed: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Ultrasonic sensor initialized successfully");
    return ESP_OK;
}

bool ultrasonic_sensor_detect()
{
    uint32_t distance = 0;
    esp_err_t err = ultrasonic_measure_cm(&ultrasonic, MAX_DISTANCE, &distance);

    if (err != ESP_OK) {
        return false;
    }

    if (distance != last_distance) {
        ESP_LOGI(TAG, "detected object from %d cm distance", distance);
        last_distance = distance;
    }

    return (distance > 0 && distance < THRESHOLD_DISTANCE);
}