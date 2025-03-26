/**
 * @file Led_light.h
 * @brief LED control interface using the MCP23017 I/O expander.
 * 
 * Provides a class to manage blinking patterns for LEDs connected to an I2C-based GPIO expander,
 * used for status indication on the CADSE board.
 */
#ifndef LED_LIGHT_H
#define LED_LIGHT_H

#include <Arduino.h>
#include <Adafruit_MCP23X17.h>

/**
 * @class LedLight
 * @brief Class for controlling LEDs via an MCP23017 GPIO expander.
 */
class LedLight {
public:

    /**
     * @brief Constructor for the LedLight class.
     */
    LedLight();
    
    /**
     * @brief Initializes the MCP23017 and configures the GPIOs.
     * 
     * Must be called before using any LED-related functions.
     */
    void begin();
        /**
     * @brief Blinks a specific LED a number of times with a given period.
     * 
     * @param ledPin GPIO pin (on MCP23017) to control.
     * @param times Number of times the LED should blink.
     * @param period Duration of one on/off cycle in milliseconds.
     */
    void blinkLED(int ledPin, int times, int period);

private:
    Adafruit_MCP23X17 mcp;
    void turnOn(int ledPin);
    void turnOff(int ledPin);
    static void ledTask(void* parameters);
};

/**
 * @brief Global instance of the LED controller used throughout the project.
 */
// Declare ledController as an external variable
extern LedLight ledController;

#endif


