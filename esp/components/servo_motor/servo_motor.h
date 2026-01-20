#pragma once
#include "esp_err.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    gpio_num_t gpio_pwm;       // es. GPIO_NUM_14
    float angle_up_deg;        // 90
    float angle_down_deg;      // 0
} servo_motor_params_t;

esp_err_t servo_motor_init(const servo_motor_params_t *p);
esp_err_t servo_motor_raise_barrier(void);
esp_err_t servo_motor_lower_barrier(void);
esp_err_t servo_motor_set_angle(float angle_deg);
esp_err_t servo_motor_deinit(void);

#ifdef __cplusplus
}
#endif
