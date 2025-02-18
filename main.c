// main.c
//Login:xzvere00 , Zverev Daniil
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "config.h"
#include "sensor.h"
#include "led.h"
#include "wifi.h"
#include "mqtt.h"
#include "uart_cmd.h"

static const char *TAG = "MAIN";          // Tag used for logging
app_config_t cfg;                         // Configuration structure
static float current_lux = 0.0f;          // Current light level in lux
static bool led_on = false;               // LED state flag

// Calculate the brightness based on the current lux and configuration
static float calculate_brightness(app_config_t *cfg, float lux) { 
    // For smooth transition, use linear interpolation of brightness.
    if (lux <= 0.0f) return 1.0f; // Maximum brightness when no light
    else if (lux >= cfg->threshold_lux) return 0.0f; // LED off when above threshold
    else return 1.0f - (lux / (float)cfg->threshold_lux); // Linear brightness scaling
}


static void sensor_task(void* arg) {
    app_config_t *cfg = (app_config_t*)arg; // Cast argument to config structure
    char msg[128]; // Buffer for MQTT message

    while (1) {
        current_lux = sensor_read_lux(); // Read current lux from sensor
        float brightness = calculate_brightness(cfg, current_lux); // Calculate brightness
        led_set_brightness(brightness); // Set LED brightness
        ESP_LOGI(TAG, "LUX=%.2f, Brightness=%.2f", current_lux, brightness); // Log values

        // Create JSON message with current lux
        snprintf(msg, sizeof(msg), "{\"lux\":%.2f}", current_lux);
        mqtt_publish(cfg->mqtt_topic, msg); // Publish MQTT message

        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 5 seconds
    }
}

// Main application entry point
void app_main(void) {
    // Initialize Non-Volatile Storage (NVS)
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase()); // Erase NVS if needed
        err = nvs_flash_init(); // Re-initialize NVS
    }
    ESP_ERROR_CHECK(err); // Check for errors

    load_settings_from_nvs(&cfg); // Load configuration from NVS
    ESP_LOGI(TAG, "Loaded config: threshold=%d, hysteresis=%d, topic=%s",
             cfg.threshold_lux, cfg.hysteresis, cfg.mqtt_topic); // Log loaded config

    sensor_init(); // Initialize the sensor
    led_init(); // Initialize the LED
    wifi_init_sta("iPhone (Даниил)", "123456789"); // Initialize WiFi in station mode with SSID and password
    uart_commands_init(&cfg); 

    // Create the sensor task with stack size 4096, priority 5
    xTaskCreate(sensor_task, "sensor_task", 4096, &cfg, 5, NULL);
}
