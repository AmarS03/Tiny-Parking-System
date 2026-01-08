/**
 * @file weight.c
 * 
 * Clean interface to calibrate and read weight sensor
 * 
 */
#include "weight.h"
#include "hx711.h"
#include "esp_log.h"

// Weight sensor pin definitions
#define HX711_DOUT_GPIO  GPIO_NUM_1
#define HX711_CLK_GPIO   GPIO_NUM_2


// Prototypes
static int32_t weight_compute_offset(size_t samples);


static hx711_t config = {
        .dout = HX711_DOUT_GPIO,
        .pd_sck = HX711_CLK_GPIO,
        .gain = HX711_GAIN_A_128
    };
static int32_t weight_offset = 0;

void weight_init()
{    
    const char *TAG = "WEIGHT_INIT";
    ESP_ERROR_CHECK(hx711_init(&config));

    // Calibrate weight sensor
    ESP_LOGI(TAG, "Calibrating weight sensor...");
    weight_offset = weight_compute_offset(20);

    ESP_LOGI(TAG, "Computed weight sensor offset: %d", weight_offset);
}

/**
 * @brief Compute weight sensor offset by averaging multiple samples
 * @param samples Number of samples to average
 * @return Computed offset value
 */
static int32_t weight_compute_offset(size_t samples)
{
    int64_t sum = 0;
    int32_t raw;

    for (size_t i = 0; i < samples; i++) {

        ESP_ERROR_CHECK(hx711_wait(&config, 5000));

        ESP_ERROR_CHECK(hx711_read_data(&config, &raw));

        sum += raw;
    }
    return sum / samples;
}

int32_t weight_read(void)
{
    int32_t raw;

    ESP_ERROR_CHECK(hx711_wait(&config, 1000));
    ESP_ERROR_CHECK(hx711_read_data(&config, &raw));

    return raw - weight_offset;
}






