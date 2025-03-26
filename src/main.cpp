///////////////////////*********************************************////////////////////


/**
 * @file main.cpp
 * @author Siamak Najafinia
 * @brief CADSE Satellite Firmware
 * @date 2024
 * @version 1.0
 * @university_id *****
 * 
 * @section Program_Objective
 * Simulate satellite-like behavior using sensor inputs, visual output, and communication features on the CADSE v5 platform.
 * 
 * @section Project Summary: CADSE Satellite Firmware
 * This project implements firmware for a simulated satellite board (CADSE) that operates across six distinct modes, each representing a core satellite function. Upon boot, the OLED display visualizes the startup process and reports errors such as WiFi failures or low battery. Users can navigate between modes using left and right capacitive touch buttons, with the buzzer signaling the selected mode (e.g., Mode 1 = 1 beep). Mode 1 detects microgravity and activates an alarm; Mode 2 monitors pressure drops for "cabin" safety and triggers visual/audible alerts; Mode 3 simulates an artificial horizon for landing by calculating roll; Mode 4 displays rolling plots of analog telemetry; and Mode 5 allows for custom, creative satellite-like behaviors. The system sends continuous telemetry every second—including sensor data, current mode, touch/button states, and WiFi RSSI—using MQTT over WiFi. It also supports receiving telecommands to switch modes or configure a persistent default startup mode. The code is modular, readable, and Doxygen-annotated, supporting portability and maintainability across all CADSE v5 boards.
 */

#include <Arduino.h>
#include "display.h"
#include "sensors/VoltageMeasurement.h"
#include "sensors/IMU.h"         // IMU sensor header.
#include "FreeRTOS.h"
#include "task.h"
#include "sensors/BME280Measurement.h"
#include "hardware/Touch.h"
#include <stdio.h>
#include <array>
#include "MqttTask.h"
#include "hardware/Buzzer.h"
#include <math.h>  // For sqrt()
#include "hardware/Led_light.h"
#include "hardware/storage.h"
#include <SPIFFS.h>
#include "inbound_processor.h"
#include "modes/modegeneral.h"
#include "modes/mode1.h"
#include "modes/mode2.h"
#include "modes/mode3.h"
#include "modes/mode4.h"
#include "modes/mode5.h"



// Global variable to store the current mode (0 to 6)
// Default mode is 0.


/**
 * @brief Current mode of the satellite system (ranges from 0 to 5).
 */
int currentMode = 0;

/**
 * @brief Default mode loaded from flash memory at boot.
 */
int defaultModeValue = 0;

/**
 * @brief Initial pressure recorded at boot, used for pressure drop detection in Mode 2.
 */
float initPressure = 0;

/**
 * @brief Threshold difference in pressure to trigger an alarm in Mode 2.
 */
float deltaPressure = 10;

/**
 * @brief Mode selector for rolling plot display in Mode 4.
 */
int rollingPlotSwitch = 3;

/**
 * @brief Threshold for detecting microgravity in Mode 1.
 */
float gravityAlaramAt = 1;

/**
 * @brief Stores the last known mode to prevent redundant buzzer alerts.
 */
int previousMode = -1;
 
/**
 * @brief Status of the last touch input (e.g., TOUCH_LEFT, TOUCH_DOWN).
 */

String touchStatus = "";

/**
 * @brief Status of the last push button event (e.g., PUSH_BUTTON_15).
 */

String buttonStatus = "";


/**
 * @brief FreeRTOS task that monitors and processes inbound MQTT messages.
 * 
 * Checks for new messages every 500 ms and calls processInboundMessage() if available.
 * 
 * @param pvParameters Unused.
 */

// ----------------- Inbound Message Task -----------------
void processInboundMessage();

