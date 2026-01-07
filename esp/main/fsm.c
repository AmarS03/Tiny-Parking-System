/*
 * fsm.c
 *
 * Function definitions for the various state
 * of the system
 *
 *  Created on: Dec 3, 2025
 *      Author: mirco
 */

#include "fsm.h"
#include "init.h"


/**
 * Initializes hardware and other software
 * components
 */
void init_fn() {
    hw_init();
    //other initializations
    wifi_init();
}


/**
 * When in idle state, the system enters low
 * power mode and waits for interrupts
 */
void idle_fn() {

}

/**
 * Detects weight and runs the CV algorithm.
 * Decide whether to allow or refuse entrance
 */
void entry_fn() {

}

/**
 * Blinks the LED reda and shows a negative
 * message on the LCD
 */
void refuse_fn() {

}

/**
 * Blinks the LED green, shows a positive message
 * on the LCD and opens the gate bar
 */
void allow_fn() {

}

/**
 * Opens the gate bar for vehicle exit
 */
void exit_fn() {

}

/**
 * Disables interrupts and shows an info
 * message on the LCD
 */
void closed_fn() {

}

