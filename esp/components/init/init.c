/**
 * @file init.c
 * 
 * This file contains the main initialization
 * functions for the hardware peripherals
 * and the overall components of the system
*/

#include "init.h"
#include "../cv/cv.h"
#include "../https/https.h"
#include "../https/https_task.h"
#include "../ultrasonic_sensor/ultrasonic_sensor.h"
#include "../weight/weight.h"
#include "../wifi/wifi.h"
#include "../servo_motor/servo_motor.h"
#include "../oled/oled.h"

#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include <esp_idf_lib_helpers.h>

#ifndef CONFIG_USE_MOCK_CAMERA

#include "esp_camera.h"
#include "esp_psram.h"
#define CAM_PWDN GPIO_NUM_38
#define CAM_RESET -1   //software reset will be performed
#define CAM_VSYNC GPIO_NUM_6
#define CAM_HREF GPIO_NUM_7
#define CAM_PCLK GPIO_NUM_21
#define CAM_XCLK GPIO_NUM_15
#define CAM_SIOD GPIO_NUM_40
#define CAM_SIOC GPIO_NUM_39
#define CAM_D0 GPIO_NUM_11
#define CAM_D1 GPIO_NUM_14
#define CAM_D2 GPIO_NUM_16
#define CAM_D3 GPIO_NUM_10
#define CAM_D4 GPIO_NUM_2
#define CAM_D5 GPIO_NUM_1
#define CAM_D6 GPIO_NUM_17
#define CAM_D7 GPIO_NUM_3

#endif

// Servo motor pin definition
#define SERVO_PWM_GPIO   GPIO_NUM_18
#define SERVO_ANGLE_DOWN 0
#define SERVO_ANGLE_UP   90

/**
 * @brief Initializes the overall system components
 * and creates necessary tasks: also sends the various
 * statuses to the backend api.
 */
void system_init()
{
    // Initialize WiFi first (for NVS)
    esp_err_t wifi_status = wifi_init();

    esp_err_t camera_status = ESP_OK;

    #ifndef CONFIG_USE_MOCK_CAMERA
    camera_status = camera_init();
    #endif

    esp_err_t ultrasonic_status = ultrasonic_sensor_init();
    esp_err_t weight_status = weight_sensor_init();
    esp_err_t servo_status = servo_init();
    esp_err_t oled_status = oled_init(I2C_NUM_1);
    
    set_status_variables(camera_status, ultrasonic_status, weight_status, servo_status, wifi_status, oled_status);

    xTaskCreate(put_status_task, "put_status_task", 8192, NULL, 5, NULL);

    vTaskDelay(pdMS_TO_TICKS(5000));

    xTaskCreatePinnedToCore(weight_task, "weight_task", 8192, NULL, tskIDLE_PRIORITY + 1, NULL, 1);

    cv_task_creator();
}


#ifndef CONFIG_USE_MOCK_CAMERA
esp_err_t camera_init()
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
        return err;
    }

    sensor_t * s = esp_camera_sensor_get();
    s->set_vflip(s, 1); // flip vertically
    s->set_brightness(s, 2); // up the brightness just a bit

    ESP_LOGI(TAG, "Camera initialized successfully");
    return ESP_OK;
}
#endif

esp_err_t weight_sensor_init()
{
    const char *TAG = "WEIGHT_SENSOR_INIT";

    esp_err_t err = weight_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Weight sensor init failed: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Weight sensor initialized successfully");
    return ESP_OK;
}

esp_err_t servo_init(void)
{
    return servo_motor_init(&(servo_motor_params_t){
        .gpio_pwm = SERVO_PWM_GPIO,
        .angle_up_deg = SERVO_ANGLE_UP,
        .angle_down_deg = SERVO_ANGLE_DOWN,
    });
}

/**
 * @brief Initializes the WiFi service and
 * connects to the configured access point
 */
esp_err_t wifi_init()
{
    esp_err_t res = wifi_init_service();

    while (!wifi_is_connected()) {
        ESP_LOGI("WIFI_INIT", "Waiting for WiFi connection...");
        res = wifi_init_service();
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    ESP_LOGI("WIFI_INIT", "WiFi connected successfully");

    return res;
}
