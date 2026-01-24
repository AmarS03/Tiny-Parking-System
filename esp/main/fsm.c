/*
 * fsm.c
 *
 * Function definitions for the various states of the system
 *
 */

#include "fsm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "ultrasonic.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_sleep.h"

#include <stdbool.h>

#include "../components/cv/cv.h"
#include "../components/weight/weight.h"
#include "../components/ultrasonic_sensor/ultrasonic_sensor.h"
#include "../components/https/https.h"
#include "../components/init/init.h"
#include "../components/servo_motor/servo_motor.h"

// Idle delay function for low power mode
#ifdef CONFIG_USE_MOCK_CAMERA
    #define IDLE_DELAY() vTaskDelay(pdMS_TO_TICKS(200))
#else
    #define IDLE_DELAY() do { \
        esp_sleep_enable_timer_wakeup(200000); \
        esp_light_sleep_start(); \
    } while(0)
#endif

// Current state of the FSM
static State_t curr_state = INIT;

// Flag that avoids multiple concurrent recognitions
static bool recognition_busy = false;

// Prototypes of state functions
static void init_fn();

static void idle_fn();

static void entry_fn();

static void refuse_fn();

static void allow_fn();

static void exit_fn();

static void closed_fn();


////////////////////////////////////////////////////////////////
//////////////// FSM Logic /////////////////////////////////////
////////////////////////////////////////////////////////////////

void fsm_handle_event(Event_t event) {
    switch (curr_state) {
        case IDLE:
            if (event == VALID_WEIGHT_DETECTED) {
                curr_state = VEHICLE_ENTRY;
            } else if (event == EXIT_DETECTED) {
                curr_state = VEHICLE_EXIT;
            } else if (event == REMOTE_OPEN) {
                curr_state = ENTRY_ALLOWED;
            }
            break;
        case VEHICLE_ENTRY:
            if (event == PLATE_RECOGNIZED) {
                curr_state = ENTRY_ALLOWED;
            } else if (event == PLATE_REFUSED) {
                curr_state = ENTRY_REFUSED;
            }
            break;
        case CLOSED:    
            break;
        default:
            // Handle invalid state
        break;
    }

}

void fsm_run_state_function() {
    switch (curr_state) {
        case INIT:
            init_fn();
            break;
        case IDLE:
            idle_fn();
            break;
        case VEHICLE_ENTRY:
            entry_fn();
            break;
        case ENTRY_REFUSED:
            refuse_fn();
            break;
        case ENTRY_ALLOWED:
            allow_fn();
            break;
        case VEHICLE_EXIT:
            exit_fn();
            break;
        case CLOSED:
            closed_fn();
            break;
        default:
            // Handle invalid state
            break;
    }
}

////////////////////////////////////////////////////////////////
//////////////// FSM State Functions ///////////////////////////
////////////////////////////////////////////////////////////////

/**
 * Initializes the whole system
 */
void init_fn() {
    system_init();

    #ifdef CONFIG_USE_MOCK_CAMERA
    ESP_LOGI("IDLE", "Running in MOCK CAMERA mode (Wokwi simulation)");
    #endif

    ESP_LOGI("INIT", "System ready. Waiting for detection...");

    curr_state = IDLE;
}

/**
 * When in idle state, the system enters low
 * power mode and waits for interrupts
 */
void idle_fn() {
    // System waiting for an event
    //enable_weight_detection(true);
    recognition_busy = false;

    // Enter low power mode until an interrupt occurs
    IDLE_DELAY();

    // wait for the ultrasonic sensor to detect vehicle passage
    if (ultrasonic_sensor_detect()) {
        ESP_LOGI("IDLE", "Detected vehicle exiting...");
        vTaskDelay(pdMS_TO_TICKS(500)); // Debounce delay
        fsm_handle_event(EXIT_DETECTED);
    }
}

/**
 * Detects weight and runs the CV algorithm.
 * Decide whether to allow or refuse entrance
 */
void entry_fn() {
    enable_weight_detection(false);

    if (!recognition_busy) {
        unblock_recognition_task();
        recognition_busy = true;
    }
}

/**
 * Blinks the LED red and shows a negative
 * message on the LCD
 */
void refuse_fn() {
    ESP_LOGI("REFUSE", "Entry refused. Access denied.");
    vTaskDelay(pdMS_TO_TICKS(5000));
    curr_state = IDLE;
}

/**
 * Shows a positive message on the
 * LCD, opens the gate bar and waits a signal
 * from the ultrasonic sensor to close it again
 */
void allow_fn() {
    ESP_LOGI("ALLOW", "Entry allowed. Opening gate...");

    // raise the barrier when entry is allowed
    servo_motor_raise_barrier();

    // wait for the ultrasonic sensor to detect vehicle passage
    bool is_still_detecting = false;

    // Await vehicle passage
    while (!is_still_detecting) {
        is_still_detecting = ultrasonic_sensor_detect();
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    ESP_LOGI("ALLOW", "Vehicle passing the gate...");

    // Await vehicle clearance
    while (is_still_detecting) {
        is_still_detecting = ultrasonic_sensor_detect();
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    ESP_LOGI("ALLOW", "Vehicle passed. Closing gate...");
    
    // close the barrier after ultrasonic read
    servo_motor_lower_barrier();
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    curr_state = IDLE;
}

/**
 * Opens the gate bar for vehicle exit
 * and waits for a signal from the weight 
 * sensor to close it again
 */
void exit_fn() {
    enable_weight_detection(false);
    ESP_LOGI("EXIT", "Exit allowed. Opening gate...");

    //raise the barrier when vehicle exit is detected
    servo_motor_raise_barrier();

    // wait for some time to allow vehicle to exit
    vTaskDelay(pdMS_TO_TICKS(5000));

    // clsose the barrier after delay
    servo_motor_lower_barrier();
    
    ESP_LOGI("EXIT", "Vehicle passed. Closing gate...");
    
    curr_state = IDLE;
}

/**
 * Disables interrupts and shows an info
 * message on the LCD
 */
void closed_fn() {

    // close the barrier when the system force closes
    servo_motor_raise_barrier();
}

