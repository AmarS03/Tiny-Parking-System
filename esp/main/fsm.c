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
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "esp_heap_caps.h"
#include "esp_crt_bundle.h"

#include "esp_sleep.h"

#ifndef CONFIG_USE_MOCK_CAMERA
    #include "esp_camera.h"
#endif

#include "../components/cv/cv.h"
#include "../components/weight/weight.h"
#include "../components/https/https.h"

static const char *TAG = "Tiny Parking FSM";

// Mock image embedded (for Wokwi)
#ifdef CONFIG_USE_MOCK_CAMERA
extern const uint8_t mock_image_start[] asm("_binary_mock_plate_jpg_start");
extern const uint8_t mock_image_end[]   asm("_binary_mock_plate_jpg_end");
#endif

// Idle delay function for low power mode
#ifdef CONFIG_USE_MOCK_CAMERA
    #define IDLE_DELAY() vTaskDelay(pdMS_TO_TICKS(200))
#else
    #define IDLE_DELAY() do { \
        esp_sleep_enable_timer_wakeup(200000); \
        esp_light_sleep_start(); \
    } while(0)
#endif

#define YIELD() vTaskDelay(pdMS_TO_TICKS(10))

// Current state of the FSM
static State_t curr_state = INIT;

// Weight detection enabled flag
static volatile bool weight_enabled = true;

// Handle for recognition task (allows to block/unblock it)
static TaskHandle_t recognition_task_handle = NULL;
// Recognition busy flag
static volatile bool recognition_busy = false;


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
 * Plate recognition task
 * Waits for notification from weight task,
 * then captures image and sends to CV API.
 * The use of task notification allows to
 * efficiently block/unblock the task without
 * busy waiting and is generally more lightweight
 * than using semaphores or event groups.
 * Based on the result of the recognition, it
 * triggers the appropriate FSM event.
 */
void recognition_task(void *arg)
{   
    while (1) {
        // Block until weight detection signals entry
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        recognition_busy = true;

        ESP_LOGI(TAG, "Plate recognition task started...");
        
    #ifdef CONFIG_USE_MOCK_CAMERA
        // MOCK VERSION: Use embedded image
        size_t image_size = mock_image_end - mock_image_start;
        ESP_LOGI(TAG, "Using MOCK image (%d bytes)", image_size);
        prepare_image_payload(mock_image_start, image_size);
    #else
        // REAL VERSION: Capture from camera
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            return;
        }
        
        ESP_LOGI(TAG, "Camera captured %d bytes", fb->len);
        prepare_image_payload(fb->buf, fb->len);
    #endif

        YIELD();

        const char *plate = extract_plate_from_response();
        YIELD();
        const char *image_link = extract_image_link_from_response();

        if (plate != NULL && image_link != NULL) {
            ESP_LOGI(TAG, "===== PLATE DETECTED: %s =====", plate);
            ESP_LOGI(TAG, "===== IMAGE LINK: %s =====", image_link);

            fsm_handle_event(PLATE_RECOGNIZED);
        } else {
            ESP_LOGE(TAG, "Plate recognition failed");

            fsm_handle_event(PLATE_REFUSED);
        }
        
        free((void*) plate);
        free((void*) image_link);

    #ifndef CONFIG_USE_MOCK_CAMERA
        esp_camera_fb_return(fb);
    #endif

        recognition_busy = false;
    }
}

/**
 * Weight detection task
 * Continuously monitors the weight sensor (since
 * it can't generate an interrupt directly).
 * When a valid weight is detected, it triggers
 * the appropriate FSM event.
 */
void weight_task(void *arg) {
    ESP_LOGI(TAG, "Starting weight detection task...");

    while (1) {
        // Check if weight detection is enabled
        if (!weight_enabled) {
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }

        if (weight_detect_vehicle()) {
            ESP_LOGI(TAG, "Valid weight detected!");
            fsm_handle_event(VALID_WEIGHT_DETECTED);
        }
        // Sleep for 200 ms before next reading
        IDLE_DELAY();
    }
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

    xTaskCreate(recognition_task, "recognition_task", 12288, NULL, 5, &recognition_task_handle);

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
    weight_enabled = true;
}

/**
 * Detects weight and runs the CV algorithm.
 * Decide whether to allow or refuse entrance
 */
void entry_fn() {
    weight_enabled = false;
   
    if (recognition_task_handle != NULL && !recognition_busy) {
        // Unblock recognition task
        xTaskNotifyGive(recognition_task_handle);
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
 * Blinks the LED green, shows a positive message
 * on the LCD and opens the gate bar
 */
void allow_fn() {
    ESP_LOGI("ALLOW", "Entry allowed. Opening gate...");
    vTaskDelay(pdMS_TO_TICKS(5000));
    // Open gate bar logic here

    curr_state = IDLE;
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

