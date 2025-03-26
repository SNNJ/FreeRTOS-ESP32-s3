/**
 * @file mode1.h
 * @brief Mode 1 logic for microgravity detection using IMU data.
 * 
 * This mode monitors acceleration magnitude to detect free-fall or low-gravity conditions.
 * If gravity drops below a configurable threshold, a buzzer alarm is triggered.
 */
// mode1.h
#ifndef MODE1_H
#define MODE1_H

#include "modes/modegeneral.h"

/**
 * @brief Executes Mode 1 functionality: microgravity detection.
 * 
 * Retrieves IMU data, calculates the acceleration magnitude, and triggers an alarm
 * if the magnitude falls below a preset threshold (`gravityAlaramAt`).
 * Updates display with acceleration values.
 */

// Function prototype for executing mode 1 logic.
void runMode1();

#endif // MODE1_H
