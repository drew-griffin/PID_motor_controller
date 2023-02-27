/**
 * @file logger.c
 * 
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 * 
 * @brief
 * This is the source file for the uartlite to graphing functionality 
 * 
 * <pre>
 * MODIFICATION HISTORY:
 * ---------------------
 * Ver  Who Date    Changes
 * -----------------------------------
 * 1.00a SW 23-Feb-2023 First release
 * </pre>
************************************************************/
#include "logger.h"

XUartLite UartLite;		/* Instance of the UartLite Device */
uint8_t SendBuffer[BUFFER_SIZE];	/* Buffer for Transmitting Data */

/**
 * @function send_data
 * @brief sends data to console using
 * polled uartlite methodology 
 */
void send_data()
{

    /* TEST CONSTANT VALUE TO BE CHANGED*/
    uint8_t   Kp = (uint8_t)(5 * 10), Ki = (uint8_t)(3.2 * 10), Kd = (uint8_t)(4.7 * 10);
    uint8_t   set_rpm = 55, read_rpm = 47;

    /*
    SendBuffer = {'D', 'B', ' ', 
                    rpm, read_rpm, 
                    (Kp >> 8), (Kp && 0xFF), ' ',
                    (Ki >> 8), (Kp && 0xFF), ' ',
                    (Kd >> 8), (Kd && 0xFF), '\n'};
    */
   /* To do - improve */
    /*
   SendBuffer[0] = 'D'; SendBuffer[1] = 'B';  SendBuffer[2] = ' '; 
   SendBuffer[3] = rpm; SendBuffer[4] = ' ';  SendBuffer[5] = read_rpm;
   SendBuffer[6] = ' '; SendBuffer[7] = Kp;   SendBuffer[8] = ' ';
   SendBuffer[9] = Ki;  SendBuffer[10] = Kd;  SendBuffer[11] = '\n';


    uint32_t numSent;
    numSent = XUartLite_Send(&UartLite, SendBuffer, BUFFER_SIZE); 
    if (numSent != BUFFER_SIZE)
    {
        return XST_FAILURE; 
    }
	*/

    xil_printf("DB %d %d %d %d %d\n\r", set_rpm, read_rpm, Kp, Ki, Kd);
}

/**
 * @function uartlite_init
 * 
 * @brief function initializes initializes the uartlite instance
 * 
 */
void uartlite_init()
{
    uint32_t Status;

    Status = XUartLite_Initialize(&UartLite, UARTLITE_DEVICE_ID);
	if (Status != XST_SUCCESS) 
    {
		return XST_FAILURE;
	}

    Status = XUartLite_SelfTest(&UartLite);
	if (Status != XST_SUCCESS) 
    {
		return XST_FAILURE;
	}

}
