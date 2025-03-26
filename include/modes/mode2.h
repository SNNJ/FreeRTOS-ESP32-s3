/**
 * @file mode2.h
 * @brief Mode 2 logic for pressure drop detection.
 * 
 * This mode monitors the environmental pressure using the BME280 sensor.
 * If the pressure drops more than 10 hPa from the initial value at boot,
 * a visual (LED) and optional audible (buzzer) alarm is triggered.
 */

// mode2.h
#ifndef MODE2_H
#define MODE2_H

#include "modes/modegeneral.h"

/**
 * @brief Executes Mode 2 functionality: cabin pressure monitoring.
 * 
 * Compares current pressure to `initPressure`. If the drop exceeds 10 hPa,
 * triggers a warning using LEDs and optionally the buzzer.
 * Displays current pressure on the OLED.
 */

// Function prototype for executing mode 2 logic.
void runMode2();

#endif // MODE2_H
