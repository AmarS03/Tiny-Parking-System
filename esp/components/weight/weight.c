/**
 * @file weight.c
 * 
 * Interface used to calibrate and read weight sensor
 * 
 */
#include "../../main/fsm.h"

#include "weight.h"

#include "hx711.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <math.h>
#include <stdbool.h>

#define TAG "WEIGHT"

// Idle delay function for low power mode
#ifdef CONFIG_USE_MOCK_CAMERA
    #define IDLE_DELAY() vTaskDelay(pdMS_TO_TICKS(200))
#else
    #define IDLE_DELAY() do { \
        esp_sleep_enable_timer_wakeup(200000); \
        esp_light_sleep_start(); \
    } while(0)
#endif

// Weight sensor pin definitions
#define HX711_DOUT_GPIO  GPIO_NUM_1
#define HX711_CLK_GPIO   GPIO_NUM_2

// Detection parameters
#define NOISE_THRESHOLD          80.0f
#define MIN_CAR_WEIGHT           400.0f
#define MAX_CAR_WEIGHT           1800.0f
#define DETECT_COUNT_REQUIRED    5

// Variables for HX711
static hx711_t hx = {
        .dout = HX711_DOUT_GPIO,
        .pd_sck = HX711_CLK_GPIO,
        .gain = HX711_GAIN_A_128
    };
static float scale = 1.0f;   // grams per raw unit
static float offset = 0.0f;  // raw offset

// Detector state
static float baseline = 0;
static float filtered = 0;
static int detect_count = 0;

// Weight detection enabled flag
static volatile bool weight_enabled = false;


// Prototypes
static void load_calibration();

static void save_calibration();

//////////////////////////////////////////////////////
//////////////// NVS helpers /////////////////////////
//////////////////////////////////////////////////////

/**
 * @brief Load calibration data from NVS
 */
static void load_calibration(void)
{
    nvs_handle_t nvs;
    if (nvs_open("weight", NVS_READONLY, &nvs) == ESP_OK) {
        nvs_get_i32(nvs, "scale", (int32_t*) &scale);
        nvs_get_i32(nvs, "offset", (int32_t*) &offset);
        nvs_close(nvs);
        ESP_LOGI(TAG, "Loaded calibration: scale=%.6f offset=%.2f", scale, offset);
    } else {
        ESP_LOGW(TAG, "No calibration found, using defaults");
    }
}

/**
 * @brief Save calibration data to NVS
 */
static void save_calibration(void)
{
    nvs_handle_t nvs;
    if (nvs_open("weight", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_i32(nvs, "scale", (int32_t) scale);
        nvs_set_i32(nvs, "offset", (int32_t) offset);
        nvs_commit(nvs);
        nvs_close(nvs);
        ESP_LOGI(TAG, "Calibration saved");
    }
}

//////////////////////////////////////////////////////
//////////////// Weight sensor API ///////////////////
//////////////////////////////////////////////////////

/**
 * The function initializes the weight sensor,
 * setting up the HX711 and loading calibration data.
 * If no calibration data is found, default values are used.
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t weight_init(void)
{
    hx711_init(&hx);

    // Initial raw offset (empty scale)
    int32_t raw;
    hx711_read_average(&hx, 10, &raw);
    offset = raw;

    load_calibration();

    baseline = 0;
    filtered = 0;
    detect_count = 0;

    ESP_LOGI(TAG, "Weight sensor initialized");
    return ESP_OK;
}

/**
 * Reads the weight in grams from the sensor,
 * applying calibration parameters.
 * @return Weight in grams
 */
float weight_read_grams(void)
{
    int32_t raw;
    if (hx711_read_average(&hx, 5, &raw) != ESP_OK) {
        return 0;
    }

    float net = (float)(raw - offset);
    return net * scale;
}

/**
 * Checks if a vehicle is detected based on
 * the weight reading and predefined thresholds.
 * The algorithm uses baseline compensation,
 * EMA filtering, and a detection count mechanism for robustness.
 * @return true if a vehicle is detected, false otherwise
 */
bool weight_detect_vehicle(void)
{
    float raw_weight = weight_read_grams();

    // Baseline drift compensation
    baseline = baseline * 0.99f + raw_weight * 0.01f;

    float net = raw_weight - baseline;

    // EMA filter
    filtered = filtered * 0.05f + net * 0.95f;

    // Noise rejection
    if (fabsf(filtered) < NOISE_THRESHOLD) {
        detect_count = 0;
        return false;
    }

    // Threshold window
    if (filtered > MIN_CAR_WEIGHT && filtered < MAX_CAR_WEIGHT) {
        detect_count++;
        if (detect_count >= DETECT_COUNT_REQUIRED) {
            ESP_LOGI(TAG, "Vehicle detected: %.1f g", filtered);
            detect_count = 0;
            return true;
        }
    } else {
        detect_count = 0;
    }

    return false;
}

/**
 * Calibrates the weight sensor using a known weight.
 * The user is prompted to remove all weight, then place
 * the known weight on the scale. Calibration parameters
 * are computed and saved to NVS.
 * @param known_weight_g The known weight in grams used for calibration
 */
void weight_calibrate(float known_weight_g)
{
    int32_t raw_empty, raw_loaded;

    ESP_LOGI(TAG, "Calibrating... remove all weight");
    vTaskDelay(pdMS_TO_TICKS(2000));

    hx711_read_average(&hx, 15, &raw_empty);
    offset = raw_empty;

    ESP_LOGI(TAG, "Place %.1f g on the scale", known_weight_g);
    vTaskDelay(pdMS_TO_TICKS(3000));

    hx711_read_average(&hx, 15, &raw_loaded);

    scale = known_weight_g / (float)(raw_loaded - raw_empty);

    save_calibration();

    ESP_LOGI(TAG, "Calibration complete: scale=%.6f", scale);
}


//////////////////////////////////////////////////////
//////////////// Weight detection task ///////////////
//////////////////////////////////////////////////////

void enable_weight_detection(bool enable) {
    weight_enabled = enable;
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



