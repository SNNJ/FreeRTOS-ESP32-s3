/**
 * @file Buzzer.h
 * @brief Interface for buzzer initialization and control on the CADSE board.
 * 
 * Provides simple functions to trigger buzzer alerts or sound patterns based on satellite events.
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

/**
 * @brief Initializes the buzzer hardware.
 * 
 * Configures the required GPIO pin for output.
 * Should be called once in the setup routine.
 */
void initBuzzer(void);

/**
 * @brief Perform a buzzer action based on the parameters.
 * 
 * @param times       Number of times to beep or meow. Default = 1.
 * @param speakNumber If true, interpret 'times' as digits and beep each digit.
 * @param meow        If true, ignore speakNumber and produce a "cat meow" sound 'times' times.
 */
void buzzerAction(int times = 1, bool speakNumber = false, bool meow = false);

#endif // BUZZER_H
