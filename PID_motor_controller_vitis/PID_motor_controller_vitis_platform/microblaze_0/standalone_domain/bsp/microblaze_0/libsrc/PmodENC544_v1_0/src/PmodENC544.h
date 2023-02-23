
#ifndef PMODENC544_H
#define PMODENC544_H


/****************** Include Files ********************/
#include <stdbool.h> 
#include "xil_types.h"
#include "xil_io.h"
#include "xstatus.h"

#define PMODENC544_ROTARY_COUNT_REG_OFFSET 0
#define PMODENC544_BTNSWT_REG_OFFSET 4
#define PMODENC544_CLR_ROTARY_COUNT_REG_OFFSET 8
#define PMODENC544_SPARE_REG_OFFSET 12


/**************************** Type Definitions *****************************/
/**
 *
 * Write a value to a PMODENC544 register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the PMODENC544device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void PMODENC544_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define PMODENC544_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a PMODENC544 register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the PMODENC544 device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 PMODENC544_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define PMODENC544_mReadReg(BaseAddress, RegOffset) \
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
 * @param   baseaddr_p is the base address of the PMODENC544 instance to be worked on.
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
XStatus PMODENC544_Reg_SelfTest(uint32_t baseaddr_p);

// API function prototypes
XStatus PMODENC544_initialize(uint32_t baseaddr_p);
uint32_t PMODENC544_getRotaryCount(void);
uint32_t PMODENC544_getBtnSwReg(void);
uint32_t PMODENC544_clearRotaryCount(void);
bool PMODENC544_isBtnPressed(void);

#endif // PMODENC544_H
