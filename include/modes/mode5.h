/**
 * @file mode5.h
 * @brief Mode 5 logic for telecommand verification and data transmission status.
 * 
 * Mode 5 visualizes telecommand activity, displaying the latest command name,
 * its value, and a verification hash on the OLED screen. It is primarily used
 * for debugging and confirming successful reception of commands like SetMode,
 * SetDefaultMode, and PacketID (used for file uploads).
 */

// mode5.h
#ifndef MODE5_H
#define MODE5_H

#include "modes/modegeneral.h"

/**
 * @brief Executes Mode 5 functionality: telecommand and packet monitoring.
 * 
 * On first entry, triggers a 5-beep signal via buzzer. Then, it sets the display
 * to TABLE_MODE and shows telemetry feedback such as command type (`tc`),
 * its numeric value (`tcValue`), and its hash (`tcHash`) for validation.
 * Used to confirm successful reception and handling of telecommands.
 */
// Function prototype for executing mode 5 logic.
void runMode5();

#endif // MODE5_H
