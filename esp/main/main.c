#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "fsm.h"


void app_main(void) {

    while (1) {
        fsm_run_state_function();
        vTaskDelay(pdMS_TO_TICKS(100));    
    }
}
