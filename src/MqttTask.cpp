#include "MqttTask.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <esp_wpa2.h>
#include "arduino_secrets.h"
#include <time.h>



// Define the flag; initially, connection has not failed.
bool mqttConnectionFailed = false;
bool wifiConnectionFailed = false;

// ---------------------------------------------------------------------
// Configuration (loaded from arduino_secrets.h)
static const String wifiSSID         = SECRET_WIFI_SSID;
static const char   wifiPassword[]   = SECRET_WIFI_PASSWORD;
static const char   wifiAnonymousId[]= SECRET_WIFI_ANONYMOUSID;
static const char   wifiEduroamId[]  = SECRET_WIFI_EDUROAMID;
static const char   mqttBroker[]     = SECRET_MQTT_BROKER;
static const int    mqttPort         = SECRET_MQTT_PORT;
static const char   mqttUser[]       = SECRET_MQTT_USER;
static const char   mqttPassword[]   = SECRET_MQTT_PASSWORD;
static const String mqttPrefix       = SECRET_MQTT_PREFIX;
static const int    mqttYear         = SECRET_MQTT_YEAR;
static const byte   mqttBoardId      = SECRET_MQTT_BOARDID;

// ---------------------------------------------------------------------
// MQTT topics (generated during initialization)
static String mqttPublish;   // telemetry topic
static String mqttSubscribe; // telecommand topic


// ---------------------------------------------------------------------



struct MqttMsg {
  size_t length;                   // store the actual length of the payload
  char payload[MQTT_MSG_MAX_LEN];  // payload data (without extra room for a null terminator)
};


// Define the global variables
uint8_t inboundMessage[MQTT_MSG_MAX_LEN] = {0};
bool newMessageAvailable = false;
size_t inboundMessageLength = 0;

// ---------------------------------------------------------------------
// #define MQTT_MSG_MAX_LEN 256

// struct MqttMsg {
//   char payload[MQTT_MSG_MAX_LEN];
// };




static QueueHandle_t mqttQueue = NULL;

// ---------------------------------------------------------------------
// // TLS certificate for the MQTT broker
// static const char tlsPublicCertificate[] = ("\


