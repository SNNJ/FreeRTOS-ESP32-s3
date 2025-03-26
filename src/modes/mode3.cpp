// mode3.cpp
#include "modes/modegeneral.h"
#include "modes/mode3.h"

void runMode3() {
    // Static variable to ensure the beep is triggered only once upon entering mode 3.
    
    if (previousMode != 3) {
        Serial.println("Beeping 3 times...");
        buzzerAction(3);
        previousMode = 3;
    }

    // Retrieve the latest IMU data.
    IMUEvents_t data = getIMUData();

    // Calculate angles in degrees.
    float angleX = atan2(
        data.accel.acceleration.x, 
        sqrt(data.accel.acceleration.y * data.accel.acceleration.y + data.accel.acceleration.z * data.accel.acceleration.z)
    ) * 180.0 / M_PI;  // pitch is angleX

    float angleY = atan2(
        data.accel.acceleration.y, 
        sqrt(data.accel.acceleration.x * data.accel.acceleration.x + data.accel.acceleration.z * data.accel.acceleration.z)
    ) * 180.0 / M_PI;  // roll is angleY

    // Set the display to ARTIFICIAL_HORIZON_MODE.
    setDisplayMode(ARTIFICIAL_HORIZON_MODE);

    // Update horizon display data with the calculated pitch and roll values.
    updateHorizonData(angleX, angleY);

    // Optionally, you could print the angles for debugging:
    // Serial.printf("Pitch: %.1f, Roll: %.1f\n", angleX, angleY);
}
