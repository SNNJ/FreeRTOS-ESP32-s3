/**
 * @file IMU.h
 * @brief Interface for initializing and managing the LSM6DS IMU sensor.
 * 
 * Provides functions to start, stop, and control the IMU data acquisition task,
 * and to retrieve accelerometer, gyroscope, and temperature sensor readings.
 */
#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"
#include <Adafruit_LSM6DS.h>
#include <Adafruit_Sensor.h>

// Structure to hold IMU sensor events.

/**
 * @brief Struct holding the latest IMU sensor readings.
 * 
 * Includes accelerometer, gyroscope, and temperature sensor events.
 */
typedef struct {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
} IMUEvents_t;
/**
 * @brief Initializes the LSM6DS IMU sensor via SPI.
 * 
 * Sets up communication, configures the sensor, and prepares for data acquisition.
 */
// Initializes the IMU sensor (SPI initialization, configuration, etc.).
void initIMU(void);
/**
 * @brief Starts the FreeRTOS task that continuously reads data from the IMU sensor.
 */
// Starts the IMU update task.
void startIMUTask(void);
/**
 * @brief Suspends the IMU reading task without deinitializing the sensor.
 */
// Stops (suspends) the IMU update task.
void stopIMUTask(void);
/**
 * @brief Resumes the IMU task if it was previously suspended.
 */
// Resumes the IMU update task.
void resumeIMUTask(void);
/**
 * @brief Sets the effective update period of the IMU task.
 * 
 * @param periodMs Desired update rate in milliseconds.
 */
// Sets the effective update period (in milliseconds) for the IMU update task.
void setIMUEffectivePeriod(uint32_t periodMs);
/**
 * @brief Retrieves the latest IMU readings from the background task.
 * 
 * @return IMUEvents_t Struct containing acceleration, gyro, and temperature values.
 */
// Returns the latest sensor events.
IMUEvents_t getIMUData(void);

#endif // IMU_H
