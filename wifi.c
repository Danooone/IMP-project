// wifi.c
//Login:xzvere00 , Zverev Daniil
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include <string.h>
#include "mqtt.h"

static const char *WIFI_TAG = "WIFI";      
static const char *IP_TAG = "IP_EVENT";    

// Handle WiFi events
static void wifi_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data) {
    ESP_LOGI(WIFI_TAG, "WiFi event id: %ld", (long int)event_id);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect(); 
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(WIFI_TAG, "WiFi disconnected, retrying connection...");
        esp_wifi_connect(); 
    }
}

// Handle IP events
static void ip_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data) {
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data; 
        ESP_LOGI(IP_TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(IP_TAG, "Initializing MQTT...");
        mqtt_init("mqtt://broker.hivemq.com:1883"); 
    }
}

// Initialize WiFi in station mode
void wifi_init_sta(const char* ssid, const char* pass) {
    esp_netif_init(); 
    esp_event_loop_create_default(); 
    esp_netif_create_default_wifi_sta(); 
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); 
    esp_wifi_init(&cfg); 

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, ip_event_handler, NULL, NULL);

    wifi_config_t wifi_config = {0}; 
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));

    esp_wifi_set_mode(WIFI_MODE_STA); 
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start(); 
    ESP_LOGI(WIFI_TAG, "Connecting to WiFi SSID:%s", ssid);
}