// Encryption using R11 from https://letsencrypt.org/certificates/
const char tlsPublicCertificate[] = ("\
-----BEGIN CERTIFICATE-----\n\
MIIFBTCCAu2gAwIBAgIQS6hSk/eaL6JzBkuoBI110DANBgkqhkiG9w0BAQsFADBP\n\
MQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJuZXQgU2VjdXJpdHkgUmVzZWFy\n\
Y2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBYMTAeFw0yNDAzMTMwMDAwMDBa\n\
Fw0yNzAzMTIyMzU5NTlaMDMxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBF\n\
bmNyeXB0MQwwCgYDVQQDEwNSMTAwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n\
AoIBAQDPV+XmxFQS7bRH/sknWHZGUCiMHT6I3wWd1bUYKb3dtVq/+vbOo76vACFL\n\
YlpaPAEvxVgD9on/jhFD68G14BQHlo9vH9fnuoE5CXVlt8KvGFs3Jijno/QHK20a\n\
/6tYvJWuQP/py1fEtVt/eA0YYbwX51TGu0mRzW4Y0YCF7qZlNrx06rxQTOr8IfM4\n\
FpOUurDTazgGzRYSespSdcitdrLCnF2YRVxvYXvGLe48E1KGAdlX5jgc3421H5KR\n\
mudKHMxFqHJV8LDmowfs/acbZp4/SItxhHFYyTr6717yW0QrPHTnj7JHwQdqzZq3\n\
DZb3EoEmUVQK7GH29/Xi8orIlQ2NAgMBAAGjgfgwgfUwDgYDVR0PAQH/BAQDAgGG\n\
MB0GA1UdJQQWMBQGCCsGAQUFBwMCBggrBgEFBQcDATASBgNVHRMBAf8ECDAGAQH/\n\
AgEAMB0GA1UdDgQWBBS7vMNHpeS8qcbDpHIMEI2iNeHI6DAfBgNVHSMEGDAWgBR5\n\
tFnme7bl5AFzgAiIyBpY9umbbjAyBggrBgEFBQcBAQQmMCQwIgYIKwYBBQUHMAKG\n\
Fmh0dHA6Ly94MS5pLmxlbmNyLm9yZy8wEwYDVR0gBAwwCjAIBgZngQwBAgEwJwYD\n\
VR0fBCAwHjAcoBqgGIYWaHR0cDovL3gxLmMubGVuY3Iub3JnLzANBgkqhkiG9w0B\n\
AQsFAAOCAgEAkrHnQTfreZ2B5s3iJeE6IOmQRJWjgVzPw139vaBw1bGWKCIL0vIo\n\
zwzn1OZDjCQiHcFCktEJr59L9MhwTyAWsVrdAfYf+B9haxQnsHKNY67u4s5Lzzfd\n\
u6PUzeetUK29v+PsPmI2cJkxp+iN3epi4hKu9ZzUPSwMqtCceb7qPVxEbpYxY1p9\n\
1n5PJKBLBX9eb9LU6l8zSxPWV7bK3lG4XaMJgnT9x3ies7msFtpKK5bDtotij/l0\n\
GaKeA97pb5uwD9KgWvaFXMIEt8jVTjLEvwRdvCn294GPDF08U8lAkIv7tghluaQh\n\
1QnlE4SEN4LOECj8dsIGJXpGUk3aU3KkJz9icKy+aUgA+2cP21uh6NcDIS3XyfaZ\n\
QjmDQ993ChII8SXWupQZVBiIpcWO4RqZk3lr7Bz5MUCwzDIA359e57SSq5CCkY0N\n\
4B6Vulk7LktfwrdGNVI5BsC9qqxSwSKgRJeZ9wygIaehbHFHFhcBaMDKpiZlBHyz\n\
rsnnlFXCb5s8HKn5LsUgGvB24L7sGNZP2CX7dhHov+YhD+jozLW2p9W4959Bz2Ei\n\
RmqDtmiXLnzqTpXbI+suyCsohKRg6Un0RC47+cpiVwHiXZAW+cn8eiNIjqbVgXLx\n\
KPpdzvvtTnOPlC7SQZSYmdunr3Bf9b77AiC/ZidstK36dRILKz7OA54=\n\
-----END CERTIFICATE-----\n");
    

// ---------------------------------------------------------------------
// WiFi and MQTT objects
static WiFiClientSecure wifiClient;
static PubSubClient client(wifiClient);

// ---------------------------------------------------------------------
// Internal helper functions



void mqttCallback(char* topic, byte *payload, unsigned int length) {
//     Serial.println("Info: Receiving message from ground...");
//     Serial.print("      COM CHANNEL: ");
//     Serial.println(topic);
//     Serial.print("      UPLINK DATA: ");
//     Serial.write(payload, length);
//     Serial.println();

    
//   // Print the payload length
//   Serial.print("Payload length: ");
//   Serial.println(length);
  
    // // Update the global inboundMessage.
    // // Note: If payload contains binary data, you might need a different approach.

  // Update the global inboundMessage with the raw bytes
  inboundMessageLength =  length ; // (length < MQTT_MSG_MAX_LEN) ? length : MQTT_MSG_MAX_LEN;
  memcpy(inboundMessage, payload, inboundMessageLength);


      // Calculate a simple hash using the djb2 algorithm
  unsigned long hash = 5381;
  for (unsigned int i = 0; i < inboundMessageLength; i++) {
    hash = ((hash << 5) + hash) + inboundMessage[i]; // hash * 33 + current byte
  }

    // Print the hash in hexadecimal
    Serial.print("inboundMessage hash (djb2): 0x");
    Serial.println(hash, HEX);



    // Set a flag to indicate a new message is available (optional)
    newMessageAvailable = true;
  }
  


  

// Connect to standard WiFi
void wifiConnect() {
  delay(10);
  Serial.print("\nAttempting Wi-Fi connection to ");
  Serial.println(wifiSSID);
  WiFi.begin(wifiSSID.c_str(), wifiPassword);

  int attempts = 0;
  const int maxAttempts = 4; // For example, try 20 times (approx 10 seconds if 500ms per attempt)

  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    wifiConnectionFailed = true;
    Serial.println("\nWiFi connection failed.");
  } else {
    randomSeed(micros());
    Serial.print(" done using IP address ");
    Serial.println(WiFi.localIP());
  }


}

