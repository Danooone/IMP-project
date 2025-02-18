// led.c
//Login:xzvere00 , Zverev Daniil
#include "led.h"
#include "driver/ledc.h"
#include <math.h>

#define LED_PIN 25                  // GPIO pin number connected to the LED
#define LEDC_CHANNEL LEDC_CHANNEL_0 // LEDC channel used for PWM
#define LEDC_TIMER LEDC_TIMER_0     // LEDC timer used for PWM
#define LEDC_MODE LEDC_HIGH_SPEED_MODE // LEDC speed mode
#define LEDC_DUTY_RES LEDC_TIMER_10_BIT // PWM duty resolution (10 bits)
#define LEDC_FREQ 5000              // PWM frequency in Hz

// Initialize the LED PWM settings
void led_init() {
    // Configure the LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer); // Apply the timer configuration

    // Configure the LEDC channel
    ledc_channel_config_t ch_conf = {
        .gpio_num = LED_PIN,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0,    // Start with LED off
        .hpoint = 0
    };
    ledc_channel_config(&ch_conf); // Apply the channel configuration
}

// Set the LED brightness level
// Parameters:
//   level - Brightness level between 0.0 (off) and 1.0 (full brightness)
void led_set_brightness(float level) {
    float gamma = 2.2; // Gamma correction value
    float corrected = powf(level, gamma); // Apply gamma correction
    uint32_t duty = (uint32_t)(corrected * ((1 << LEDC_DUTY_RES) - 1)); // Calculate PWM duty cycle
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty); // Set the duty cycle
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);    // Update the PWM signal
}
