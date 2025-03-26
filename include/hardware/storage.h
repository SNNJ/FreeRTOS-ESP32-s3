/**
 * @file storage.h
 * @brief Interface for SPIFFS-based flash storage operations on the CADSE board.
 * 
 * Provides methods for reading/writing persistent configuration (e.g., default mode)
 * and logging telemetry or packet data to a binary file.
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

/// @brief Path to the file storing the default boot mode.
#define DEFAULT_MODE_FILENAME "/default_mode.txt"
/// @brief Path to the file used for storing telemetry/packet data.
#define DATA_FILE_FILENAME    "/data_file.bin"


/**
 * @brief Initializes the SPIFFS flash storage.
 * 
 * Must be called during setup. Returns true if successful.
 * 
 * @return true if SPIFFS was initialized successfully, false otherwise.
 */
// Initialize flash storage (SPIFFS)
bool initStorage();

/**
 * @brief Reads the default mode from flash memory.
 * 
 * @return int Mode number (0–5), or -1 if the file was not found or read failed.
 */
// Read the default mode from flash; returns -1 if not found or error.
int readDefaultMode();

/**
 * @brief Writes the given mode number as the new default startup mode.
 * 
 * @param mode The mode to write (0–5).
 * @return true if write was successful, false otherwise.
 */
// Write a new default mode (0 to 5) to flash.
bool writeDefaultMode(int mode);

/**
 * @brief Deletes the existing telemetry data file.
 * 
 * Typically called when receiving a reset packet with ID = 1.
 * 
 * @return true if file was cleared successfully, false otherwise.
 */
// Clear the data file (used when packet ID is 1).
bool clearDataFile();

/**
 * @brief Appends raw telemetry or packet data to the binary data file.
 * 
 * @param packetID Packet identifier. If 1, triggers file reset before writing.
 * @param data Pointer to raw byte array to append.
 * @param length Number of bytes to write.
 * @return true if data was appended successfully, false otherwise.
 */
// Append a packet (raw bytes) to the continuous data file.
// The parameter packetID can be used to decide if the file must be cleared first.
bool appendPacketToFile(uint32_t packetID, const uint8_t *data, size_t length);

#endif // STORAGE_H