// Connect to eduroam (WPA2 Enterprise)
void wifiConnectEduroam() {
  delay(10);
  Serial.print("\nAttempting Wi-Fi connection to ");
  Serial.println(wifiSSID);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)wifiAnonymousId, strlen(wifiAnonymousId));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)wifiEduroamId, strlen(wifiEduroamId));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)wifiPassword, strlen(wifiPassword));
  esp_wifi_sta_wpa2_ent_enable();
  WiFi.begin(wifiSSID.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.print(" done using IP address ");
  Serial.println(WiFi.localIP());
}

// Establish connection to the MQTT broker
void mqttConnect() {


  wifiClient.setCACert(tlsPublicCertificate);
  String mqttClientId = mqttPrefix + "-" + String(mqttBoardId);
  int attempts = 0;
  while (!client.connected() && attempts < 4) {
    Serial.print("Attempting MQTT connection to ");
    Serial.println(mqttBroker);
    Serial.print("... ");
    if (client.connect(mqttClientId.c_str(), mqttUser, mqttPassword)) {
      Serial.print("done using client ID ");
      Serial.println(mqttClientId);
      client.subscribe(mqttSubscribe.c_str());
    } else {
      attempts++;
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(3000);
    }
  }
  if (!client.connected()) {
    mqttConnectionFailed = true;
    Serial.println("MQTT connection failed after 4 attempts");
  }

}

// FreeRTOS task that continuously runs the MQTT loop.
void mqttLoopTask(void *pvParameters) {
  for (;;) {
    if (!client.connected()) {
      mqttConnect();
    }
    client.loop();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}




  int getWiFiRSSI() {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.RSSI();  // ✅ Returns the current Wi-Fi signal strength
    } else {
        Serial.println("WiFi not connected. RSSI unavailable.");
        return -1000;  // A very low RSSI value indicating no connection
    }
}



// ---------------------------------------------------------------------
// Public functions (declared in MqttTask.h)

void initMqttTask() {


  // Generate board-specific MQTT topics.
  String mqttTopic = mqttPrefix + "/" + String(mqttYear) + "/" + String(mqttBoardId) + "/";
  mqttPublish   = mqttTopic + "tm";
//   mqttSubscribe = mqttTopic + "tc";
  
  mqttSubscribe = mqttPrefix + "/" + String(mqttYear) + "/" + String(240) + "/" + "tc";
  

  Serial.println("Initializing MQTT Task...");

  // Connect to WiFi (choose the correct method based on SSID).
  if (wifiSSID.equals("eduroam")) {
    wifiConnectEduroam();
  } else {
    wifiConnect();
  }

// If WiFi failed, skip MQTT initialization.
if (wifiConnectionFailed) {
    Serial.println("Skipping MQTT initialization due to WiFi connection failure.");
    mqttConnectionFailed = true;
    return;
    }


  // Setup MQTT server and callback.
  client.setServer(mqttBroker, mqttPort);
  client.setCallback(mqttCallback);
  
  // Connect to the MQTT broker.
  mqttConnect();


// If MQTT connection failed after 4 attempts, skip further initialization.
if (mqttConnectionFailed) {
    Serial.println("Skipping further code due to MQTT connection failure.");
    return;
    }
    

  // Create a FreeRTOS task to continuously process MQTT.
  xTaskCreatePinnedToCore(
    mqttLoopTask,   // Task function.
    "mqttLoopTask", // Name of the task.
    8192,           // Stack size in bytes. // 4096
    NULL,           // Task input parameter.
    1,              // Priority.
    NULL,           // Task handle.
    1               // Core where the task should run.
  );


}

// Publish an MQTT message (to be called from main.cpp).
void publishMqttMessage(const String &message) {
  if (client.connected()) {
    client.publish(mqttPublish.c_str(), message.c_str());
  } else {
    Serial.println("MQTT client not connected; cannot publish message.");
  }
}
