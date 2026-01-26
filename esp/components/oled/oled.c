/**
 * @file oled.h
 * 
 * Utility functions for OLED display
 * 
 */

#include "oled.h"
#include "esp_log.h"

#define OLED_SDA_GPIO GPIO_NUM_9
#define OLED_SCL_GPIO GPIO_NUM_8


static const char *TAG = "OLED";

static SSD1306_t dev;


/**
 * @brief Initializes the OLED display over I2C
 * @param i2c_port The I2C port number to use
 */
esp_err_t oled_init(i2c_port_t i2c_port)
{
    // Initialize I2C master
    dev._i2c_num = i2c_port;
    i2c_master_init(&dev, OLED_SDA_GPIO, OLED_SCL_GPIO, -1);

    // Initialize SSD1306 (I2C, 128x64)
    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xFF);

    ESP_LOGI(TAG, "SSD1306 initialized");

    return ESP_OK;
}

void oled_clear(void)
{
    ssd1306_clear_screen(&dev, false);
}

void oled_print(uint8_t row, const char *text)
{
    // row: 0–7 for 128x64
    ssd1306_display_text(&dev, row, (char *)text, strlen(text), false);
}
