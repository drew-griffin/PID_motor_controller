/**
 * @file fit.c
 * 
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 * 
 * @brief
 * This is the source file for FIT interrupt handler
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
#include <stdint.h>
#include "fit.h"

#define HEART_BEAT      0x00008000

/**
 * FIT_Handler() - Fixed Interval interrupt handler
 * 
 * Blinks LED[15] every 0.25 seconds, as proof to the systems operation.
 * Will stop if watchdog timer fails
 * 
 * @note    Registered in sys_init.c
 * 
*/
void FIT_Handler(void) {
    static bool isInitialized = false; // starts up led heartbeat
    static bool dpOn;

    if (!isInitialized) {
        dpOn = true;
        isInitialized = true;
    }

    dpOn = (dpOn) ? false : true;
    NX4IO_SSEG_setDecPt (SSEGHI, DIGIT7, dpOn);
}
