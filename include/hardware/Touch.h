/**
 * @file Touch.h
 * @brief Capacitive touch and push button input interface for CADSE board.
 * 
 * Provides initialization and task management for reading input events,
 * and defines the enum used to communicate button/touch events via a FreeRTOS queue.
 */

#ifndef TOUCH_H
#define TOUCH_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <Arduino.h>

/**
 * @brief Enumeration of all possible button and touch events.
 */

// Define an enum for each button event
typedef enum {
    BUTTON_EVENT_TOUCH_UP,
    BUTTON_EVENT_TOUCH_LEFT,
    BUTTON_EVENT_TOUCH_X,
    BUTTON_EVENT_TOUCH_RIGHT,
    BUTTON_EVENT_TOUCH_DOWN,
    BUTTON_EVENT_PUSH_15, //  Pressing => Low
    BUTTON_EVENT_PUSH_16   //  Pressing => High
} ButtonEvent_t;

/**
 * @brief FreeRTOS queue handle used to send button/touch events between tasks.
 * 
 * Other tasks (e.g., physicalInput) listen to this queue for input events.
 */
// Externally visible queue handle where events are sent
extern QueueHandle_t buttonEventQueue;

/**
 * @brief Initializes GPIOs for capacitive touch and push button input.
 * 
 * Should be called once during setup().
 */
// Initializes the touch sensor & push button module
void initTouchSensor(void);

/**
 * @brief Starts the FreeRTOS task responsible for scanning all input buttons.
 * 
 * This task detects touch and push events and pushes them into buttonEventQueue.
 */
// Starts the task that reads all buttons (touch and push)
void startTouchTask(void);

#endif // TOUCH_H











///////////////////////////////////////////////////////////////////////////////////////
// #ifndef TOUCH_H
// #define TOUCH_H

// #include "FreeRTOS.h"
// #include "task.h"
// #include "queue.h"  

// // Initializes the touch sensor module.
// void initTouchSensor(void);

// // Starts the touch sensor task.
// void startTouchTask(void);

// // Declare the queue handle that will be used to send touch events.
// extern QueueHandle_t touchEventQueue;

// #endif // TOUCH_H


// // //// the previouse version /////



