/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xstatus.h"
#include "microblaze_sleep.h"

//#include "xtmrctr.h" //AXI Timer header file?
#include "xintc.h"
#include "nexys4IO.h"
#include "xgpio.h"
#include "PWM_Analyzer.h"

/*********** Peripheral-related constants **********/
// Clock frequencies
#define CPU_CLOCK_FREQ_HZ		XPAR_CPU_CORE_CLOCK_FREQ_HZ
#define AXI_CLOCK_FREQ_HZ		XPAR_CPU_M_AXI_DP_FREQ_HZ

// Definitions for peripheral NEXYS4IO
#define N4IO_DEVICE_ID		    XPAR_NEXYS4IO_0_DEVICE_ID
#define N4IO_BASEADDR		    XPAR_NEXYS4IO_0_S00_AXI_BASEADDR
#define N4IO_HIGHADDR		    XPAR_NEXYS4IO_0_S00_AXI_HIGHADDR

// Definitions for Fixed Interval timer - 100 MHz input clock, 2Hz output clock
// Used as interval to handle the slide switches and pushbuttons
#define FIT_IN_CLOCK_FREQ_HZ	XPAR_CPU_CORE_CLOCK_FREQ_HZ
#define FIT_CLOCK_FREQ_HZ		2
#define FIT_INTR_NUM			XPAR_MICROBLAZE_0_AXI_INTC_FIT_TIMER_0_INTERRUPT_INTR

// Definitions for Interrupt Controller
#define INTC_DEVICE_ID			XPAR_INTC_0_DEVICE_ID
#define INTC_BASEADDR			XPAR_INTC_0_BASEADDR
#define INTC_HIGHADDR			XPAR_INTC_0_HIGHADDR

// Definitions for GPIO output
#define GPIO_REG_BASE_ADDRESS	XPAR_AXI_GPIO_0_BASEADDR
#define GPIO_DEVICE_ID			XPAR_AXI_GPIO_0_DEVICE_ID

/********** Function Prototypes **********/
int	 do_init(void);
u32 buildPWMCtrlReg(bool enable, u16 RedDC, u16 GreenDC, u16 BlueDC);

/********** Global Variables **********/
bool pwmEnable = true;								// true to enable PWM output

/********** AXI Peripheral Instances **********/
XGpio 	Gpio;
int main()
{
	u32 PWMCntrlReg;
    init_platform();
    uint32_t sts = do_init();
    if (XST_SUCCESS != sts){
    	xil_printf("FATAL(main): System initialization failed\r\n");
    	return 1;
    }

    while(1)
    {
    	// build control register for 50% DC to motor
    	//PWMCntrlReg = buildPWMCtrlReg(pwmEnable, 511, 0, 0);
    	//XGpio_DiscreteWrite(&Gpio, 1, PWMCntrlReg);
    	xil_printf("50% Duty Cycle\n\r");
        usleep(2000 * 1000);
        // build control register for 50% DC to motor
        PWMCntrlReg = buildPWMCtrlReg(pwmEnable, 1, 0, 0);
        XGpio_DiscreteWrite(&Gpio, 1, PWMCntrlReg);
        xil_printf("0% Duty Cycle\n\r");
        usleep(2000 * 1000);
    }

    cleanup_platform();
    return 0;
}
/**
 * do_init() - initializes the embedded system peripherals
 *
 * @brief This function initializes the AXI peripherals and sets up for interrupt handling.  Interrupts
 * are enabled/disabled in main() or other functions.  When this function exits the peripherals have
 * been initialized but interrupts are left disabled
 *
 * @return			XST_Success if the timer is initialized.  XST_FAILURE if it is not
 *
 * @note:  Digilent PWM Analyzer is a low level driver and does not need to be initialized
 */
int	 do_init(void) {
	uint32_t status;				// status from Xilinx Lib calls

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
	return XST_SUCCESS;
}

/**
 * buildPWMCtrlReg() - returns a PWM ControlReg value
 *
 * @brief combines the enable and PWM duty cycle bits to create
 * a value that can be loaded into the PWM Control register
 *
 * @param enable	PWM enable.  True to enable the PWM outputs
 * @param RedDc		Duty cycle for RED pwm.  Range is 0..1023
 * @param GreenDc	Duty cycle for GREEN pwm.  Range is 0..1023
 * @param Blue		Duty cycle for BLUE pwm.  Range is 0..1023
 *
 * @return			A PWM Control register value
 */

u32 buildPWMCtrlReg(bool enable, u16 RedDC, u16 GreenDC, u16 BlueDC) {
 	u32 cntlreg;

 	// initialize the value depending on whether PWM is enabled
 	// enable is Control register[31]
 	cntlreg = (enable) ? 0x80000000 : 0x0000000;

 	// add the duty cycles
 	cntlreg |=	((BlueDC & 0x03FF)  << 0)  |
 				((GreenDC & 0x03FF) << 10) |
 				((RedDC & 0x03FF)   << 20);
 	return cntlreg;
 }
