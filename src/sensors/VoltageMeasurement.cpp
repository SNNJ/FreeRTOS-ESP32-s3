#include "sensors/VoltageMeasurement.h"
#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"

// -------------------
// Pin Definitions
// -------------------
#define VBAT_PIN 7  // GPIO7 for V_BAT sensing
#define USB_PIN  3  // GPIO3 for USB +5V sensing

// -------------------
// Voltage & ADC Settings
// -------------------
// The attenuation level maximum voltages (approximate).
#define ADC_2_5db_MAX 1.25f  // for ADC_2_5db
#define ADC_11db_MAX  3.10f  // for ADC_11db

// Divider ratios
#define VBAT_DIVIDER_RATIO 4.133f
#define USB_DIVIDER_RATIO  1.468f

// We'll store the measured voltages in these static variables.
static float vbatVoltage = 0.0f;
static float usbVoltage  = 0.0f;

// -------------------
// FreeRTOS Task
// -------------------
static void VoltageTask(void *pvParameters)
{
    (void) pvParameters; // Unused parameter

    for (;;)
    {
        // Read raw ADC values
        uint16_t vbatRaw = analogRead(VBAT_PIN);
        uint16_t usbRaw  = analogRead(USB_PIN);

        // Convert raw ADC values to voltage
        // Using the maximum voltage for each attenuation level.
        // (The Arduino core accounts for internal reference scaling.)
        vbatVoltage = (vbatRaw / 4095.0f) * ADC_2_5db_MAX * VBAT_DIVIDER_RATIO;
        usbVoltage  = (usbRaw  / 4095.0f) * ADC_11db_MAX  * USB_DIVIDER_RATIO;

        // Delay for ~1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// -------------------
// Public Functions
// -------------------

// Configure the ADC pins and attenuation.
void initVoltageMeasurement(void)
{
    // For 0-1.25V range on VBAT_PIN:
    analogSetPinAttenuation(VBAT_PIN, ADC_2_5db);
    // For 0-3.10V range on USB_PIN:
    analogSetPinAttenuation(USB_PIN, ADC_11db);
}

// Start the task that updates voltages every second.
void startVoltageMeasurementTask(void)
{
    xTaskCreate(VoltageTask, "VoltageTask", 2048, NULL, 1, NULL);
}

// Return the most recently measured VBAT voltage.
float getVbatVoltage(void)
{
    return vbatVoltage;
}

// Return the most recently measured USB voltage.
float getUsbVoltage(void)
{
    return usbVoltage;
}
