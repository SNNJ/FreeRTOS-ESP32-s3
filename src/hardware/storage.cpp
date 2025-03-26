#include "hardware/storage.h"
#include <SPIFFS.h>
#include <FS.h>

bool initStorage() {
  if (!SPIFFS.begin(true)) { // auto-format if mount fails
    Serial.println("An error occurred while mounting SPIFFS");
    return false;
  }
  return true;
}

int readDefaultMode() {
  File file = SPIFFS.open(DEFAULT_MODE_FILENAME, "r");
  if (!file) {
    Serial.println("Failed to open default mode file");
    return -1;
  }
  String modeStr = file.readStringUntil('\n');
  file.close();
  int mode = modeStr.toInt();
  if (mode < 0 || mode > 5) {
    Serial.println("Invalid default mode in file");
    return -1;
  }
  return mode;
}

bool writeDefaultMode(int mode) {
  if (mode < 0 || mode > 5) {
    Serial.println("Mode out of range");
    return false;
  }
  File file = SPIFFS.open(DEFAULT_MODE_FILENAME, "w");
  if (!file) {
    Serial.println("Failed to open default mode file for writing");
    return false;
  }
  file.print(String(mode));
  file.close();
  return true;
}

bool clearDataFile() {
  // Open in "w" mode to truncate/clear the file.
  File file = SPIFFS.open(DATA_FILE_FILENAME, "w");
  if (!file) {
    Serial.println("Failed to open data file for clearing");
    return false;
  }
  file.close();
  return true;
}

bool appendPacketToFile(uint32_t packetID, const uint8_t *data, size_t length) {
  // If this is the first packet, clear the file first.
  if (packetID == 1) {
    if (!clearDataFile()) {
      return false;
    }
  }
  // Open file in append mode.
  File file = SPIFFS.open(DATA_FILE_FILENAME, "a");
  if (!file) {
    Serial.println("Failed to open data file for appending");
    return false;
  }
  size_t written = file.write(data, length);
  file.close();
  return (written == length);
}
