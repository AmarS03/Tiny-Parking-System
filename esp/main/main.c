#include <stdio.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_camera.h"
#include "esp_psram.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fsm.h"

//variables
Event_t event = NONE;

State_t curr_state = INIT;

Fsm_t fsm[] = {
               {INIT, init_fn},
               {IDLE, idle_fn},
               {VEHICLE_ENTRY, entry_fn},
               {ENTRY_REFUSED, refuse_fn},
               {ENTRY_ALLOWED, allow_fn},
               {VEHICLE_EXIT, exit_fn},
               {CLOSED, closed_fn}
};

static const char *TAG = "CAM_TEST";

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

void app_main(void)
{
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

    while (1) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
        } else {
            ESP_LOGI(TAG, "Captured frame: %dx%d, %d bytes",
                     fb->width, fb->height, fb->len);
            esp_camera_fb_return(fb);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

}
