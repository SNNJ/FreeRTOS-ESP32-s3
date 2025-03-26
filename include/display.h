/**
 * @file display.h
 * @brief OLED display interface for multiple satellite visual modes.
 * 
 * Provides display modes and functions for updating the OLED with tabular data,
 * artificial horizon, and real-time rolling plots. Integrates with FreeRTOS tasks.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>


/**
 * @brief Enum representing available display modes.
 */

// Define the available display modes.
enum DisplayMode {
  TABLE_MODE,
  ARTIFICIAL_HORIZON_MODE,
  ROLLING_PLOT_MODE
};


/**
 * @brief Structure representing a single table entry for display.
 * 
 * Used in TABLE_MODE to display name-value-unit triples.
 */
// Table entry structure (shared with main)
struct TableEntry {
  String name;
  float  value;
  String unit;
};

/**
 * @brief Initializes the OLED display and any associated hardware.
 */
// Initializes the OLED display and any required hardware.
void displayInit();

/**
 * @brief Starts the FreeRTOS task that handles display updates.
 * 
 * @param mode Initial display mode (see DisplayMode enum).
 */

// Starts the FreeRTOS display update task in the given mode.
void startDisplayTask(DisplayMode mode);

/**
 * @brief Sets the current display mode.
 * 
 * Can be called at runtime to switch between modes.
 * 
 * @param mode New display mode.
 */

// Change the display mode (can be called at runtime).
void setDisplayMode(DisplayMode mode);

/**
 * @brief Updates the OLED with new telemetry table data.
 * 
 * @param newData Pointer to an array of TableEntry structs.
 * @param count Number of entries in the array.
 */

// Updates the table data (called from main.cpp). This function triggers a display update.
void updateTableData(const TableEntry* newData, int count);

/**
 * @brief Updates the artificial horizon with new pitch and roll values.
 * 
 * @param pitch Pitch angle in degrees.
 * @param roll Roll angle in degrees.
 */
// Called from main.cpp to update the horizon data (ARTIFICIAL_HORIZON_MODE).
// Now accepts both pitch and roll.
void updateHorizonData(float pitch, float roll);

/**
 * @brief Updates the rolling plot display with a new value and timestamp.
 * 
 * @param newValue New data point to plot.
 * @param newTime Timestamp associated with the value.
 * @param xLabel Label for the X-axis.
 * @param yLabel Label for the Y-axis.
 */
// Called from main.cpp to update the rolling plot data.
// newValue: new data point; newTime: the associated time stamp.
// xLabel and yLabel: axis labels for the chart.
void updateRollingPlotData(float newValue, float newTime, const char* xLabel, const char* yLabel);


// // New function to reset the rolling plot data.
// void resetRollingPlotData();


#endif // DISPLAY_H
