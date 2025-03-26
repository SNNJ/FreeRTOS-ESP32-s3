#include "hardware/Led_light.h"

LedLight ledController;  // Global LED Controller

LedLight::LedLight() {}

void LedLight::begin() {
    Wire.begin();  // Start I2C

    Serial.println("Initializing MCP23017...");
    if (!mcp.begin_I2C(0x20)) { // Initialize MCP23017 at I2C 0x20
        Serial.println("MCP23017 not found! Check wiring.");
        while (1);
    }

    Serial.println("MCP23017 initialized successfully.");

    // Configure GPIOs as OUTPUTs and set HIGH (LEDs OFF)
    for (int pin = 7; pin <= 15; pin++) {
        mcp.pinMode(pin, OUTPUT);
        mcp.digitalWrite(pin, HIGH);
    }
}

void LedLight::turnOn(int ledPin) {
    // Serial.print("Turning ON LED at pin: ");
    // Serial.println(ledPin);
    mcp.digitalWrite(ledPin, LOW); // LEDs are active LOW
}

void LedLight::turnOff(int ledPin) {
    // Serial.print("Turning OFF LED at pin: ");
    // Serial.println(ledPin);
    mcp.digitalWrite(ledPin, HIGH);
}

void LedLight::blinkLED(int ledPin, int times, int period) {
    // Serial.print("Scheduling blink for pin ");
    // Serial.print(ledPin);
    // Serial.print(", times: ");
    // Serial.print(times);
    // Serial.print(", period: ");
    // Serial.println(period);

    int* taskParams = new int[3]{ledPin, times, period};
    xTaskCreate(ledTask, "LED Task", 2048, taskParams, 1, NULL);
}

void LedLight::ledTask(void* parameters) {
    int* params = (int*)parameters;
    int ledPin = params[0];
    int times = params[1];
    int period = params[2];

    delete[] params; // Free allocated memory

    // Serial.print("Executing LED task for pin ");
    // Serial.print(ledPin);
    // Serial.print(", times: ");
    // Serial.print(times);
    // Serial.print(", period: ");
    // Serial.println(period);

    for (int i = 0; i < times; i++) {
        // Serial.print("LED ON (");
        // Serial.print(ledPin);
        // Serial.println(")");
        ledController.turnOn(ledPin);

        vTaskDelay(pdMS_TO_TICKS(period / 2));

        // Serial.print("LED OFF (");
        // Serial.print(ledPin);
        // Serial.println(")");
        ledController.turnOff(ledPin);

        vTaskDelay(pdMS_TO_TICKS(period / 2));
    }

    // Serial.println("Task completed, deleting...");
    vTaskDelete(NULL); // Kill the task
}




