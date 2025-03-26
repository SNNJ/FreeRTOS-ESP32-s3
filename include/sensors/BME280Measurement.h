/**
 * @file BME280Measurement.h
 * @brief Interface for BME280 environmental sensor (temperature, pressure, humidity).
 * 
 * Provides initialization, periodic measurement task management, and data access functions
 * for the Bosch BME280 sensor over I2C.
 */
#ifndef BME280MEASUREMENT_H
#define BME280MEASUREMENT_H

#include <Arduino.h>

// Initializes the BME280 sensor over I2C.
void initBME280(void);

// Starts the FreeRTOS task that periodically measures sensor data.
void startBME280Task(void);

// Sets the measurement period (in milliseconds). Default is 500 ms.
void setBMEPeriod(uint32_t periodMs);

// Getter functions for the latest sensor data.
float getBMETemperature(void);
float getBMEPressure(void);
float getBMEHumidity(void);

#endif // BME280MEASUREMENT_H
