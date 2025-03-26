/**
 * @file inbound_processor.h
 * @brief Interface for processing inbound telecommands and data packets from MQTT.
 * 
 * Parses and interprets MQTT messages such as SetMode, SetDefaultMode, and PacketID,
 * updating global variables and triggering corresponding actions on the CADSE board.
 */
#ifndef INBOUND_PROCESSOR_H
#define INBOUND_PROCESSOR_H

#include <Arduino.h>

/// @brief Current operating mode of the system (shared externally).
extern int currentMode;
/// @brief Default mode loaded at boot, may be updated by telecommand.
extern int defaultModeValue;

// -----------------------------
// Telecommand State Variables
// -----------------------------

/**
 * @brief Type of the last received telecommand.
 * 
 * May be one of: "SetMode", "SetDefaultMode", or "PacketID".
 */

// Global variables for telecommand (tc) data
extern String tc;           // Will hold "SetMode", "SetDefaultMode", or "PacketID"

/**
 * @brief Associated value of the last telecommand.
 * 
 * - For SetMode/SetDefaultMode: mode index (0–5)  
 * - For PacketID: numeric identifier of the data packet
 */
extern int tcValue;         // For SetMode/SetDefaultMode, it's the mode value; for PacketID, the packet ID.

/**
 * @brief Validation hash of the received data (used with PacketID).
 * 
 * Calculated using djb2 hashing algorithm, stored as a hex string.
 */
extern String tcHash;       // For PacketID, the computed djb2 hash (in hex string)

/**
 * @brief Parses and handles the latest MQTT inbound message.
 * 
 * Expects the message to be present in the global `inboundMessage` buffer.
 * Updates `tc`, `tcValue`, and `tcHash` based on command type.
 */

// Process the inbound message (assumed to be in the global inboundMessage buffer)
void processInboundMessage();

#endif // INBOUND_PROCESSOR_H