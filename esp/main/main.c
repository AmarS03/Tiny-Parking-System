#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "fsm.h"

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
