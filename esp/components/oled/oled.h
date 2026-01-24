/**
 * @file oled.h
 * 
 * Header file for OLED display component
 * 
 */

#ifndef OLED_H
#define OLED_H

#include "ssd1306.h"
#include "driver/i2c_master.h"
#include <stdbool.h>

// Initialize the OLED display
void oled_init(i2c_port_t i2c_port);

// Clear the OLED display
void oled_clear(void);

// Wrapper to print text on the OLED display at specified row
void oled_print(uint8_t row, const char *text);


#endif /* OLED_H */