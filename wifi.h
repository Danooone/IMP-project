// wifi.h
// Login: xzvere00 , Zverev Daniil

#pragma once 

#ifdef __cplusplus
extern "C" {  
#endif

// Initialize WiFi in station mode
// Parameters:
//   ssid - The WiFi network name
//   pass - The WiFi network password
void wifi_init_sta(const char* ssid, const char* pass);

#ifdef __cplusplus
}
#endif
