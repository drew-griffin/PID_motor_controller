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

/**
 * WDTHandler() - turns on LED15 and restarts WDT
 * unless kill switch is flipped
 * 
 * @brief       Starts up with LED15 lit and kicks the
 *              dog every time through unless there is
 *              a kill switched that is flipped.
 * 
 * @note:       tied to INTC in sys_init.c
*/
void WDTHandler(void) {
    static bool isInitialized = false;

    if(!isInitialized) {
        isInitialized = true;
        uint16_t prv_led = NX4IO_getLEDS_DATA();
        NX4IO_setLEDs(WDT_LED | prv_led);
    }

    if(!wdt_crash) {
        XWdtTb_RestartWdt(&WDTTB_Inst);
    }

    if(XWdtTb_IsWdtExpired(&WDTTB_Inst)){
            NX4IO_setLEDs(0x0000FFFF);
            NX410_SSEG_setAllDigits(SSEGHI, CC_BLANK, CC_B, CC_LCY, CC_E, DP_NONE);
            NX410_SSEG_setAllDigits(SSEGLO, CC_B, CC_LCY, CC_E, CC_BLANK, DP_NONE);
            HB3_setPWM(true, 1); //turn off motor
    }
}
