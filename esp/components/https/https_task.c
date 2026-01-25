/**
 * @file https_task.c
 * 
 * Helpers and wrapper to send https calls
 * to the API
 */

#include "https_task.h"
#include "https.h"
#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"

// Status variables
static esp_err_t wifi_status;
static esp_err_t camera_status;
static esp_err_t ultrasonic_status;
static esp_err_t weight_status;
static esp_err_t servo_status;
static esp_err_t oled_status;

// Entry/exit request variables
static char * license_plate;
static char * image_url;
static float recorded_weight;

// Response payload variable
static char * payload;

const char *TAG = "HTTPS Task module";

////////////////////////////////////////////////////////////////////
///////////////////// Status tasks /////////////////////////////////
////////////////////////////////////////////////////////////////////

void put_status_task(void *arg) {
    ESP_LOGI(TAG, "Sending status to backend...");
    send_system_status_to_api(camera_status, ultrasonic_status, weight_status, servo_status, wifi_status, oled_status);
}

void set_status_variables(esp_err_t camera, esp_err_t ultrasonic, esp_err_t weight, esp_err_t servo, esp_err_t wifi, esp_err_t oled) {
    camera_status = camera;
    ultrasonic_status = ultrasonic;
    weight_status = weight;
    servo_status = servo;
    wifi_status = wifi;
    oled_status = oled;
}

void send_system_status_to_api() {
    cJSON *board_status = cJSON_CreateArray();
    
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "name", "ESP main module");
    cJSON_AddStringToObject(item, "status", camera_status == ESP_OK ? "Active" : "Problem");
    cJSON_AddStringToObject(item, "espStatus", esp_err_to_name(camera_status));
    cJSON_AddItemToArray(board_status, item);
    
    item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "name", "Ultrasonic sensor");
    cJSON_AddStringToObject(item, "status", ultrasonic_status == ESP_OK ? "Active" : "Problem");
    cJSON_AddStringToObject(item, "espStatus", esp_err_to_name(ultrasonic_status));
    cJSON_AddItemToArray(board_status, item);
    
    item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "name", "Weight sensor");
    cJSON_AddStringToObject(item, "status", weight_status == ESP_OK ? "Active" : "Problem");
    cJSON_AddStringToObject(item, "espStatus", esp_err_to_name(weight_status));
    cJSON_AddItemToArray(board_status, item);
    
    item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "name", "Motor sensor");
    cJSON_AddStringToObject(item, "status", servo_status == ESP_OK ? "Active" : "Problem");
    cJSON_AddStringToObject(item, "espStatus", esp_err_to_name(servo_status));
    cJSON_AddItemToArray(board_status, item);
    
    item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "name", "Wifi sensor");
    cJSON_AddStringToObject(item, "status", wifi_status == ESP_OK ? "Active" : "Problem");
    cJSON_AddStringToObject(item, "espStatus", esp_err_to_name(wifi_status));
    cJSON_AddItemToArray(board_status, item);

    item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "name", "OLED Display");
    cJSON_AddStringToObject(item, "status", wifi_status == ESP_OK ? "Active" : "Problem");
    cJSON_AddStringToObject(item, "espStatus", esp_err_to_name(oled_status));
    cJSON_AddItemToArray(board_status, item);
    
    char *status_json = cJSON_Print(board_status);
    https_put_status(status_json);

    cJSON_Delete(board_status);
    free(status_json);
}

////////////////////////////////////////////////////////////////////
/////////////////// Entry/Exit tasks ///////////////////////////////
////////////////////////////////////////////////////////////////////

void post_entry_task(void *arg) {
    ESP_LOGI(TAG, "Sending entry request to backend...");
    send_entry_to_api();
}

void send_entry_to_api(void) {
    cJSON *entry_payload = cJSON_CreateObject();
    cJSON_AddStringToObject(entry_payload, "licensePlate", license_plate);
    cJSON_AddStringToObject(entry_payload, "imageUrl", image_url);
    cJSON_AddNumberToObject(entry_payload, "recordedWeight", recorded_weight);
    
    char *entry_json = cJSON_Print(entry_payload);
    https_post_entry(entry_json);
    
    cJSON_Delete(entry_payload);
    free(entry_json);
}

void set_entry_variables(char * license_plate, char * image_url, float * recorded_weight) {
    license_plate = license_plate;
    image_url = image_url;
    recorded_weight = recorded_weight;
}

void post_exit_task(void *arg) {
    ESP_LOGI(TAG, "Sending exit request to backend...");
    send_exit_to_api();
}

void send_exit_to_api(void) {
    cJSON *exit_payload = cJSON_CreateObject();
    cJSON_AddStringToObject(exit_payload, "licensePlate", license_plate);
    
    char *exit_json = cJSON_Print(exit_payload);
    https_post_entry(exit_json);
    
    cJSON_Delete(exit_payload);
    free(exit_json);
}

void set_exit_variables(char * license_plate) {
    license_plate = license_plate;
}
