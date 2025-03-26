#include "sensors/IMU.h"
#include <stdio.h>

// Create the LSM6DS object.
Adafruit_LSM6DS lsm6ds = Adafruit_LSM6DS();

// Define SPI pins for ESP32-S3.
#define SPI_MOSI 11
#define SPI_MISO 13
#define SPI_SCLK 12
#define SPI_CS   10

// Global variable to hold the latest sensor events.
static IMUEvents_t imuEvents;

// Global variable for effective update period (default = 500 ms).
static uint32_t effectivePeriodMs = 500;

// Global task handle for the IMU update task.
static TaskHandle_t imuTaskHandle = NULL;

// Initialize the IMU sensor.
void initIMU(void)
{

    lsm6ds.begin_SPI(SPI_CS, SPI_SCLK, SPI_MISO, SPI_MOSI);
  // Allow time for the system to settle.
  delay(500);
  
//   // Initialize SPI and the LSM6DS sensor.
//   if (!lsm6ds.begin_SPI(SPI_CS, SPI_SCLK, SPI_MISO, SPI_MOSI))
//   {
//     Serial.println("Failed to find LSM6DS chip!");
//     while (1) { delay(10); }
//   }
  Serial.println("LSM6DS IMU Found!");

  // Configure sensor settings.
  lsm6ds.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);       // 2G range.
  lsm6ds.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);       // 250 dps range.
  lsm6ds.setAccelDataRate(LSM6DS_RATE_26_HZ);          // 104 Hz.  LSM6DS_RATE_26_HZ
  lsm6ds.setGyroDataRate(LSM6DS_RATE_26_HZ);           // 104 Hz.  LSM6DS_RATE_26_HZ
}

// Update the sensor data (read sensor events).
static void updateIMU(void)
{
  lsm6ds.getEvent(&imuEvents.accel, &imuEvents.gyro, &imuEvents.temp);
}

// Returns the latest IMU sensor events.
IMUEvents_t getIMUData(void)
{
  return imuEvents;
}

// Change the effective update period.
void setIMUEffectivePeriod(uint32_t periodMs)
{
  effectivePeriodMs = periodMs;
  Serial.print("IMU effective period set to: ");
  Serial.print(effectivePeriodMs);
  Serial.println(" ms");
}

// The IMU task that periodically updates sensor data.
static void IMUTask(void *pvParameters)
{
  (void) pvParameters; // Unused parameter.
  TickType_t xLastWakeTime = xTaskGetTickCount();
  
  for(;;)
  {
    updateIMU();
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(effectivePeriodMs));
  }
}

// Start the IMU update task.
void startIMUTask(void)
{
  xTaskCreate(IMUTask, "IMUTask", 2048, NULL, 1, &imuTaskHandle);
}

// Suspend (stop) the IMU update task.
void stopIMUTask(void)
{
  if (imuTaskHandle != NULL)
  {
    vTaskSuspend(imuTaskHandle);
    Serial.println("IMU task suspended.");
  }
}

// Resume the IMU update task.
void resumeIMUTask(void)
{
  if (imuTaskHandle != NULL)
  {
    vTaskResume(imuTaskHandle);
    Serial.println("IMU task resumed.");
  }
}
