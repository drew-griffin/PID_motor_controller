/**
*
* @file nexys4IO_l.h
*
* @author Roy Kravitz (roy.kravitz@pdx.edu)
* @copyright Portland State University, 2014-2017, 2018
*
* This header file contains identifiers and low level driver functions for the Nexys4IO
* custom peripheral.  The peripheral provides access to the Nexys4 pushbuttons
* and slide switches, the LEDs, the RGB LEDs, and the Seven Segment display
* on the Digilent Nexys4 DDR board.
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
#ifndef NEXYS4IO_L_H
#define NEXYS4IO_L_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xil_io.h"
#include "xstatus.h"

/************************** Constant Definitions *****************************/

/** @name Registers
 *
 * Register offsets for this device.
 * @{
 */
#define NEXYS4IO_BTNSW_IN_OFFSET 0
#define NEXYS4IO_LEDS_DATA_OFFSET 4
#define NEXYS4IO_RGB1_DATA_OFFSET 8
#define NEXYS4IO_RGB1_CNTRL_OFFSET 12
#define NEXYS4IO_RGB2_DATA_OFFSET 16
#define NEXYS4IO_RGB2_CNTRL_OFFSET 20
#define NEXYS4IO_SSEGLO_DATA_OFFSET 24
#define NEXYS4IO_SSEGHI_DATA_OFFSET 28
#define NEXYS4IO_RSVD00_OFFSET 32
#define NEXYS4IO_RSVD01_OFFSET 36
#define NEXYS4IO_RSVD02_OFFSET 40
#define NEXYS4IO_RSVD03_OFFSET 44
#define NEXYS4IO_RSVD04_OFFSET 48
#define NEXYS4IO_RSVD05_OFFSET 52
#define NEXYS4IO_RSVD06_OFFSET 56
#define NEXYS4IO_RSVD07_OFFSET 60

/* @} */

/**************************** macros and function prototypes *****************************/
/**
 *
 * Write a value to a NEXYS4IO register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the NEXYS4IOdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void NEXYS4IO_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define NEXYS4IO_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a NEXYS4IO register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the NEXYS4IO device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 NEXYS4IO_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define NEXYS4IO_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

/************************** Function Prototypes ****************************/
/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseadde is the base address of the NEXYS4IO instance to be worked on.
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
XStatus NEXYS4IO_Reg_SelfTest(u32 baseaddr);


#endif // NEXYS4IO_L_H
