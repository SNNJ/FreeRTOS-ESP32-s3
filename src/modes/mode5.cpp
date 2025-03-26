// mode5.cpp
#include "modes/modegeneral.h"
#include "modes/mode5.h"

void runMode5() {
    // Static variable to ensure the beep is triggered only once upon entering mode 5.
    
    if (previousMode != 5) {
        Serial.println("Beeping 5 times...");
        buzzerAction(5);
        previousMode = 5;
    }
    
    // Set the display mode to TABLE_MODE.
    setDisplayMode(TABLE_MODE);
    
    // If tc is not empty, update the display with telemetry data.
    if (tc != "") {
        updateTableData(std::array<TableEntry, 1>{
            {
                {tc, (float)tcValue, tcHash}
            }
        }.data(), 1);
    }
}
