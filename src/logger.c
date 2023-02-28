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
uint8_t ControlBuffer[CONTROL_BUFFER_SIZE] = {'D', 'B', ' '};
uint8_t DataBuffer[DATA_BUFFER_SIZE];	/* Buffer for Transmitting Data */

/**
 * @function send_data
 * @brief sends data to console using
 * polled uartlite methodology 
 */
void send_data(uint8_t set_rpm, uint8_t read_rpm, uint8_t Kp, uint8_t Ki, uint8_t Kd)
{
    DataBuffer[0] =  set_rpm/10 % 10 + '0';
    DataBuffer[1] =  set_rpm % 10 + '0';
    DataBuffer[2] =  ' '; 
    DataBuffer[3] =  read_rpm/10 % 10 + '0';
    DataBuffer[4] =  read_rpm % 10 + '0';
    DataBuffer[5] =  ' '; 
    DataBuffer[6] =  Kp/10 % 10 + '0';
    DataBuffer[7] =  Kp % 10 + '0';
    DataBuffer[8] =  ' '; 
    DataBuffer[9] =  Ki/10 % 10 + '0';
    DataBuffer[10] = Ki % 10 + '0';
    DataBuffer[11] = ' '; 
    DataBuffer[12] = Kd/10 % 10 + '0';
    DataBuffer[13] = Kd % 10 + '0';
    DataBuffer[14] = '\n'; 
   


    uint32_t numSent;
    numSent = XUartLite_Send(&UartLite, ControlBuffer, CONTROL_BUFFER_SIZE); 
    if (numSent != CONTROL_BUFFER_SIZE)
    {
        return XST_FAILURE; 
    }
    
    //block until first message is sent 
    while (XUartLite_IsSending(&UartLite)){};

    numSent = XUartLite_Send(&UartLite, DataBuffer, DATA_BUFFER_SIZE); 
    if (numSent != DATA_BUFFER_SIZE)
    {
        return XST_FAILURE; 
    }
	/* Could use this instead */
    //xil_printf("DB %d %d %d %d %d\n\r", set_rpm, read_rpm, Kp, Ki, Kd);
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
