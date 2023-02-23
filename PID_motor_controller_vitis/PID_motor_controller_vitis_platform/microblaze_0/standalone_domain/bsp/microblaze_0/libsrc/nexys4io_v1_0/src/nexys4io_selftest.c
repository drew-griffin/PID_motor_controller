/**
*
* @file nexys4IO_selftest.c
*
* @author Roy Kravitz (roy.kravitz@pdx.edu)
* @copyright Portland State University, 2014-2017, 2018
*
* This header file contains a self test for the Nexys4IO peripheral.  The
* test is rudimentary, only checking that a few of the peripheral registers
* can be written and read back.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a	rhk	12/20/14	First release of driver
* 1.01a rhk 01/10/18	updates for SDK 2017.3
* </pre>
*
******************************************************************************/
/***************************** Include Files *******************************/
#include "nexys4IO.h"
#include "xparameters.h"
#include "stdio.h"
#include "xil_io.h"

/************************** Constant Definitions ***************************/
#define READ_WRITE_MUL_FACTOR 0x10

/************************** Function Definitions ***************************/
/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the NEXYS4IOinstance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus NEXYS4IO_Reg_SelfTest(u32 baseaddr)
{
	int write_loop_index;
	int read_loop_index;


	xil_printf("******************************\n\r");
	xil_printf("* Nexys4IO Peripheral Self Test\n\r");
	xil_printf("******************************\n\n\r");

	/*
	 * Write to user logic slave module register(s) and read back
	 */

	xil_printf("User logic slave module test...\n\r");
	// write values to the first 4 registers
	for (write_loop_index = 0 ; write_loop_index < 4; write_loop_index++)
	{
		NEXYS4IO_mWriteReg (baseaddr, write_loop_index*4, (write_loop_index+1) * READ_WRITE_MUL_FACTOR);
	}
	
	// read the values back, check to see if they match
	for (read_loop_index = 0 ; read_loop_index < 4; read_loop_index++)
	{
		// slave register 0 (BTNSW_IN) is a read-only register so skip it
		if (read_loop_index == 0) 
		{
			// register 0 (BTNSW_IN) is a read-only register skip it
			continue;
		}
		else
		{
			if ( NEXYS4IO_mReadReg (baseaddr, read_loop_index*4) != (u32) ((read_loop_index+1) * READ_WRITE_MUL_FACTOR))
			{
				xil_printf ("Error reading register value at address %x\n", (int)baseaddr + read_loop_index * 4);
				return XST_FAILURE;
			}
		}
	}

	xil_printf("   - slave register write/read passed\n\n\r");

	return XST_SUCCESS;
}