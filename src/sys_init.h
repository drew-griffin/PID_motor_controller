/**
 * @file sys_init.h
 * 
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 * 
 * @brief
 * This is the header file for initilization of the system for the PID_Controller
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

#ifndef SYS_INIT_H
#define SYS_INIT_H

#include "xstatus.h"
#include "platform.h"

/**
 * system_init() - sets up the system startup code
 * 
 * @return if system initialization was sucessfull
*/
int system_init(void);

#endif
