/**
 * @file wdt.h
 * 
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 * 
 * @brief
 * This is the header file for the watchdog timer functionality
 * 
 * <pre>
 * MODIFICATION HISTORY:
 * ---------------------
 * Ver  Who Date    Changes
 * -----------------------------------
 * 1.00a SW 23-Feb-2023 First release
 * </pre>
************************************************************/

#include "xparameters.h"
#include "xwdttb.h"

/*********Peripheral Device Constants****************************/
#define WDT_INTR_NUM        XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMEBASE_WDT_0_WDT_INTERRUPT_INTR
#define WDT_DEVICE_ID       XPAR_AXI_TIMEBASE_WDT_0_DEVICE_ID

/********** AXI Peripheral Instances **********/
XWdtTb WDTTB_Inst;

void WDTHandler(void);
