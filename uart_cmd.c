// uart_commands.c
//Login:xzvere00 , Zverev Daniil
#include "uart_cmd.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "config.h"
#include "esp_log.h"

extern app_config_t cfg;                      // External configuration structure

static const char *TAG = "UART_CMD";          // Tag used for UART command logging

// Task to handle UART commands
static void uart_cmd_task(void* arg) {
    uint8_t data[128]; // Buffer to store incoming UART data
    while (1) {
        // Read bytes from UART_NUM_0 with a timeout of 1000 ms
        int len = uart_read_bytes(UART_NUM_0, data, sizeof(data) - 1, pdMS_TO_TICKS(1000));
        if (len > 0) {
            data[len] = 0;            // Null-terminate the received data
            char* line = (char*)data; // Cast data to a string

            if (strncmp(line, "threshold ", 10) == 0) {
                int val = atoi(line + 10);          
                cfg.threshold_lux = val;             
                save_settings_to_nvs(&cfg);          
                ESP_LOGI(TAG, "Threshold set to %d", val); 
            }
            else if (strncmp(line, "hysteresis ", 11) == 0) {
                int val = atoi(line + 11);          
                cfg.hysteresis = val;              
                save_settings_to_nvs(&cfg);         
                ESP_LOGI(TAG, "Hysteresis set to %d", val); 
            }
            else if (strncmp(line, "mqtt_topic ", 11) == 0) {
                strncpy(cfg.mqtt_topic, line + 11, sizeof(cfg.mqtt_topic));
                save_settings_to_nvs(&cfg);         
                ESP_LOGI(TAG, "MQTT topic set to %s", cfg.mqtt_topic); 
            }
            else if (strncmp(line, "show", 4) == 0) {
                ESP_LOGI(TAG, "Current config: threshold=%d, hysteresis=%d, topic=%s",
                         cfg.threshold_lux, cfg.hysteresis, cfg.mqtt_topic);
            }
        }
    }
}

// Initialize UART commands
void uart_commands_init(void* arg) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,                  
        .data_bits = UART_DATA_8_BITS,        
        .parity = UART_PARITY_DISABLE,        
        .stop_bits = UART_STOP_BITS_1,        
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_driver_install(UART_NUM_0, 1024, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config); 
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    xTaskCreate(uart_cmd_task, "uart_cmd_task", 2048, arg, 5, NULL);
}
