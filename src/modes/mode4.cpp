// mode4.cpp
#include "modes/modegeneral.h"
#include "modes/mode4.h"

void runMode4() {
    // Static variable to ensure the beep is triggered only once upon entering mode 4.
    
    if (previousMode != 4) {
        Serial.println("Beeping 4 times...");
        buzzerAction(4);
        previousMode = 4;
    }
    
    // Set the display mode to ROLLING_PLOT_MODE.
    setDisplayMode(ROLLING_PLOT_MODE);
    
    // Local static time variable to keep track of the elapsed time for the rolling plot.
    static float t = 0.0;
    float value = 0.0;
    
    // Retrieve the latest IMU data.
    IMUEvents_t data = getIMUData();
    
    // Update the rolling plot based on the current rollingPlotSwitch value.
    switch (rollingPlotSwitch) {
        case 1: {
            value = data.accel.acceleration.x;
            updateRollingPlotData(value, t, "Time (s)", "Acc X m/s^2");
            break;
        }
        case 2: {
            value = getBMEHumidity();
            updateRollingPlotData(value, t, "Time (s)", "Humi %");
            break;
        }
        case 3: {
            value = data.temp.temperature;
            updateRollingPlotData(value, t, "Time (s)", "Temp C");
            break;
        }
        default:
            break;
    }
    
    // Increase time; adjust the increment as needed.
    t += 0.2;
}
