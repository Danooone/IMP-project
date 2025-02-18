// led.h
//Login:xzvere00 , Zverev Daniil
#pragma once 

#ifdef __cplusplus
extern "C" {  
#endif

// Initialize the LED
void led_init();

// Set the LED brightness level
// Parameters:
//   level - Brightness level (e.g., 0.0 for off, 1.0 for full brightness)
void led_set_brightness(float level);

#ifdef __cplusplus
}  
#endif
