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

#include <stdlib.h>
#include <stdio.h>
#include "xil_printf.h"
#include "microblaze_sleep.h"
#include "sys_init.h"
#include "cntrl_logic.h"


/*****************PID Control Instances*****************/
static ptr_user_io_t uIO;

int main()
{
    /* To do tie this to gpio_pwm signal in axi_gpio_0 block */
    /* In HDL tie gpio_pwm to control_Reg_0 in instantiation */
    /* This will testing sending output */
    /* Be sure to have RedDC a test value, as this drives EN pin*/
    xil_printf("PID Motor Controller System Starting\r\n\n");

    uint32_t sts = system_init();
    if (XST_SUCCESS != sts) {
        xil_printf("FATAL(main): System initialization failed\r\n");
        return 1;
    }
    init_IO_struct(uIO);
    

    microblaze_enable_interrupts();
    NX4IO_setLEDs(0x00000000); // clear LEDs, odd behavior where they turn on
    while(1)
    {
        read_user_IO(uIO);
        update_pid(uIO);
        display();
    }
    
    microblaze_disable_interrupts();
    cleanup_platform();
    return 0;
}


/**
 * buildPWMCtrlReg() - returns a PWM ControlReg value
 *
 * @brief combines the enable and PWM duty cycle bits to create
 * a value that can be loaded into the PWM Control register
 * This control register will write RGB_1 in this application.
 *
 * @param enable	PWM enable.  True to enable the PWM outputs
 * @param RedDc		Duty cycle for RED pwm.  Range is 0..1023
 * @param GreenDc	Duty cycle for GREEN pwm.  Range is 0..1023
 * @param Blue		Duty cycle for BLUE pwm.  Range is 0..1023
 *
 * @return			A PWM Control register value
 */
/*
u32 buildPWMCtrlReg(bool enable, u16 RedDC, u16 GreenDC, u16 BlueDC)
{
	u32 cntlreg;

	// initialize the value depending on whether PWM is enabled
	// enable is Control register[31]
	cntlreg = (enable) ? 0x80000000 : 0x0000000;

	// add the duty cycles
	cntlreg |= ((BlueDC & 0x03FF) << 0) |
			   ((GreenDC & 0x03FF) << 10) |
			   ((RedDC & 0x03FF) << 20);
	return cntlreg;
}
*/
