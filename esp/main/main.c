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

#include "fsm.h"

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


void app_main(void) {
    xTaskCreate(fsm_task, "fsm_task", 8192, NULL, 5, NULL);
}
