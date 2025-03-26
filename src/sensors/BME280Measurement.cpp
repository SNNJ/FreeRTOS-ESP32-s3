#include "sensors/BME280Measurement.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include "FreeRTOS.h"
#include "task.h"

// -------------------------
// Pin definitions for I2C
// -------------------------
#define I2C_SDA 8
#define I2C_SCL 9

// Default I2C address for BME280
#define BME280_ADDR 0x76

// Global BME280 object
static Adafruit_BME280 bme;

// Measured values (updated by the task)
static float temperature = 0.0f;
static float pressure    = 0.0f;
static float humidity    = 0.0f;

// Default measurement period: 500 ms
static uint32_t measurementPeriodMs = 100;

// -------------------------
// BME280 Measurement Task
// -------------------------
static void BME280Task(void *pvParameters)
{
    (void) pvParameters; // Unused

    for (;;)
    {
        // Read temperature (C), pressure (Pa -> hPa), humidity (%)
        temperature = bme.readTemperature();           // °C
        pressure    = bme.readPressure() / 100.0F;     // hPa
        humidity    = bme.readHumidity();              // %

        // Delay for the configured measurement period
        vTaskDelay(pdMS_TO_TICKS(measurementPeriodMs));
    }
}

// -------------------------
// Public Functions
// -------------------------

void initBME280(void)
{
    // Start I2C
    Wire.begin(I2C_SDA, I2C_SCL);

    // Give some time for sensor to power up
    delay(500);

    Serial.println("Initializing BME280 sensor...");
    if (!bme.begin(BME280_ADDR)) {
        Serial.println("Failed to find BME280 sensor! Check connections.");
        while (1) {
            delay(10);
        }
    }
    Serial.println("BME280 sensor initialized successfully!");

    // Optional: configure oversampling & filter
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X2,   // Temperature oversampling
                    Adafruit_BME280::SAMPLING_X16,  // Pressure oversampling
                    Adafruit_BME280::SAMPLING_X1,   // Humidity oversampling
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_500);
}

void startBME280Task(void)
{
    xTaskCreate(BME280Task, "BME280Task", 2048, NULL, 1, NULL);
}

void setBMEPeriod(uint32_t periodMs)
{
    measurementPeriodMs = periodMs;
}

float getBMETemperature(void)
{
    return temperature;
}

float getBMEPressure(void)
{
    return pressure;
}

float getBMEHumidity(void)
{
    return humidity;
}
