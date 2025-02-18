// mqtt.c
//Login:xzvere00 , Zverev Daniil
#include "mqtt.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "config.h"
#include "cJSON.h"

static const char *TAG = "MQTT";                // Tag used for MQTT logging
static const char *MQTT_TAG = "MQTT";           // Another tag for MQTT logging
static esp_mqtt_client_handle_t client = NULL;  // MQTT client handle

extern app_config_t cfg;                         // External configuration structure

// Publish the current lux value to the MQTT broker
void publish_lux_data(esp_mqtt_client_handle_t client, float lux_value) {
    char lux_str[10];
    snprintf(lux_str, sizeof(lux_str), "%.2f", lux_value); // Convert lux to string
    int msg_id = esp_mqtt_client_publish(client, "device/lux", lux_str, 0, 1, 0); // Publish message
    if (msg_id != -1) {
       ESP_LOGI(TAG, "Published LUX: %s with msg_id=%d", lux_str, msg_id); // Log successful publish
    } else {
        ESP_LOGI(MQTT_TAG, "Failed to publish LUX"); // Log failed publish
    }
}

// Handle MQTT events
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = event_data; // Cast event data
    switch(event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected"); // Log connection
            esp_mqtt_client_subscribe(client, "device/control", 1); // Subscribe to control topic
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT disconnected"); // Log disconnection
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT message published, msg_id=%d", event->msg_id); // Log published message
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT error occurred"); // Log error
            break;
            ESP_LOGW(TAG, "Unhandled MQTT event id: %d", event->event_id); 
            // Note: The above warning is unreachable code after break
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG,"Data on topic: %.*s", event->topic_len, event->topic); // Log received data
            if (strncmp(event->topic, "device/control", event->topic_len) == 0) {
                cJSON *root = cJSON_ParseWithLength(event->data, event->data_len); // Parse JSON data
                if (root) {
                    cJSON *th = cJSON_GetObjectItem(root, "threshold");   // Get "threshold" from JSON
                    cJSON *hy = cJSON_GetObjectItem(root, "hysteresis");  // Get "hysteresis" from JSON
                    cJSON *tp = cJSON_GetObjectItem(root, "topic");       // Get "topic" from JSON
                    bool updated = false; // Flag to check if config is updated
                    if (cJSON_IsNumber(th)) {
                        ESP_LOGI(TAG, "Updating threshold to: %d", th->valueint);
                        cfg.threshold_lux = th->valueint; // Update threshold
                        updated = true;
                    }
                    if (cJSON_IsNumber(hy)) {
                        ESP_LOGI(TAG, "Updating hysteresis to: %d", hy->valueint);
                        cfg.hysteresis = hy->valueint; // Update hysteresis
                        updated = true;
                    }
                    if (cJSON_IsString(tp)) {
                        ESP_LOGI(TAG, "Updating MQTT topic to: %s", tp->valuestring);
                        strncpy(cfg.mqtt_topic, tp->valuestring, sizeof(cfg.mqtt_topic)); // Update MQTT topic
                        updated = true;
                    }
                    if (updated) {
                        save_settings_to_nvs(&cfg); // Save updated config to NVS
                        ESP_LOGI(TAG, "Configuration updated and saved");
                    }
                    cJSON_Delete(root); // Free JSON memory
                } else {
                    ESP_LOGE(TAG, "Failed to parse JSON data");
                }
            }
            break;
        default:
            break; // Do nothing for other events
    }
}

// Initialize the MQTT client
void mqtt_init(const char* uri) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri, // Set MQTT broker URI
    };
    client = esp_mqtt_client_init(&mqtt_cfg); // Initialize MQTT client
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL); // Register event handler
    esp_mqtt_client_start(client); // Start MQTT client
}

// Publish a message to a specific MQTT topic
void mqtt_publish(const char* topic, const char* msg) {
    if (client) {
        int msg_id = esp_mqtt_client_publish(client, topic, msg, 0, 1, 0);
        if (msg_id != -1) {
            ESP_LOGI(TAG, "Message published to topic '%s': %s (msg_id=%d)", topic, msg, msg_id);
        } else {
            ESP_LOGE(TAG, "Failed to publish message to topic '%s': %s", topic, msg);
        }
    } else {
        ESP_LOGE(TAG, "MQTT client is not initialized");
    }
}
