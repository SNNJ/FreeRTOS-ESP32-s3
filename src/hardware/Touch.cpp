#include "hardware/Touch.h"

//------------------
// Pin Definitions
//------------------
// Touch buttons
#define TOUCH_UP_PIN     1
#define TOUCH_LEFT_PIN   2
#define TOUCH_X_PIN      4
#define TOUCH_RIGHT_PIN  6
#define TOUCH_DOWN_PIN   5

// Push buttons
#define PUSH_15_PIN      15  // Pressing => LOW
#define PUSH_16_PIN      16  // Pressing => HIGH

// Touch threshold
static const int touchThreshold = 40000; // Updated to 40000

// Debounce delay
static const int debounceDelayMs = 200;

// Create the event queue (declared extern in Touch.h)
QueueHandle_t buttonEventQueue = NULL;

/**
 * Checks a touch pin and sends an event if the reading exceeds touchThreshold.
 */
static void checkTouchPin(int pin, ButtonEvent_t evt)
{
    int touchValue = touchRead(pin);
    if (touchValue > touchThreshold) {
        // Send event to the queue (non-blocking)
        xQueueSend(buttonEventQueue, &evt, 0);
        // Debounce to avoid rapid re-trigger
        vTaskDelay(pdMS_TO_TICKS(debounceDelayMs));
    }
}

/**
 * Checks a push pin with either active-low or active-high logic.
 * @param pin: The GPIO pin to read.
 * @param evt: The event to send if triggered.
 * @param activeLow: If true, pressing => LOW. If false, pressing => HIGH.
 */

 ///// <<<< LOGIC I >>>>>
// static void checkPushPin(int pin, ButtonEvent_t evt, bool activeLow)
// {
//     int state = digitalRead(pin);
//     bool isPressed = (activeLow && state == LOW) || (!activeLow && state == HIGH);

//     if (isPressed) {
//         xQueueSend(buttonEventQueue, &evt, 0);
//         vTaskDelay(pdMS_TO_TICKS(debounceDelayMs*3));
//     }
// }

///// <<<< LOGIC II >>>>>
static void checkPushPin(int pin, ButtonEvent_t evt, bool activeLow)
{
    // Read the current state.
    int state = digitalRead(pin);
    bool isPressed = (activeLow && state == LOW) || (!activeLow && state == HIGH);

    // If we see a press, wait 100ms and check again.
    if (isPressed) {
        vTaskDelay(pdMS_TO_TICKS(150));  // Wait 100 ms
        // Check the state again.
        int secondState = digitalRead(pin);
        bool stillPressed = (activeLow && secondState == LOW) || (!activeLow && secondState == HIGH);

        if (stillPressed) {
            // Now we know the button was pressed continuously for 100 ms.
            xQueueSend(buttonEventQueue, &evt, 0);
            // (Optional) Add a debounce delay here to avoid repeated triggers.
            vTaskDelay(pdMS_TO_TICKS(200)); 
        }
    }
}


//----------------------------------------
// Task that reads all buttons every ~100ms
//----------------------------------------
static void TouchTask(void *pvParameters)
{
    (void) pvParameters; // Unused

    for (;;) {
        // 1) Check each touch button
        checkTouchPin(TOUCH_UP_PIN,     BUTTON_EVENT_TOUCH_UP);
        checkTouchPin(TOUCH_LEFT_PIN,   BUTTON_EVENT_TOUCH_LEFT);
        checkTouchPin(TOUCH_X_PIN,      BUTTON_EVENT_TOUCH_X);
        checkTouchPin(TOUCH_RIGHT_PIN,  BUTTON_EVENT_TOUCH_RIGHT);
        checkTouchPin(TOUCH_DOWN_PIN,   BUTTON_EVENT_TOUCH_DOWN);

        // 2) Check each push button
        // GPIO 15 is pull-up => pressing => LOW
        checkPushPin(PUSH_15_PIN, BUTTON_EVENT_PUSH_15, true);
        // GPIO 16 is reversed => pressing => HIGH
        checkPushPin(PUSH_16_PIN, BUTTON_EVENT_PUSH_16, false);

        // Wait ~100ms before checking again
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void initTouchSensor(void)
{
    // For touch pins, no pinMode() is required.
    // For push buttons:
    pinMode(PUSH_15_PIN, INPUT_PULLUP); // Pressing => LOW
    // If you need a built-in pull-down for pin 16, you can do INPUT_PULLDOWN,
    // but the ESP32 has limited internal pull-down support. 
    // If the hardware externally pulls it down, do pinMode(PUSH_16_PIN, INPUT).
    pinMode(PUSH_16_PIN, INPUT); 
}

void startTouchTask(void)
{
    // Create the queue if it doesn't exist yet (max 10 events)
    if (buttonEventQueue == NULL) {
        buttonEventQueue = xQueueCreate(10, sizeof(ButtonEvent_t));
    }

    // Create the TouchTask
    xTaskCreate(TouchTask, "TouchTask", 2048, NULL, 1, NULL);
}