// FreeRTOS task that checks for new inbound messages every 500 ms.
void inboundTask(void *pvParameters) {
  for (;;) {
    if (newMessageAvailable) {
      processInboundMessage();
      newMessageAvailable = false;
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}


/**
 * @brief FreeRTOS task that collects sensor data and publishes it via MQTT.
 * 
 * Reads IMU and BME280 data every second, builds a JSON payload, and sends it to the MQTT broker.
 * Resets touch and button status flags after publishing.
 * 
 * @param pvParameters Unused.
 */

// ----------------- Sensor Task -----------------
// This task reads sensor data, updates the display, and publishes an MQTT payload every 1 second.
void sensorTask(void *pvParameters) {
  (void) pvParameters; // Unused parameter
  int rssi;

  while (1) {
    // Retrieve the latest IMU data.
    IMUEvents_t imuData = getIMUData();

    // Compose a JSON payload with all sensor values.
    String payload = "{";
    payload += "\"mode\":" + String(currentMode) + ",";
    payload += "\"voltages\":{";
    payload += "\"BattVolt\":" + String(getVbatVoltage(), 2) + ",";
    payload += "\"BusVolt\":"  + String(getUsbVoltage(), 2);
    payload += "},";
    payload += "\"IMU\":{";
    payload += "\"AccelX\":" + String(imuData.accel.acceleration.x, 1) + ",";
    payload += "\"AccelY\":" + String(imuData.accel.acceleration.y, 1) + ",";
    payload += "\"AccelZ\":" + String(imuData.accel.acceleration.z, 1) + ",";
    payload += "\"GyroX\":"  + String(imuData.gyro.gyro.x, 1) + ",";
    payload += "\"GyroY\":"  + String(imuData.gyro.gyro.y, 1) + ",";
    payload += "\"GyroZ\":"  + String(imuData.gyro.gyro.z, 1) + ",";
    payload += "\"Temp\":"   + String(imuData.temp.temperature, 0);
    payload += "},";
    payload += "\"BME\":{";
    payload += "\"Temp\":" + String(getBMETemperature(), 0) + ",";
    payload += "\"Pres\":" + String(getBMEPressure(), 1) + ",";
    payload += "\"Humi\":" + String(getBMEHumidity(), 0);
    // payload += "}";
    // payload += "}";
    
    payload += "},";

    rssi= getWiFiRSSI();

    // WiFi signal strength
    payload += "\"WiFiRSSI\":" + String(rssi);

    // // Touch sensor status
    // payload += "\"Touch\":\"" + touchStatus + "\",";

    // // Push button status
    // payload += "\"Button\":\"" + buttonStatus + "\"";
    
    payload += "}";

   
    touchStatus = "";
    buttonStatus = "";

    // Publish the JSON payload via MQTT.
    publishMqttMessage(payload);

    // Delay for 1000 ms (1 second).
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}


/**
 * @brief FreeRTOS task that handles capacitive touch and button inputs.
 * 
 * Responds to user input events like TOUCH_LEFT or PUSH_BUTTON_15, modifies mode or parameters accordingly,
 * and updates global status strings for telemetry reporting.
 * 
 * @param pvParameters Unused.
 */

// ----------------- Physical input Task -----------------

// Task that receives button events and toggles the LED or does other actions.
static void physicalInput(void *pvParameters)
{
    (void) pvParameters; // Unused

    ButtonEvent_t evt;
    for (;;) {
        // Wait indefinitely for an event from the queue
        if (xQueueReceive(buttonEventQueue, &evt, portMAX_DELAY) == pdTRUE) {
            switch (evt) {
              case BUTTON_EVENT_TOUCH_UP:
              touchStatus = "TOUCH_UP";
              if (currentMode < 5) {
                  currentMode++;  // Increase mode, max is 6
              }
              Serial.printf("Event: TOUCH_UP -> Current Mode: %d\n", currentMode);
              break;
                case BUTTON_EVENT_TOUCH_LEFT:
                touchStatus = "TOUCH_LEFT";
                    Serial.println("Event: TOUCH_LEFT");

              if (currentMode == 2) {

                deltaPressure--;  // Increase mode, max is 6
                }

                else if (currentMode == 4) {
                  
                rollingPlotSwitch = (rollingPlotSwitch < 2) ?  1 : rollingPlotSwitch - 1;
                Serial.printf("Event: TOUCH_LEFT %d\n",rollingPlotSwitch );


                // updateTableData(std::array<TableEntry, 1>{
                //   {
                //     {"Mode", static_cast<float>(currentMode), "FromFlash"}
                //   }
                // }.data(), 1);
                  

                } 
                else if (currentMode == 1) {
                  gravityAlaramAt--;

                }

                    break;
                case BUTTON_EVENT_TOUCH_X:
                    touchStatus = "TOUCH_X";
                    Serial.println("Event: TOUCH_X");
                    break;
                case BUTTON_EVENT_TOUCH_RIGHT:
                touchStatus = "TOUCH_RIGHT";
                    Serial.println("Event: TOUCH_RIGHT");
                    if (currentMode == 2) {

                      deltaPressure++;  // Increase mode, max is 6
                      }
      
                      else if (currentMode == 4) {
                        
                      rollingPlotSwitch = (rollingPlotSwitch > 2) ?   1 : rollingPlotSwitch + 1;
                      Serial.printf("Event: TOUCH_RIGHT %d\n",rollingPlotSwitch );
                        
      
                      } 
                      else if (currentMode == 1) {
                        gravityAlaramAt++;

                      }


                    break;

                case BUTTON_EVENT_TOUCH_DOWN:
                    touchStatus = "TOUCH_DOWN";
                    if (currentMode > 0) {
                        currentMode--;  // Decrease mode, min is 0
                    }
                    Serial.printf("Event: TOUCH_DOWN -> Current Mode: %d\n", currentMode);
                    break;
                case BUTTON_EVENT_PUSH_15:
                    buttonStatus = "PUSH_BUTTON_15";
                    Serial.println("Event: PUSH_BUTTON_15");
                    break;
                case BUTTON_EVENT_PUSH_16:
                    buttonStatus = "PUSH_BUTTON_16";
                    Serial.println("Event: PUSH_BUTTON_16");
                    break;
            }


            //Serial.printf("LED toggled -> %s\n", ledState ? "ON" : "OFF");
        }
    }
}



/**
 * @brief FreeRTOS task that manages mode logic and triggers appropriate mode behavior.
 * 
 * Updates the display and calls the correct runModeX() function based on currentMode.
 * Runs every 100 ms.
 * 
 * @param pvParameters Unused.
 */



// ----------------- Mode Task -----------------
// // This task cycles the current mode between 0 and 6 every 100 ms.

void modeTask(void *pvParameters) {
  (void) pvParameters; // Unused parameter

    // Static variable to keep track of the previous mode.
    

    static float t = 0.0;
    float value = 0.0;

  while (1) {
  
    // Cycle mode: increment and wrap around from 6 back to 0.
    // currentMode = (currentMode + 1) % 7;
    //currentMode = 3;


    setDisplayMode(TABLE_MODE);
        // Update the display with the current mode.
        updateTableData(std::array<TableEntry, 1>{
          {
            {"Mode", static_cast<float>(currentMode), ""}
          }
        }.data(), 1);

    // Execute extra code based on the current mode.
    switch (currentMode) {
      case 0:
        // Only beep once when entering mode 1.
        if (previousMode != 0) {
        Serial.println("Case 0");

         Serial.println("Beeping 0 times...");
        //  buzzerAction(1);
        //  delay(1000); // Ensure beep is fully executed.
        previousMode = 0;
        }
        break;
      case 1:
        runMode1();
      break;

      case 2:
        runMode2();
      break;
      case 3: 
        runMode3();
      break;

      case 4: 
        runMode4();
      break;

      case 5:
        runMode5();
      break;
      default:
        break;
    }



    // Delay for 100 ms.
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


/**
 * @brief Arduino setup function.
 * 
 * Initializes all sensors, display, touch inputs, flash storage, and FreeRTOS tasks for telemetry,
 * input handling, mode management, and MQTT communication.
 */


void setup() {
  // Start serial communication for debugging.
  Serial.begin(115200);
  delay(1000); // Allow time for the serial monitor to initialize

  // Initialize the display module (also initializes the OLED).
  displayInit();
  
  // Start the FreeRTOS display update task in TABLE_MODE.
  startDisplayTask(TABLE_MODE);

  // Initialize voltage measurement pins and settings.
  initVoltageMeasurement();

  // Start the voltage measurement task.
  startVoltageMeasurementTask();
  delay(1000);
  
  // Display voltage measurement messages.
  updateTableData(std::array<TableEntry, 2>{
    {
      {"BattVolt", getVbatVoltage(), "V"},
      {"BusVolt",  getUsbVoltage(), "V"}
    }
  }.data(), 2);
  

  // ---------- IMU Initialization ----------
  initIMU();
  startIMUTask();
  setIMUEffectivePeriod(500);
  delay(3000);

    // Update the display with IMU sensor data.
  IMUEvents_t imuData  = getIMUData();

  updateTableData(std::array<TableEntry, 7>{
  {
    {"AccelX", imuData.accel.acceleration.x, "m/s^2"},
    {"AccelY", imuData.accel.acceleration.y, "m/s^2"},
    {"AccelZ", imuData.accel.acceleration.z, "m/s^2"},
    {"GyroX",  imuData.gyro.gyro.x, "rad/s"},
    {"GyroY",  imuData.gyro.gyro.y, "rad/s"},
    {"GyroZ",  imuData.gyro.gyro.z, "rad/s"},
    {"Temp",   imuData.temp.temperature, "°C"}
  }
}.data(), 7);



  // ---------- BME280 Initialization ----------
  initBME280();
  startBME280Task();
  delay(3000);
updateTableData(std::array<TableEntry, 3>{
  {
    {"Temp", getBMETemperature(), "°C"},
    {"Pres", getBMEPressure(), "hPa"},
    {"Humi", getBMEHumidity(), "%"}
  }
}.data(), 3);

initPressure = getBMEPressure();

delay(1000);


  // ---------- Touch Sensor Initialization ----------
  initTouchSensor();
  startTouchTask();

  updateTableData(std::array<TableEntry, 2>{
    {
      {"TouchButton", 4, "ready!"},
      {"PushButton", 2, "ready!"}
    }
  }.data(), 2);
  
  delay(2000);

  // ---------- Default Mode From Flash ----------
   // Initialize flash storage (SPIFFS)
   if (!initStorage()) {
    Serial.println("Storage initialization failed");
  }

  // Read default mode from flash; if not available, use 0.
  int storedDefault = readDefaultMode();
  if (storedDefault >= 0 && storedDefault <= 5) {
    defaultModeValue = storedDefault;
  } else {
    defaultModeValue = 0;
    writeDefaultMode(defaultModeValue);
  }

  currentMode = defaultModeValue;

  updateTableData(std::array<TableEntry, 1>{
    {
      {"Mode", static_cast<float>(currentMode), "FromFlash"}
    }
  }.data(), 1);
  delay(1000);

  // ---------- Buzzer & Led Initialization ----------
  // Initialize and trigger the buzzer.
  initBuzzer();

  ledController.begin(); // Initialize MCP23017

  updateTableData(std::array<TableEntry, 1>{
    {
      {"LED RED", 3, "Blinking"}
    }
  }.data(), 1);

  //buzzerAction(1, false, true);
  ledController.blinkLED(7, 4, 400);  // Red1: 3 times, 0.5s period
  ledController.blinkLED(10, 4, 400);  // Red1: 3 times, 0.5s period
  ledController.blinkLED(13, 4, 400);  // Red1: 3 times, 0.5s period
  

  // ---------- MQTT Initialization ----------
  initMqttTask();



  // Create the sensor task (runs every 1 second).
  xTaskCreate(sensorTask, "SensorTask", 4096, NULL, 1, NULL);


  // Create the FreeRTOS task to process inbound messages
  xTaskCreate(
    inboundTask,            // Task function.
    "InboundTask",          // Task name.
    4096,                   // Stack size in bytes.
    NULL,                   // Parameter.
    1,                      // Priority.
    NULL                    // Task handle.
  );

  // Create the mode task (runs every 100 ms).
  xTaskCreate(modeTask, "ModeTask", 4096, NULL, 1, NULL);

  
    // Create a task that receives events and toggles the MODE
    xTaskCreate(physicalInput, "PhysicalInput", 2048, NULL, 1, NULL);

}

/**
 * @brief Main loop function (empty).
 * 
 * Not used in this project since all logic is handled by FreeRTOS tasks.
 */

void loop() {
  // The loop is empty because tasks handle all the work.
}

// ////////////////////////////////////////////////////////////////////////////////////
