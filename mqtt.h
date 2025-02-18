// mqtt.h
//Login:xzvere00 , Zverev Daniil
#pragma once  

#ifdef __cplusplus
extern "C" {  
#endif

// Initialize the MQTT client
// Parameters:
//   uri - The URI of the MQTT broker 
void mqtt_init(const char* uri);

// Publish a message to a specific MQTT topic
// Parameters:
//   topic - The MQTT topic to publish to
//   msg - The message to send
void mqtt_publish(const char* topic, const char* msg);

#ifdef __cplusplus
}
#endif
