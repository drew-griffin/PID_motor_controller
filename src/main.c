/******************************************************************************
 * @file main.c
 *
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 *
 * @brief
 * This is the main loop that initializes the system and then loops over control
 * and display
 *
 * <pre>
 * MODIFICATION HISTORY:
 * ---------------------
 * Ver  Who Date    Changes
 * -----------------------------------
 * 1.00a SW 23-Feb-2023 First release
 * </pre>
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
#include "logger.h"
#include "wdt.h"


/*****************PID Control Instances*****************/
static ptr_user_io_t uIO;


int main()
{
    xil_printf("PID Motor Controller System Starting\r\n\n");

    uint32_t sts = system_init();
    if (XST_SUCCESS != sts) {
        xil_printf("FATAL(main): System initialization failed\r\n");
        return 1;
    }
  

    microblaze_enable_interrupts();
    init_IO_struct(uIO);
    NX4IO_setLEDs(0x00000000); // clear LEDs, odd behavior where they turn on
    PMODENC544_clearRotaryCount(); // set rotary count to 0
    while(1)
    {
        read_user_IO(uIO);
        update_pid(uIO);
        display();
        send_uartlite_data();
        if(XWdtTb_IsWdtExpired(&WDTTB_Inst)) {
            while(1) {
                NX4IO_setLEDs(0x0000FFFF);
                NX410_SSEG_setAllDigits(SSEGHI, CC_BLANK, CC_B, CC_LCY, CC_E, DP_NONE);
	            NX410_SSEG_setAllDigits(SSEGLO, CC_B, CC_LCY, CC_E, CC_BLANK, DP_NONE);
            }
        } 
        //usleep(1000 * 1000); 
    }
    
    microblaze_disable_interrupts();
    cleanup_platform();
    return 0;
}

