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

	// initialize the GPIO driver
	status = XGpio_Initialize(&Gpio, GPIO_DEVICE_ID);
	if (status != XST_SUCCESS)
		return XST_FAILURE;
	// sets channel 1, direction for all 32 bits is output hence the 0s
	XGpio_SetDataDirection(&Gpio, 1, 0x00000000);

	// initialize the Nexys4 driver
	status = NX4IO_initialize(N4IO_BASEADDR);
	if (status != XST_SUCCESS){
		return XST_FAILURE;
	}

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

    // start the interrupt controller such that interrupts are enabled for
	// all devices that cause interrupts.
	status = XIntc_Start(&INTC_Inst, XIN_REAL_MODE);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

    // enable/disable the interrupts
	XIntc_Enable(&INTC_Inst, FIT_INTR_NUM);
	return XST_SUCCESS;
}
