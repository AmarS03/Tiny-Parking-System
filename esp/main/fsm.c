/*
 * fsm.c
 *
 * Function definitions for the various states of the system
 *
 */

#include "fsm.h"
#include "init.h"
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


#include "../components/cv/cv.h"
#include "../components/weight/weight.h"
#include "../components/https/https.h"

static const char *TAG = "Tiny Parking FSM";

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

void https_task(void *arg)
{
    ESP_LOGI(TAG, "HTTPS task started");
    
    // TESTING: per vedere se funziona il modulo HTTPS
    ESP_LOGI(TAG, "----- HTTPS TESTING -----");
    esp_err_t err = https_init();

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS GET /status succeeded");
    } else {
        ESP_LOGE(TAG, "HTTPS GET /status failed: %s", esp_err_to_name(err));
    } 

    vTaskDelete(NULL);
}

/**
 * Initializes hardware and other software
 * components
 */
void init_fn() {
    hw_init();
    //other initializations
    wifi_init();

    xTaskCreate(https_task, "https_init", 8192, NULL, 6, NULL);

    vTaskDelay(pdMS_TO_TICKS(5000));

    xTaskCreate(weight_task, "weight_task", 8192, NULL, 5, NULL);

    //xTaskCreate(recognition_task, "recognition_task", 12288, NULL, 5, &recognition_task_handle);

    cv_task_creator();

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
    enable_weight_detection(true);
    recognition_busy = false;
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
    vTaskDelay(pdMS_TO_TICKS(5000));
    // Open gate bar logic here

    curr_state = IDLE;
}

/**
 * Opens the gate bar for vehicle exit
 * and waits for a signal from the weight 
 * sensor to close it again
 */
void exit_fn() {

}

/**
 * Disables interrupts and shows an info
 * message on the LCD
 */
void closed_fn() {

}

