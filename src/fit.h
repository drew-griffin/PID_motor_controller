/**
 * @file fit.h
 * 
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 * 
 * @brief
 * This is the header file for FIT interrupt handler
 * 
 * <pre>
 * MODIFICATION HISTORY:
 * ---------------------
 * Ver  Who Date    Changes
 * -----------------------------------
 * 1.00a SW 23-Feb-2023 First release
 * </pre>
************************************************************/

#ifndef FIT_H
#define FIT_H

#include "xparameters.h"
#include "cntrl_logic.h"

/*********Peripheral Device Constants****************************/
// Definitions for Fixed Interval timer - 100 MHz input clock, 4Hz output clock
// Used as interval to handle the slide switches and pushbuttons
#define FIT_IN_CLOCK_FREQ_HZ	XPAR_CPU_CORE_CLOCK_FREQ_HZ
#define FIT_CLOCK_FREQ_HZ		4
#define FIT_INTR_NUM			XPAR_MICROBLAZE_0_AXI_INTC_FIT_TIMER_0_INTERRUPT_INTR

uint8_t second_counter;

/**
 * FIT_Handler() - blinks heartbeat LED every 0.25 seconds
 * 
 * @note: tied to INTC in sys_init.c
*/
void FIT_Handler(void);

#endif
