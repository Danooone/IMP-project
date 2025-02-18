// sensor.h
//Login:xzvere00 , Zverev Daniil
#pragma once  

#ifdef __cplusplus
extern "C" {  
#endif

// Initialize the sensor
void sensor_init();

// Read the light level in lux
// Returns:
//   The current light level as a float
float sensor_read_lux();

#ifdef __cplusplus
}
#endif
