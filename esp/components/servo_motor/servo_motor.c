#include "servo_motor.h"

#include <stdbool.h>
#include "iot_servo.h"          // da espressif/servo
#include "esp_check.h"

static bool s_inited = false;
static servo_motor_params_t s_p;

static servo_config_t s_cfg = {
    .max_angle = 180,
    .min_width_us = 500,
    .max_width_us = 2500,
    .freq = 50,
    .timer_number = LEDC_TIMER_0,
    .channels = {
        .servo_pin = { -1 },
        .ch = { LEDC_CHANNEL_0 },
    },
    .channel_number = 1,
};

esp_err_t servo_motor_init(const servo_motor_params_t *p)
{
    ESP_RETURN_ON_FALSE(p != NULL, ESP_ERR_INVALID_ARG, "servo_motor", "params null");

    s_p = *p;
    s_cfg.channels.servo_pin[0] = (int)s_p.gpio_pwm;

    esp_err_t err = iot_servo_init(LEDC_LOW_SPEED_MODE, &s_cfg);
    ESP_RETURN_ON_ERROR(err, "servo_motor", "iot_servo_init failed");

    s_inited = true;
    return servo_motor_lower_barrier(); // parti “giu”
}

esp_err_t servo_motor_set_angle(float angle_deg)
{
    ESP_RETURN_ON_FALSE(s_inited, ESP_ERR_INVALID_STATE, "servo_motor", "not inited");
    return iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, angle_deg);
}

esp_err_t servo_motor_raise_barrier(void)
{
    return servo_motor_set_angle(s_p.angle_up_deg);
}

esp_err_t servo_motor_lower_barrier(void)
{
    return servo_motor_set_angle(s_p.angle_down_deg);
}

esp_err_t servo_motor_deinit(void)
{
    if (!s_inited) return ESP_OK;
    s_inited = false;
    return iot_servo_deinit(LEDC_LOW_SPEED_MODE);
}
