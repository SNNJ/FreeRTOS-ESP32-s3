
#include "display.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <freertos/semphr.h>
#include <math.h>  // for sin, cos functions

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Create the OLED display object.
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// -----------------------
// TABLE_MODE variables
// -----------------------
static const int MAX_ENTRIES = 10;
static TableEntry tableData[MAX_ENTRIES];
static int currentNumEntries = 0;

// -------------------------------
// ARTIFICIAL_HORIZON_MODE variables
// -------------------------------
static float currentPitch = 0.0; // in degrees
static float currentRoll = 0.0;  // in degrees

// -----------------------
// ROLLING_PLOT_MODE variables
// -----------------------
#define MAX_PLOT_POINTS 128
static float rollingPlotData[MAX_PLOT_POINTS];
static float rollingPlotTime[MAX_PLOT_POINTS];
static int rollingPlotCount = 0;
static String rollingPlotXLabel = "";
static String rollingPlotYLabel = "";

// Current display mode (default to TABLE_MODE).
static DisplayMode currentMode = TABLE_MODE;

// FreeRTOS task handle for the display task.
static TaskHandle_t displayTaskHandle = NULL;

// Binary semaphore to signal a new data update.
static SemaphoreHandle_t displayUpdateSemaphore = NULL;

// Forward declarations for internal drawing functions.
static void drawTable();
static void drawHorizon();
static void drawRollingPlot();

// FreeRTOS task: waits for a new update signal then refreshes the display.
static void displayTask(void* parameter) {
  (void)parameter; // Unused parameter
  for (;;) {
    if (xSemaphoreTake(displayUpdateSemaphore, portMAX_DELAY) == pdTRUE) {
      switch (currentMode) {
        case TABLE_MODE:
          drawTable();
          break;
        case ARTIFICIAL_HORIZON_MODE:
          drawHorizon();
          break;
        case ROLLING_PLOT_MODE:
          drawRollingPlot();
          break;
      }
    }
  }
}

//----------------------------
// Drawing functions
//----------------------------

// TABLE_MODE drawing.
static void drawTable() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  int y = 0;
  const int lineHeight = 10;  // Adjust as needed
  for (int i = 0; i < currentNumEntries; i++) {
    String line = tableData[i].name + ": " +
                  String(tableData[i].value, 1) + " " +
                  tableData[i].unit;
    display.setCursor(0, y);
    display.println(line);
    y += lineHeight;
  }
  display.display();
}

// ARTIFICIAL_HORIZON_MODE drawing.
static void drawHorizon() {
  display.clearDisplay();
  
  const float maxPitch = 45.0; // Maximum expected pitch (degrees)
  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2;
  
  // Compute vertical offset from pitch.
  int offset = (int)((currentPitch / maxPitch) * (SCREEN_HEIGHT / 2));
  int horizonY = centerY + offset;
  
  // Define unrotated endpoints for the horizon line.
  int xLeft = 0;
  int yLeft = horizonY;
  int xRight = SCREEN_WIDTH - 1;
  int yRight = horizonY;
  
  // Convert roll angle to radians.
  float rollRad = currentRoll * DEG_TO_RAD;
  
  // Rotate endpoints about the display center.
  int x1 = centerX + (xLeft - centerX) * cos(rollRad) - (yLeft - centerY) * sin(rollRad);
  int y1 = centerY + (xLeft - centerX) * sin(rollRad) + (yLeft - centerY) * cos(rollRad);
  int x2 = centerX + (xRight - centerX) * cos(rollRad) - (yRight - centerY) * sin(rollRad);
  int y2 = centerY + (xRight - centerX) * sin(rollRad) + (yRight - centerY) * cos(rollRad);
  
  // Draw the rotated horizon line.
  display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  
  // Draw a center marker.
  display.drawCircle(centerX, centerY, 2, SSD1306_WHITE);
  
  display.display();
}

