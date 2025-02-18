// config.c
//Login:xzvere00 , Zverev Daniil
#include "config.h"         
#include "nvs.h"            
#include "nvs_flash.h"      
#include <string.h>        

// Load settings from Non-Volatile Storage (NVS) into the config structure
void load_settings_from_nvs(app_config_t *cfg) {
    nvs_handle_t handle;  // Handle for NVS operations

    // Open the NVS storage in read-only mode
    if (nvs_open("storage", NVS_READONLY, &handle) == ESP_OK) {
        size_t required_size = sizeof(cfg->mqtt_topic);  // Size needed for MQTT topic

        // Get the "threshold_lux" value from NVS
        if (nvs_get_i32(handle, "threshold_lux", &cfg->threshold_lux) != ESP_OK) {
            cfg->threshold_lux = 200;  // Default value if not found
        }

        // Get the "hysteresis" value from NVS
        if (nvs_get_i32(handle, "hysteresis", &cfg->hysteresis) != ESP_OK) {
            cfg->hysteresis = 20;  // Default value if not found
        }

        // Get the "mqtt_topic" string from NVS
        if (nvs_get_str(handle, "mqtt_topic", cfg->mqtt_topic, &required_size) != ESP_OK) {
            strcpy(cfg->mqtt_topic, "device/lux");  // Default value if not found
        }

        nvs_close(handle);  // Close the NVS handle
    } else {
        // If opening NVS fails, set default values
        cfg->threshold_lux = 200;
        cfg->hysteresis = 20;
        strcpy(cfg->mqtt_topic, "device/lux");
    }
}

// Save settings from the config structure to Non-Volatile Storage (NVS)
void save_settings_to_nvs(const app_config_t *cfg) {
    nvs_handle_t handle;  // Handle for NVS operations

    // Open the NVS storage in read-write mode
    if (nvs_open("storage", NVS_READWRITE, &handle) == ESP_OK) {
        // Set the "threshold_lux" value in NVS
        nvs_set_i32(handle, "threshold_lux", cfg->threshold_lux);

        // Set the "hysteresis" value in NVS
        nvs_set_i32(handle, "hysteresis", cfg->hysteresis);

        // Set the "mqtt_topic" string in NVS
        nvs_set_str(handle, "mqtt_topic", cfg->mqtt_topic);

        nvs_commit(handle);  // Commit the changes to NVS
        nvs_close(handle);   // Close the NVS handle
    }
}
