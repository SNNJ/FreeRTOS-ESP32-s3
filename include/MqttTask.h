/**
 * @file MqttTask.h
 * @brief MQTT communication interface and task declarations for CADSE satellite project.
 * 
 * Provides WiFi and MQTT initialization, message publishing, and inbound message tracking.
 */

#ifndef MQTTTASK_H
#define MQTTTASK_H

#include <Arduino.h>
#include "arduino_secrets.h"

#define MQTT_MSG_MAX_LEN 170  // maximum payload bytes

/**
 * @brief Initializes WiFi, connects to MQTT broker, and starts the MQTT background task.
 * 
 * Should be called once during setup().
 */

// Call this once from setup() to initialize WiFi, MQTT, and start the MQTT processing task.
void initMqttTask();

/**
 * @brief Publishes a message to the configured MQTT topic.
 * 
 * @param message The message string to publish.
 */
// Call this from main.cpp to publish a message via MQTT.
void publishMqttMessage(const String &message);




/**
 * @brief Returns the current WiFi RSSI (signal strength) in dB.
 * 
 * @return int WiFi signal strength in dBm.
 */
int getWiFiRSSI();  // ✅ Declare the function



/**
 * @brief Flag indicating that the WiFi connection attempt failed.
 */
extern bool wifiConnectionFailed; // New flag for WiFi failure


/**
 * @brief Flag indicating that the WiFi connection attempt failed.
 */
extern bool mqttConnectionFailed;

/**
 * @brief Flag indicating that the WiFi connection attempt failed.
 */
extern bool wifiConnectionFailed; // New flag for WiFi failure


/**
 * @brief Flag indicating that the WiFi connection attempt failed.
 */
extern bool mqttConnectionFailed;

// Global variable to hold the raw inbound MQTT message
extern uint8_t inboundMessage[MQTT_MSG_MAX_LEN];

/**
 * @brief True if a new inbound message has been received and is ready to process.
 */

// Optional flag to indicate a new message has been received
extern bool newMessageAvailable;

/**
 * @brief Number of bytes received in the latest inbound MQTT message.
 */

// Global variable to track the number of bytes received
extern size_t inboundMessageLength;

#endif // MQTTTASK_H
