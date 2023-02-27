/**
 * @file wdt.h
 * 
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 * 
 * @brief
 * This is the header file for the watchdog timer functionality
 * 
 * <pre>
 * MODIFICATION HISTORY:
 * ---------------------
 * Ver  Who Date    Changes
 * -----------------------------------
 * 1.00a SW 23-Feb-2023 First release
 * </pre>
************************************************************/

#include <stdbool.h>
#include "wdt.h"
#include "nexys4io.h"
#include "cntrl_logic.h" // need this for encoder switch value in handler

/*************Local File Defines*********************/
#define WDT_LED         0x00008000

void WDTHandler(void) {
    static bool isInitialized = false;

    if(!isInitialized) {
        isInitialized = true;
        NX4IO_setLEDs(WDT_LED);
    }

    if(!wdt_crash) {
        XWdtTb_RestartWdt(&WDTTB_Inst);
    }
}
