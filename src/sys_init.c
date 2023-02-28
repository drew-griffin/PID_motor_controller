/**
 * @file sys_init.c
 * 
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 * 
 * @brief
 * This is the source file for initialization of the system for the PID_Controller
 * software.
 * 
 * <pre>
 * MODIFICATION HISTORY:
 * ---------------------
 * Ver  Who Date    Changes
 * -----------------------------------
 * 1.00a SW 23-Feb-2023 First release
 * </pre>
************************************************************/

#include <stdint.h>
#include "xparameters.h"
#include "xintc.h"
#include "sys_init.h"
#include "fit.h"
#include "cntrl_logic.h"
#include "logger.h"
#include "wdt.h"

/*********Peripheral Device Constants****************************/
//Definition for Interrupt Controller
#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID
#define INTC_BASEADDR           XPAR_INTC_0_BASEADDR

/********** AXI Peripheral Instances **********/
XIntc 		INTC_Inst;		// Interrupt Controller instance


/**
 * system_init() - Sets up platform and system state
 * 
 * @brief   Sets up system and platform peripherals and other state needed
 *  for running the program.
 * 
 * @return      returns status of initialization (success or failure)
*/
int system_init(void) {
    uint32_t status;

    // initialize hardware specific setups
    init_platform();

	// initialize uartlite
    uartlite_init(); 

    // init hardware peripherals
    // initialize the PMOD Encoder
    status = PMODENC544_initialize(PMODENC_BA);
    if (status != XST_SUCCESS){
    	return XST_FAILURE;
    }

	// initialize the PMOD HB3
	status = HB3_initialize(HB3_BA);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// initialize the Nexys4 driver
	status = NX4IO_initialize(N4IO_BASEADDR);
	if (status != XST_SUCCESS){
		return XST_FAILURE;
	}

	// initialize the WDT-time based system
	status = XWdtTb_Initialize(&WDTTB_Inst, WDT_DEVICE_ID);
	if (status != XST_SUCCESS)
	{
		xil_printf("Failed to initialize watchdog timer\r\n");
		return XST_FAILURE;
	}

	// self test since this is a new module for us
	status = XWdtTb_SelfTest(&WDTTB_Inst);
	if (status != XST_SUCCESS)
	{
		xil_printf("Failed watchdog timer test\r\n");
		return XST_FAILURE;
	}

	XWdtTb_Stop(&WDTTB_Inst); // suggested in the interrupt example

	// initialize the interrupt controller
	status = XIntc_Initialize(&INTC_Inst, INTC_DEVICE_ID);
	if (status != XST_SUCCESS)
	{
	   return XST_FAILURE;
	}

    // connect the interrupt handlers to the interrupts
	status = XIntc_Connect(&INTC_Inst, FIT_INTR_NUM,
						   (XInterruptHandler)FIT_Handler,
						   (void *)0);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// connect the interrupt handler for the WDT
	status = XIntc_Connect(&INTC_Inst, WDT_INTR_NUM,
							(XInterruptHandler)WDTHandler,
							(void *)0);
	if (status != XST_SUCCESS)
	{
		xil_printf("WDT handler didn't register\r\n");
		return XST_FAILURE;
	}

    // start the interrupt controller such that interrupts are enabled for
	// all devices that cause interrupts.
	status = XIntc_Start(&INTC_Inst, XIN_REAL_MODE);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

    // enable/disable the interrupts
	XIntc_Enable(&INTC_Inst, FIT_INTR_NUM);
	XIntc_Enable(&INTC_Inst, WDT_INTR_NUM);

	XWdtTb_Start(&WDTTB_Inst); // restart the timer for the watchdog timer
	return XST_SUCCESS;
}
