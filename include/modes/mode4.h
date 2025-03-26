/**
 * @file mode4.h
 * @brief Mode 4 logic for real-time rolling plot display.
 * 
 * This mode displays a continuous graph of sensor data (e.g., voltage, temperature, pressure)
 * on the OLED. The signal source, amplitude, and time range can be adjusted via telecommands
 * or touch inputs.
 */
// mode4.h
#ifndef MODE4_H
#define MODE4_H

#include "modes/modegeneral.h"

/**
 * @brief Selects which signal source to display in the rolling plot.
 * 
 * Can be modified by user input (touch buttons) or telecommands.
 */
extern int rollingPlotSwitch;

/**
 * @brief Executes Mode 4 functionality: rolling plot visualization.
 * 
 * Continuously plots selected sensor data over time with dynamic labels.
 * Used for visual trend monitoring of analog signals.
 */
// Function prototype for executing mode 4 logic.
void runMode4();

#endif // MODE4_H
