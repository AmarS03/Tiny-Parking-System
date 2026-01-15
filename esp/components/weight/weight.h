/**
 * @file weight.h
 * 
 * Header file for weight sensor interface
 * 
 */
#ifndef WEIGHT_H
#define WEIGHT_H

#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

// Initialize the weight sensor
esp_err_t weight_init(void);

// Read the weight in grams
float weight_read_grams(void);

// Check if a vehicle is detected based on weight threshold
bool weight_detect_vehicle(void);

// Calibrate the weight sensor with a known weight in grams
void weight_calibrate(float known_weight_g);

#endif /* WEIGHT_H */