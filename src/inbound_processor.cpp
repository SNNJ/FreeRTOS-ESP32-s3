#include "inbound_processor.h"
#include "hardware/storage.h"   // if you need storage functions
#include "MqttTask.h"  // if it declares inboundMessage, newMessageAvailable, etc.


// Define the globals.
String tc = "";
int tcValue = 0;
String tcHash = "";

void processInboundMessage() {
  // Convert the raw bytes to a String (if appropriate) or process them as binary.
  String msgStr = String((char*)inboundMessage, inboundMessageLength);
  Serial.print("Processing inbound message: ");
  Serial.println(msgStr);

  unsigned long hash = 5381;
  for (unsigned int i = 0; i < inboundMessageLength; i++) {
    hash = ((hash << 5) + hash) + inboundMessage[i]; // hash * 33 + current byte
  }

  tcHash = String(hash, HEX);


  if (msgStr.startsWith("SetMode:")) {
    int colonIndex = msgStr.indexOf(':');
    if (colonIndex != -1) {
      String value = msgStr.substring(colonIndex + 1);
      value.trim();
      int modeVal = value.toInt();
      if (modeVal >= 0 && modeVal <= 5) {
        // Update current mode (assume a global variable exists)
        currentMode = modeVal;
        Serial.print("Updated currentMode to: ");
        Serial.println(currentMode);
        tc = "SetMode:";
        tcValue = modeVal;
      
      }
    }
  }
  else if (msgStr.startsWith("SetDefaultMode:")) {
    int prefixLength = String("SetDefaultMode:").length();
    String value = msgStr.substring(prefixLength);
    value.trim();
    int modeVal = value.toInt();
    if (modeVal >= 0 && modeVal <= 5) {
      // Update default mode and store it in flash
      defaultModeValue = modeVal;
      tc = "SetDefaultMode:";
      tcValue = modeVal;           // Save mode value in tcValue.
      
      if (writeDefaultMode(defaultModeValue)) {
        Serial.print("Updated defaultMode to: ");
        Serial.println(defaultModeValue);
      }
    }
  }
  else if (msgStr.startsWith("PacketID:")) {
    int firstColon = msgStr.indexOf(':');
    int secondColon = msgStr.indexOf(':', firstColon + 1);
    if (firstColon != -1 && secondColon != -1) {
      String packetIDStr = msgStr.substring(firstColon + 1, secondColon);
      packetIDStr.trim();
      uint32_t packetID = (uint32_t)packetIDStr.toInt();
      tcValue = packetID;   // For PacketID command, store the packet ID in tcValue.
      tc = "PacketID";      // Set tc to "PacketID"
      int headerLength = secondColon + 1;
      int dataLength = inboundMessageLength - headerLength;
      if (dataLength > 0) {
        bool res = appendPacketToFile(packetID, inboundMessage + headerLength, dataLength);
        if (res) {
          Serial.print("Appended packet ");
          Serial.print(packetID);
          Serial.println(" to flash storage file.");
        }
      }
    }
  }
  else {
    Serial.println("Unknown inbound message type.");
  }

  newMessageAvailable = false;
}
