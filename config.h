// config.h
//Login:xzvere00 , Zverev Daniil
#pragma once  

#ifdef __cplusplus
extern "C" { 
#endif

// Structure to store application settings
typedef struct {
    int threshold_lux;        // Light threshold in lux
    int hysteresis;           // Hysteresis value
    char mqtt_topic[64];      // MQTT topic name
} app_config_t;

// Load settings from non-volatile storage
void load_settings_from_nvs(app_config_t *cfg);

// Save settings to non-volatile storage
void save_settings_to_nvs(const app_config_t *cfg);

#ifdef __cplusplus
}
#endif
