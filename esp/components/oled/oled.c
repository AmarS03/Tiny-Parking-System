/**
 * @file oled.h
 * 
 * Utility functions for OLED display
 * 
 */

#include "oled.h"
#include "esp_log.h"


static const char *TAG = "OLED";

static SSD1306_t dev;


/**
 * @brief Initializes the OLED display over I2C
 * @param i2c_port The I2C port number to use
 */
void oled_init(i2c_port_t i2c_port)
{
    // Initialize I2C master
    i2c_master_init(&dev, GPIO_NUM_42, GPIO_NUM_41, -1);

    // Initialize SSD1306 (I2C, 128x64)
    ssd1306_init(&dev, 128, 64);

    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xFF);

    ESP_LOGI(TAG, "SSD1306 initialized");
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
