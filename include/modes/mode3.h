/**
 * @file mode3.h
 * @brief Mode 3 logic for artificial horizon (attitude indicator) visualization.
 * 
 * This mode calculates the roll angle using IMU data and displays an artificial horizon
 * on the OLED, simulating spacecraft orientation for landing maneuvers.
 */

// mode3.h
#ifndef MODE3_H
#define MODE3_H

#include "modes/modegeneral.h"

/**
 * @brief Executes Mode 3 functionality: artificial horizon display.
 * 
 * Calculates roll angle from accelerometer data assuming Earth's gravity (9.81 m/s²).
 * Updates the OLED with a visual representation of the horizon line.
 */

// Function prototype for executing mode 3 logic.
void runMode3();

#endif // MODE3_H
