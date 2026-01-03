#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "ultrasonic.h"

#define TRIG_GPIO   GPIO_NUM_5
#define ECHO_GPIO   GPIO_NUM_18

#define LED_R_GPIO  GPIO_NUM_6
#define LED_G_GPIO  GPIO_NUM_7
#define LED_B_GPIO  GPIO_NUM_8

#define MAX_DISTANCE_CM 500

static ultrasonic_sensor_t sensor = {
  .trigger_pin = TRIG_GPIO,
  .echo_pin = ECHO_GPIO
};

static void rgb_off(void) {
  // Common anode: HIGH = spento
  gpio_set_level(LED_R_GPIO, 1);
  gpio_set_level(LED_G_GPIO, 1);
  gpio_set_level(LED_B_GPIO, 1);
}

static void rgb_green(void) {
  // Common anode: LOW = acceso
  gpio_set_level(LED_R_GPIO, 1);
  gpio_set_level(LED_G_GPIO, 0);
  gpio_set_level(LED_B_GPIO, 1);
}

static void rgb_init(void) {
  gpio_config_t io = {0};
  io.pin_bit_mask = (1ULL << LED_R_GPIO) | (1ULL << LED_G_GPIO) | (1ULL << LED_B_GPIO);
  io.mode = GPIO_MODE_OUTPUT;
  io.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&io);

  rgb_off();
}

void app_main(void) {
  rgb_init();

  // init sensore ultrasuoni
  ESP_ERROR_CHECK(ultrasonic_init(&sensor));

  while (1) {
    uint32_t distance_cm = 0;
    esp_err_t res = ultrasonic_measure_cm(&sensor, MAX_DISTANCE_CM, &distance_cm);

    if (res != ESP_OK) {
      printf("Errore misura (res=0x%x)\n", (unsigned)res);
      rgb_off();
    } else {
      printf("Distanza: %u cm\n", (unsigned)distance_cm);

      if (distance_cm > 200) {
        rgb_green();
      } else {
        rgb_off();
      }
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
