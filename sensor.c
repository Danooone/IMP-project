// sensor.c
//Login:xzvere00 , Zverev Daniil
#include "sensor.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_MASTER_NUM I2C_NUM_0               // I2C port number for master
#define I2C_SDA_IO 21                          // GPIO pin for I2C SDA
#define I2C_SCL_IO 22                          // GPIO pin for I2C SCL
#define I2C_FREQ_HZ 100000                     // I2C clock frequency in Hz

#define BH1750_ADDR 0x23                       // I2C address of the BH1750 sensor
#define BH1750_CMD_CONT_HRES 0x10              // Command for continuous high-resolution mode

static const char *TAG = "SENSOR";             // Tag used for logging

// Initialize the I2C master
static esp_err_t i2c_init_master() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,               
        .sda_io_num = I2C_SDA_IO,              
        .scl_io_num = I2C_SCL_IO,              
        .sda_pullup_en = GPIO_PULLUP_ENABLE,   
        .scl_pullup_en = GPIO_PULLUP_ENABLE,   
        .master.clk_speed = I2C_FREQ_HZ,       
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf)); 
    return i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
}

// Initialize the sensor
void sensor_init() {
    i2c_init_master(); // Initialize I2C master
    uint8_t cmd = BH1750_CMD_CONT_HRES; 
    // Send command to sensor to start measurements
    i2c_master_write_to_device(I2C_MASTER_NUM, BH1750_ADDR, &cmd, 1, pdMS_TO_TICKS(100));
    ESP_LOGI(TAG, "Sensor initialized");
}

// Read the lux value from the sensor
float sensor_read_lux() {
    uint8_t data[2]; 
    // Read 2 bytes of data from the sensor
    if (i2c_master_read_from_device(I2C_MASTER_NUM, BH1750_ADDR, data, 2, pdMS_TO_TICKS(100)) == ESP_OK) {
        uint16_t raw = (data[0] << 8) | data[1]; 
        return raw / 1.2f; // Convert raw value to lux
    }
    return -1.0f; 
}
