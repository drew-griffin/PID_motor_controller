

/***************************** Include Files *******************************/
#include "myHB3ip.h"

/***************************** Global variables ****************************/
static uint32_t baseAddress = 0L;
static bool isInitialized = false;

/************************** Function Definitions ***************************/
/**
 * Initializes the PmodENC544 peripheral and runs the self-test
 *
 * @param   baseaddr_p  base address of the PmodENC544 peripheral
 *
 * @return  returns XST_SUCCESS if the PmodENC544 is intialized, false otherwise
 *
 */
XStatus HB3_initialize(uint32_t baseaddr_p)
{
    XStatus sts;
    
    if (baseaddr_p == NULL) {
        isInitialized = false;
        return XST_FAILURE;  
    }
    
    if (isInitialized) {
        return XST_SUCCESS;
    }
    else {
        baseAddress = baseaddr_p;
        sts = MYHB3IP_Reg_SelfTest(baseAddress);
        if (sts != XST_SUCCESS)
            return XST_FAILURE;
        isInitialized = true;
    }
    return XST_SUCCESS; 
}

void HB3_setPWM(bool enable, u16 DC)
{
	u32 cntlreg;

	// initialize the value depending on whether PWM is enabled
	// enable is Control register[31]
	cntlreg = (enable) ? 0x80000000 : 0x0000000;

	// add the duty cycles
	cntlreg |= ((DC & 0x03FF) << 20);

	if (isInitialized){
		MYHB3IP_mWriteReg(baseAddress, HB3_PWM_OFFSET, cntlreg);
	}
}

uint32_t HB3_getTicks(void)
{
    uint32_t count;
    if (isInitialized) {
        count = MYHB3IP_mReadReg(baseAddress, HB3_TICKS_OFFSET);
    }
    else{
        count = 0xDEADBEEF;
    }
    return count;
}