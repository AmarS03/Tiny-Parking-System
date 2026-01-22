/**
 * @file ultrasonic.h
 * 
 * Header file for the ultrasonic sensor interface module
 * 
 */

#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <stdbool.h>

void ultrasonic_sensor_init();

bool ultrasonic_sensor_detect();

#endif /* ULTRASONIC_SENSOR_H */