

/***************************** Include Files *******************************/
#include "PmodENC544.h"

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
XStatus PMODENC544_initialize(uint32_t baseaddr_p)
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
        sts = PMODENC544_Reg_SelfTest(baseAddress);
        if (sts != XST_SUCCESS)
            return XST_FAILURE;
        PMODENC544_clearRotaryCount();
        isInitialized = true;
    }
    return XST_SUCCESS; 
}


/**
 * Returns the rotary encoder count
 *
 * @param   NONE - The base address is set with PMODENCE544_Initialize()
 *
 * @return  The current rotary count
 *
 */
uint32_t PMODENC544_getRotaryCount(void)
{
    uint32_t count;
    
    if (isInitialized) {
        count = PMODENC544_mReadReg(baseAddress, PMODENC544_ROTARY_COUNT_REG_OFFSET);
    }
    else {
        count = 0xDEADBEEF;
    }
    return count; 
}


/**
 * Returns the PmodENC button and switch values.  button is returned in bit[0].
 * switch is returned in bit[1].  All other bits are unused/reserved
 *
 * @param   NONE - The base address is set with PMODENCE544_Initialize()
 *
 * @return  The "raw" values (just the bits) of the button and switch
 *
 */
uint32_t PMODENC544_getBtnSwReg(void)
{
    uint32_t btnsw;
    
    if (isInitialized) {
        btnsw = PMODENC544_mReadReg(baseAddress, PMODENC544_BTNSWT_REG_OFFSET);
    }
    else {
        btnsw = 0xDEADBEEF;
    }
    return btnsw; 
}


/**
 * Sets the rotary encoder count register to 0
 *
 * @param   NONE - The base address is set with PMODENCE544_Initialize()
 *
 * @return  rotary encoder count...which should be 0
 *
 */
uint32_t PMODENC544_clearRotaryCount(void)
{
    uint32_t count;
    
    if (isInitialized) {
        // toggle bit[0] of the clear rotary count register
        count = 0x00000001;
        PMODENC544_mWriteReg(baseAddress, PMODENC544_CLR_ROTARY_COUNT_REG_OFFSET, count);
        PMODENC544_mWriteReg(baseAddress, PMODENC544_CLR_ROTARY_COUNT_REG_OFFSET, 0x0);
        count = PMODENC544_getRotaryCount();
    }
    else {
        count = 0xDEADBEEF;
    }
    return count; 
}


/**
 * Returns 1 if the PmodENC button (the rotary encoder shaft) is pressed
 *
 * @param   NONE - The base address is set with PMODENCE544_Initialize()
 *
 * @return  returns true if the button is pressed, false otherwise
 *
 */
bool PMODENC544_isBtnPressed(void)
{
    uint32_t btnsw;
    
    if (isInitialized){
        btnsw = PMODENC544_mReadReg(baseAddress, PMODENC544_BTNSWT_REG_OFFSET);
        return (btnsw & 0x1) ? true : false;
    }
    else
        return false; 
}
