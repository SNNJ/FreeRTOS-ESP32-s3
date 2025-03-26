// mode1.cpp
#include "modes/modegeneral.h"
#include "modes/mode1.h"

void runMode1() {
    // Static variable to ensure that the beep is triggered only once upon entering mode 1.
    //static int previousMode1 = -1;
    if (previousMode != 1) {
        Serial.println("Beeping 1 times...");
        buzzerAction(1);
        previousMode = 1;
    }
    
    // Retrieve the latest IMU data.
    IMUEvents_t imuData = getIMUData();
    
    // Calculate the magnitude from the accelerometer readings.
    float magnitude = sqrt(
        (imuData.accel.acceleration.y * imuData.accel.acceleration.y) +
        (imuData.accel.acceleration.z * imuData.accel.acceleration.z) +
        (imuData.accel.acceleration.x * imuData.accel.acceleration.x)
    );
    
    // If the magnitude is below the threshold, trigger the gravity alarm.
    if (magnitude < gravityAlaramAt) {
        Serial.print("Acceleration magnitude: ");
        Serial.println(magnitude);
        buzzerAction(1, false, true);

        ledController.blinkLED(7, 1, 400);
        ledController.blinkLED(10, 1, 400);
        ledController.blinkLED(13, 1, 400);
    }
    
    // Update the display with sensor and mode data.
    updateTableData(std::array<TableEntry, 6>{
        {
            {"Mode", 1, ""},
            {"Magnitude", magnitude, ""},
            {"AccelX", imuData.accel.acceleration.x, "m/s^2"},
            {"AccelY", imuData.accel.acceleration.y, "m/s^2"},
            {"AccelZ", imuData.accel.acceleration.z, "m/s^2"},
            {"gravityAlarmAt", gravityAlaramAt, ""}
        }
    }.data(), 6);
}
