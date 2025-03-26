// mode2.cpp
#include "modes/modegeneral.h"
#include "modes/mode2.h"

void runMode2() {
    // Static variable to ensure the beep is triggered only once upon entering mode 2.
    
    if (previousMode != 2) {
        Serial.println("Beeping 1 times...");
        buzzerAction(2);
        previousMode = 2;
    }

    // Set display to TABLE_MODE.
    setDisplayMode(TABLE_MODE);

    // Update the display with BME280 sensor data.
    updateTableData(std::array<TableEntry, 5>{
        {
            {"Temp", getBMETemperature(), "°C"},
            {"Humi", getBMEHumidity(), "%"},
            {"Pres", getBMEPressure(), "hPa"},
            {"initPres", initPressure, "hPa"},
            {"thresholdPres", initPressure - deltaPressure, "hPa"}
        }
    }.data(), 5);

    // Check if the current pressure is below the threshold.
    if (getBMEPressure() < initPressure - deltaPressure) {
        ledController.blinkLED(7, 1, 400);
        ledController.blinkLED(10, 1, 400);
        ledController.blinkLED(13, 1, 400);
        buzzerAction(1, false, true);
    }
}
