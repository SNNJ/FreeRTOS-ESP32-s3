/**
 * @file modegeneral.h
 * @brief Shared declarations and helper interfaces for all satellite operation modes.
 * 
 * This file includes common global variables and function prototypes
 * used across multiple mode implementations (mode1 to mode5).
 * It also provides access to shared sensor modules, display utilities,
 * and hardware interfaces like buzzer and LEDs.
 */

// modegeneral.h
#ifndef MODEGENERAL_H
#define MODEGENERAL_H

#include <Arduino.h>
#include <array>
#include <math.h>

// Include modules that are common to all modes
#include "display.h"        // For setDisplayMode, updateTableData, updateHorizonData, updateRollingPlotData, etc.
#include "hardware/Buzzer.h"         // For buzzerAction
#include "sensors/IMU.h"            // For getIMUData and the IMUEvents_t structure
#include "sensors/BME280Measurement.h"  // For getBMETemperature, getBMEPressure, getBMEHumidity
#include "hardware/Led_light.h"      // For ledController

// -----------------------
// Common Global Variables
// -----------------------

// The current mode (0 to 6). Defined in main, declared here for external use.
extern int currentMode;

extern int previousMode;

// Variables used for mode 2 (pressure measurement)
extern float initPressure;
extern float deltaPressure;

// Variable used in mode 1 for IMU alarm threshold.
extern float gravityAlaramAt;

// Variable used in mode 4 for the rolling plot time axis.
extern float modeGeneralTime;

// Variable used in mode 5 (telemetry string, value, and hash)
/**
 * @brief Telecommand identifier string used in Mode 5.
 */
extern String tc;

/**
 * @brief Value associated with the current telecommand (Mode 5).
 */
extern int tcValue;

/**
 * @brief Hash used to verify telecommand authenticity (Mode 5).
 */
extern String tcHash;

// -----------------------
// Common Helper Function Prototypes
// -----------------------

// You can add common helper functions that all mode files might use.
// For example, a function to update the display with the current mode information:
void updateModeDisplay();

#endif // MODEGENERAL_H
