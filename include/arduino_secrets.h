/*
 * Copy this template into arduino_secrets.h and change values accordingly.
 *
 * This file holds settings following the Arduino convention, see
 * https://docs.arduino.cc/arduino-cloud/tutorials/store-your-sensitive-data-safely-when-sharing
 *
 */

// Settings: Wi-Fi credentials
#define SECRET_WIFI_SSID        "FRITZ!Box 7530 TH"               // Replace "eduroam" for other Wi-Fi
#define SECRET_WIFI_PASSWORD    "******"              // Wi-Fi or university account password
#define SECRET_WIFI_ANONYMOUSID "wlan@tu-berlin.de"     // Don't change, only used for Eduroam
#define SECRET_WIFI_EDUROAMID   "********@tu-berlin.de" // TU account name, only used for Eduroam

// Settings: MQTT
#define SECRET_MQTT_BROKER   "heide.bastla.net" // Server hostname (FQDN)
#define SECRET_MQTT_PORT     8883               // Server TLS port
#define SECRET_MQTT_USER     "***"            // Server credentials
#define SECRET_MQTT_PASSWORD "***"
#define SECRET_MQTT_PREFIX   "**"            // Prefix for topic and client ID
#define SECRET_MQTT_YEAR     2024               // Current year (beginning of course)
#define SECRET_MQTT_BOARDID  25                  // [ID]: Assigned PCB number
