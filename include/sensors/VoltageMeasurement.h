/**
 * @file VoltageMeasurement.h
 * @brief Interface for battery and USB voltage measurement on the CADSE board.
 * 
 * Provides initialization, measurement, and getter functions for voltage monitoring,
 * along with a FreeRTOS task to update readings periodically.
 */


#ifndef VOLTAGEMEASUREMENT_H
#define VOLTAGEMEASUREMENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the ADC hardware and sets appropriate attenuation levels.
 * 
 * Should be called once during setup before voltage readings are taken.
 */
// Initializes the ADC pins and attenuation levels.
void initVoltageMeasurement(void);

/**
 * @brief Starts a FreeRTOS task that continuously reads battery and USB voltage.
 * 
 * The task runs in the background and updates internal values at fixed intervals.
 */
// Starts the FreeRTOS task that periodically reads the voltages.
void startVoltageMeasurementTask(void);

/**
 * @brief Retrieves the most recently measured battery voltage.
 * 
 * @return float Battery voltage in volts.
 */
// Getter functions to retrieve the latest measured voltages.
float getVbatVoltage(void);

/**
 * @brief Retrieves the most recently measured USB voltage.
 * 
 * @return float USB voltage in volts.
 */
float getUsbVoltage(void);

#ifdef __cplusplus
}
#endif

#endif // VOLTAGEMEASUREMENT_H
