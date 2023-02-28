/**
 * @file cntrl_logic.h
 * 
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 * 
 * @brief
 * This is the header file for control functionality
 * 
 * <pre>
 * MODIFICATION HISTORY:
 * ---------------------
 * Ver  Who Date    Changes
 * -----------------------------------
 * 1.00a SW 23-Feb-2023 First release
 * </pre>
************************************************************/

#ifndef CNTRL_LOGIC_H
#define CNTRL_LOGIC_H

#include <stdint.h>
#include <stdbool.h>
#include "nexys4io.h"
#include "xparameters.h"
#include "PmodENC544.h"
#include "myHB3IP.h"

/*********Peripheral Device Constants****************************/
// Definitions for peripheral NEXYS4IO
#define N4IO_DEVICE_ID		    XPAR_NEXYS4IO_0_DEVICE_ID
#define N4IO_BASEADDR		    XPAR_NEXYS4IO_0_S00_AXI_BASEADDR
// Definitions for PMOD Encoder
#define 	PMODENC_ID 	XPAR_PMODENC544_0_DEVICE_ID
#define 	PMODENC_BA	XPAR_PMODENC544_0_S00_AXI_BASEADDR
// Definitions for PMOD HB3
#define HB3_BA 					XPAR_MYHB3IP_0_S00_AXI_BASEADDR

// PWM Analyzer HW is currently in design and was used in testing, but can be removed for final submission
#include "PWM_Analyzer.h"
#define AXI_CLOCK_FREQ_HZ		XPAR_CPU_M_AXI_DP_FREQ_HZ
// Definitions for PWM_Analyzer
#define PWM_BASEADDR 			XPAR_PWM_ANALYZER_0_S00_AXI_BASEADDR


/*********Control Structs****************************/
typedef struct user_io {
    uint8_t button_state;
    uint16_t switch_state;
    uint8_t rotary_count;
    uint8_t enc_BtnSw_state;
    bool has_changed;
} user_io_t, *ptr_user_io_t;

/***********Shared Global Variables******************/
bool wdt_crash; // used for the wdt

/**
 * read_user_IO() - reads and returns user IO data
 * 
 * @param       pointer to a user IO struct
 * 
 * @return      updates user IO struct with current data used for updating display values
*/
void read_user_IO(ptr_user_io_t uIO);

/**
 * init_IO_struct() - setus up IO struct for user
 * 
 * @param       pointer to a user IO struct
 * 
 * @returns     initialized user IO struct
*/
void init_IO_struct(ptr_user_io_t uIO);

/**
 * display() - displays set mode or run mode for user
*/
void display(void);

/**
 * update_pid() - function loop that updates info from
 * incoming user interfaces of the switches, push buttons,
 * and encoder
 * 
 * @param       pointer to a user IO struct
*/
void update_pid(ptr_user_io_t uIO);

/**
 * send_uartlite_data
 * @brief sends uartlite data every second
 * if BtnL has been changed to true 
 */
void send_uartlite_data(); 


#endif
