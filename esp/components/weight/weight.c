/**
 * @file weight.c
 * 
 * Interface used to calibrate and read weight sensor
 * 
 */

#include "../../main/fsm.h"
#include "../https/https_task.h"
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

// Weight sensor pin definitions
#define HX711_DOUT_GPIO  GPIO_NUM_12
#define HX711_CLK_GPIO   GPIO_NUM_13

// Detection parameters
#define NOISE_THRESHOLD          5.0f
#define MIN_CAR_WEIGHT           25.0f
#define MAX_CAR_WEIGHT           100.0f
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
static float last_raw_weight = -1;
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
    if (nvs_open("weight", NVS_READONLY, &nvs) == ESP_OK) {\
        size_t scale_size = sizeof(scale);
        size_t offset_size = sizeof(offset);
        nvs_get_blob(nvs, "scale", &scale, &scale_size);
        nvs_get_blob(nvs, "offset", &offset, &offset_size);
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
    esp_err_t err = nvs_open("weight", NVS_READWRITE, &nvs);
    if (err == ESP_OK) {
        nvs_set_blob(nvs, "scale", &scale, sizeof(scale));
        nvs_set_blob(nvs, "offset", &offset, sizeof(offset));
        nvs_commit(nvs);
        nvs_close(nvs);
        ESP_LOGI(TAG, "Calibration saved");
    } else {
        ESP_LOGE(TAG, "Failed to open NVS for saving calibration %s", esp_err_to_name(err));
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
    // try to initialize HX711 
    esp_err_t ret = hx711_init(&hx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize HX711: %s", esp_err_to_name(ret));
        return ret;
    }

    // Initial raw offset (empty scale)
    int32_t raw;
    esp_err_t err = hx711_read_average(&hx, 10, &raw);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read average from HX711: %s", esp_err_to_name(err));
    }
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
    esp_err_t ret = hx711_read_average(&hx, 5, &raw);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read weight %s", esp_err_to_name(ret));
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

    if (raw_weight != last_raw_weight) {
        ESP_LOGI(TAG, "Raw weight: %.1f g", raw_weight);
    }

    // Baseline drift compensation
    baseline = baseline * 0.99f + raw_weight * 0.01f;

    float net = raw_weight - baseline;

    // EMA filter
    filtered = filtered * 0.05f + net * 0.95f;

    if (raw_weight != last_raw_weight) {
        ESP_LOGI(TAG, "Filtered weight: %.1f g", filtered);
        last_raw_weight = raw_weight;
    }

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

            float rounded = roundf(filtered * 10.0f) / 10.0f;
            set_weight_data(&rounded);
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
    esp_err_t ret = hx711_init(&hx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize HX711: %s", esp_err_to_name(ret));
    }

    int32_t raw_empty, raw_loaded;

    ESP_LOGI(TAG, "Calibrating... remove all weight");
    vTaskDelay(pdMS_TO_TICKS(2000));

    hx711_read_average(&hx, 15, &raw_empty);
    offset = raw_empty;

    ESP_LOGI(TAG, "Raw empty reading: %d", raw_empty);

    ESP_LOGI(TAG, "Place %.1f g on the scale", known_weight_g);
    vTaskDelay(pdMS_TO_TICKS(5000));

    hx711_read_average(&hx, 15, &raw_loaded);

    ESP_LOGI(TAG, "Raw loaded reading: %d", raw_loaded);

    scale = known_weight_g / ((float)(raw_loaded - raw_empty) ? : known_weight_g);

    save_calibration();

    ESP_LOGI(TAG, "Calibration complete: scale=%.6f", scale);
    ESP_LOGI(TAG, "Restart the device to apply new calibration");
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
        
        // Sleep for 300 ms before next reading
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}



