/**
 * @file init.c
 * 
 * This file cointains the main initialization
 * functions for the hardware peripherals
 * and the overall components of the system
 */


#include "init.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_camera.h"
#include "esp_psram.h"
#include "ultrasonic.h"

// Ultrasonic sensor pin definitions
#define TRIG_GPIO   GPIO_NUM_5
#define ECHO_GPIO   GPIO_NUM_18

// Camera pin definitions
#define CAM_PWDN 38
#define CAM_RESET -1   //software reset will be performed
#define CAM_VSYNC 6
#define CAM_HREF 7
#define CAM_PCLK 13
#define CAM_XCLK 15
#define CAM_SIOD 4
#define CAM_SIOC 5
#define CAM_D0 11
#define CAM_D1 9
#define CAM_D2 8
#define CAM_D3 10
#define CAM_D4 12
#define CAM_D5 18
#define CAM_D6 17
#define CAM_D7 16


void hw_init()
{
    camera_init();
    ultrasonic_init();
    weight_sensor_init();
}

void camera_init()
{
    const char *TAG = "CAM_INIT";
    ESP_LOGI(TAG, "PSRAM size: %d bytes", esp_psram_get_size());

    camera_config_t config = {
        .pin_pwdn  = CAM_PWDN,
        .pin_reset = CAM_RESET,
        .pin_xclk = CAM_XCLK,
        .pin_sccb_sda = CAM_SIOD,
        .pin_sccb_scl = CAM_SIOC,

        .pin_d7 = CAM_D7,
        .pin_d6 = CAM_D6,
        .pin_d5 = CAM_D5,
        .pin_d4 = CAM_D4,
        .pin_d3 = CAM_D3,
        .pin_d2 = CAM_D2,
        .pin_d1 = CAM_D1,
        .pin_d0 = CAM_D0,
        .pin_vsync = CAM_VSYNC,
        .pin_href = CAM_HREF,
        .pin_pclk = CAM_PCLK,

        .xclk_freq_hz = 20000000,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = PIXFORMAT_JPEG,
        .frame_size   = FRAMESIZE_QVGA,
        .jpeg_quality = 12,
        .fb_count = 2,
        .fb_location = CAMERA_FB_IN_PSRAM
    };

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed: 0x%x", err);
        return;
    }

    ESP_LOGI(TAG, "Camera initialized successfully");

}

void ultrasonic_init()
{
    const char *TAG = "ULTRASONIC_INIT";
    static ultrasonic_sensor_t config = {
        .trigger_pin = TRIG_GPIO,
        .echo_pin = ECHO_GPIO
    };

    ESP_ERROR_CHECK(ultrasonic_init_sensor(&config));

    ESP_LOGI(TAG, "Ultrasonic sensor initialized successfully");
}

void weight_sensor_init()
{
}

void wifi_init()
{
    
}