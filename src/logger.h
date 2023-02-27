/**
 * @file logger.h
 * 
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 * 
 * @brief
 * This is the header file for the uartlite to graphing functionality 
 * 
 * <pre>
 * MODIFICATION HISTORY:
 * ---------------------
 * Ver  Who Date    Changes
 * -----------------------------------
 * 1.00a SW 23-Feb-2023 First release
 * </pre>
************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include "xparameters.h"
#include "xstatus.h"
#include "xuartlite.h"
#include "xil_printf.h"

#define UARTLITE_DEVICE_ID	XPAR_UARTLITE_0_DEVICE_ID
#define BUFFER_SIZE 12

/* send data */
void send_data(); 

/* configure the uart_light*/
void uartlite_init(); 

#endif