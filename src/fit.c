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
#include "user_input.h"

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
    static uint32_t led_heartbeat;

    if (!isInitialized) {
        led_heartbeat = HEART_BEAT;
        isInitialized = true;
    }

    NX4IO_setLEDs(led_heartbeat);

    led_heartbeat ^= HEART_BEAT;
}
