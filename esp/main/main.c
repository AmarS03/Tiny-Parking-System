/**
 * ESP Main Application
 * 
 * This file contains the main entry point for the ESP32 application.
 * The function is kept minimal, as the FSM task handles the business logic,
 * while this main function is responsible for initializing the system
 * and starting the FreeRTOS scheduler.
 */


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#include "fsm.h"
#include "../components/weight/weight.h"

/**
 * FSM Task
 * This task runs the finite state machine, executing state functions
 * in a loop with a small delay to allow other tasks to run.
 */
void fsm_task(void *arg)
{
    while (1) {
        fsm_run_state_function();   // executes one state iteration
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

#ifdef CONFIG_WEIGHT_CALIBRATION
/**
 * Calibration Task
 * This task performs weight calibration at startup.
 * It's meant to be run once and then deleted.
 */
void calibration_task(void *arg)
{
    // initialize NVS before anything else
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Perform weight calibration with a known weight of 1000 grams
    weight_calibrate(35.0f);
    vTaskDelete(NULL); // Delete this task after calibration
}
#endif


void app_main(void) {
    #ifdef CONFIG_WEIGHT_CALIBRATION
    // Start calibration task
    xTaskCreate(calibration_task, "calibration_task", 8192, NULL, 5, NULL);
    #else

    xTaskCreate(fsm_task, "fsm_task", 8192, NULL, 5, NULL);
    #endif
}