// ROLLING_PLOT_MODE drawing.
// The plot area is defined with small margins, and the x and y axis labels are drawn inside the chart.
// Three tick marks (at 1/3, 2/3, and 3/3 of the range) are drawn for both axes,
// with tick labels showing one decimal digit.
static void drawRollingPlot() {
  display.clearDisplay();

  // Define chart area margins.
  const int marginLeft = 10;
  const int marginRight = 10;
  const int marginTop = 10;
  const int marginBottom = 12;
  
  int plotX = marginLeft;
  int plotY = marginTop;
  int plotWidth = SCREEN_WIDTH - marginLeft - marginRight;
  int plotHeight = SCREEN_HEIGHT - marginTop - marginBottom;

  // Draw the plot border (axes).
  display.drawRect(plotX, plotY, plotWidth, plotHeight, SSD1306_WHITE);

  // If no data available, show a message.
  if (rollingPlotCount == 0) {
    display.setCursor(0, 0);
    display.println("No data");
    display.display();
    return;
  }
  
  // Compute min and max for the y axis (data) and x axis (time).
  float minVal = rollingPlotData[0], maxVal = rollingPlotData[0];
  float minTime = rollingPlotTime[0], maxTime = rollingPlotTime[0];
  for (int i = 1; i < rollingPlotCount; i++) {
    if (rollingPlotData[i] < minVal) minVal = rollingPlotData[i];
    if (rollingPlotData[i] > maxVal) maxVal = rollingPlotData[i];
    if (rollingPlotTime[i] < minTime) minTime = rollingPlotTime[i];
    if (rollingPlotTime[i] > maxTime) maxTime = rollingPlotTime[i];
  }
  if (maxVal == minVal) { maxVal = minVal + 1.0; }
  if (maxTime == minTime) { maxTime = minTime + 1.0; }
  
  // Draw tick marks and labels (three ticks each axis).
  // Y axis ticks.
  for (int t = 1; t <= 2; t++) {
    float tickVal = minVal + (maxVal - minVal) * t / 3.0;
    int yTick = plotY + plotHeight - (int)(((tickVal - minVal) / (maxVal - minVal)) * plotHeight);
    // Draw a small tick on the left border.
    display.drawLine(plotX - 3, yTick, plotX, yTick, SSD1306_WHITE);
    // Draw the tick label (one decimal digit).
    String tickLabel = String(tickVal, 1);
    display.setCursor(0, yTick - 3);
    display.print(tickLabel);
  }
  // X axis ticks.
  for (int t = 0; t <= 3; t++) {
    float tickTime = minTime + (maxTime - minTime) * t / 3.0;
    int xTick = plotX + (int)(((tickTime - minTime) / (maxTime - minTime)) * plotWidth);
    // Draw a small tick on the bottom border.
    display.drawLine(xTick, plotY + plotHeight, xTick, plotY + plotHeight + 3, SSD1306_WHITE);
    // Draw the tick label (one decimal digit).
    String tickLabel = String(tickTime, 0);
    // Center the tick label below the tick.
    int16_t xOffset = tickLabel.length() * 6 / 2;
    display.setCursor(xTick - xOffset, plotY + plotHeight + 4);
    display.print(tickLabel);
  }
  
  // Draw the rolling plot line.
  for (int i = 1; i < rollingPlotCount; i++) {
    // Map time to x coordinate.
    int x1 = plotX + (int)(((rollingPlotTime[i - 1] - minTime) / (maxTime - minTime)) * plotWidth);
    int x2 = plotX + (int)(((rollingPlotTime[i] - minTime) / (maxTime - minTime)) * plotWidth);
    // Map value to y coordinate (inverted so higher values appear at the top).
    int y1 = plotY + plotHeight - (int)(((rollingPlotData[i - 1] - minVal) / (maxVal - minVal)) * plotHeight);
    int y2 = plotY + plotHeight - (int)(((rollingPlotData[i] - minVal) / (maxVal - minVal)) * plotHeight);
    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }
  
  // Draw in-chart axis labels.
  // Y axis label: top-left corner of plot area.
  display.setCursor(plotX + 8, plotY + 2);
  display.print(rollingPlotYLabel);
  // X axis label: bottom-right corner inside plot area.
  int xLabelWidth = rollingPlotXLabel.length() * 6; // approximate width at text size 1
  display.setCursor(plotX + plotWidth - xLabelWidth - 2, plotY + plotHeight - 8);
  display.print(rollingPlotXLabel);
  
  display.display();
}

//----------------------------
// Public functions
//----------------------------

void displayInit() {
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (true); // Halt if initialization fails.
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Display Init OK");
  display.display();
  delay(1000);
}

void startDisplayTask(DisplayMode mode) {
  currentMode = mode;
  if (displayUpdateSemaphore == NULL) {
    displayUpdateSemaphore = xSemaphoreCreateBinary();
  }
  if (displayTaskHandle == NULL) {
    xTaskCreatePinnedToCore(
      displayTask,         // Task function.
      "DisplayTask",       // Task name.
      4096,                // Stack size.
      NULL,                // Parameters.
      1,                   // Priority.
      &displayTaskHandle,  // Task handle.
      1                    // Core ID.
    );
  }
}

void setDisplayMode(DisplayMode mode) {
  currentMode = mode;
}

// Update table data for TABLE_MODE.
void updateTableData(const TableEntry* newData, int count) {
  if (count > MAX_ENTRIES) count = MAX_ENTRIES;
  for (int i = 0; i < count; i++) {
    tableData[i] = newData[i];
  }
  currentNumEntries = count;
  xSemaphoreGive(displayUpdateSemaphore);
}

// Update horizon data for ARTIFICIAL_HORIZON_MODE.
void updateHorizonData(float pitch, float roll) {
  currentPitch = pitch;
  currentRoll = roll;
  xSemaphoreGive(displayUpdateSemaphore);
}

// Update rolling plot data for ROLLING_PLOT_MODE.
// Accepts a new data point and its associated time stamp, plus axis labels.
// The new point is appended to the rolling buffer (shifting older points if necessary).
void updateRollingPlotData(float newValue, float newTime, const char* xLabel, const char* yLabel) {
  rollingPlotXLabel = String(xLabel);
  rollingPlotYLabel = String(yLabel);
  
  if (rollingPlotCount < MAX_PLOT_POINTS) {
    rollingPlotData[rollingPlotCount] = newValue;
    rollingPlotTime[rollingPlotCount] = newTime;
    rollingPlotCount++;
  } else {
    // Shift left if buffer is full.
    for (int i = 0; i < MAX_PLOT_POINTS - 1; i++) {
      rollingPlotData[i] = rollingPlotData[i + 1];
      rollingPlotTime[i] = rollingPlotTime[i + 1];
    }
    rollingPlotData[MAX_PLOT_POINTS - 1] = newValue;
    rollingPlotTime[MAX_PLOT_POINTS - 1] = newTime;
  }
  xSemaphoreGive(displayUpdateSemaphore);
}


// // This resets the data so that a completely new set of values can be plotted.
// void resetRollingPlotData() {
//   rollingPlotCount = 0;
//   // Optionally clear the arrays (not strictly necessary)
//   for (int i = 0; i < MAX_PLOT_POINTS; i++) {
//     rollingPlotData[i] = 0;
//     rollingPlotTime[i] = 0;
//   }
//   // Optionally reset axis labels.
//   rollingPlotXLabel = "";
//   rollingPlotYLabel = "";
  
//   // Force an immediate display update.
//   xSemaphoreGive(displayUpdateSemaphore);
// }

///////////////////////////////////////////////////////////////////
