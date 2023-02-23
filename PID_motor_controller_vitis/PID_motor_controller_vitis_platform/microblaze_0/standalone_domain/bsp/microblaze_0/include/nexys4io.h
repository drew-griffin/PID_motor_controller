/**
*
* @file nexys4IO.h
*
* @author Roy Kravitz (roy.kravitz@pdx.edu)
* @copyright Portland State University, 2014-2017m, 2018
*
* This header file contains identifiers and driver functions for the Nexys4IO
* custom peripheral.  The peripheral provides access to the Nexys4 pushbuttons
* and slide switches, LEDs, the RGB LEDs, and the Seven Segment display
* on the Digilent Nexys4 DDR board.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a	rhk	12/20/14	First release of driver
* 1.01a rhk	01/10/18	updated for SDK 2017.3
* </pre>
*
******************************************************************************/
#ifndef NEXYS4IO_H
#define NEXYS4IO_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
#include "stdbool.h"
#include "nexys4IO_l.h"

/************************** Constant Definitions *****************************/

/** @name Bit Masks
*
* Bit masks for the Nexys4IO registers.
*
* All of the registers in the Nexys4IO periheral are 32-bits wide
*
* @{
*/

// Masks for pushbuttons and switches
#define NEXYS4IO_BTNR_MASK 			0x00010000
#define NEXYS4IO_BTNL_MASK			0x00020000
#define NEXYS4IO_BTND_MASK			0x00040000
#define NEXYS4IO_BTNU_MASK			0x00080000
#define NEXYS4IO_BTNC_MASK			0x00100000
#define NEXYS4IO_ALLBTNS_MASK		0x001F0000
#define NEXYS4IO_ALLSWITCHES_MASK	0x0000FFFF

// Masks for LEDs
#define NEXYS4IO_LEDS_MASK			0x0000FFFF
#define NEXYS4IO_RGB_BLUEDC_MASK	0x000000FF
#define NEXYS4IO_RGB_GREENDC_MASK	0x0000FF00
#define NEXYS4IO_RGB_REDDC_MASK		0x00FF0000
#define NEXYS4IO_RGB_CHEN_MASK		0x00000007

// Masks for Seven Segment Display digits
#define NEXYS4IO_SSEG_DIG0_MASK		0x0000001F
#define NEXYS4IO_SSEG_DIG1_MASK		0x000007C0
#define NEXYS4IO_SSEG_DIG2_MASK		0x0001F000
#define NEXYS4IO_SSEG_DIG3_MASK		0x007C0000
#define NEXYS4IO_SSEG_DECPTS_MASK	0x0F000000
#define NEXYS4IO_SSEG_DECPT3_MASK	0x08000000
#define NEXYS4IO_SSEG_DECPT2_MASK	0x04000000
#define NEXYS4IO_SSEG_DECPT1_MASK	0x02000000
#define NEXYS4IO_SSEG_DECPT0_MASK	0x01000000	
	

/* @} */


/**************************** Type Definitions *****************************/
/** @name Literals and constants
*
* Literals and constants used for selecting specific devices
*
*/

// button selectors
enum _NX4IO_btns {BTNR, BTNL, BTND, BTNU, BTNC};

// RBG LED selectors
enum _NX4IO_rgbleds {RGB1 = 1, RGB2 = 2};

// Seven Segment digit selectors
enum _NX4IO_ssegbanks {SSEGLO = 1, SSEGHI = 2};
enum _NX4IO_ssegdigits {DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, DIGIT5, DIGIT6, DIGIT7};

// Seven Segment digit character codes
enum _NX4IO_charcodes {
	CC_0, CC_1, CC_2, CC_3, CC_4, CC_5, CC_6, CC_7, CC_8, CC_9,
	CC_A, CC_B, CC_C, CC_D, CC_E, CC_F,
	CC_SEGa, CC_SEGb, CC_SEGc, CC_SEGd, CC_SEGe, CC_SEGf, CC_SEGg,
	CC_SPACE, CC_UCH, CC_UCL, CC_UCR, CC_LCL, CC_LCR,
	CC_LCY, CC_BLANK, CC_BLANK1
};

enum _NX410_decpts {
	DP_0 = 0x0, DP_1 = 0x01, DP_2 = 0x04, DP_3 = 0x8, DP_ALL = 0xF, DP_NONE = 0x0
};

/***************** Macros (Inline Functions) Definitions ********************/


/************************** Function Prototypes *****************************/

// Initialization functions
int NX4IO_initialize(u32 BaseAddr);

// Buttons and switch functions
u32 NX4IO_getBTNSW_IN(void);
u8 NX4IO_getBtns(void);
u16 NX4IO_getSwitches(void);
bool NX4IO_isPressed(enum _NX4IO_btns);

// LED functions
u32 NX4IO_getLEDS_DATA(void);
void NX4IO_setLEDs(u32 ledvalue);

// RGB LED functions
u32 NX4IO_RGBLED_getRGB_DATA(enum _NX4IO_rgbleds led);
u32 NX4IO_RGBLED_getRGB_CNTRL(enum _NX4IO_rgbleds led);

void NX4IO_RGBLED_setRGB_DATA(enum _NX4IO_rgbleds led, u32 data);
void NX4IO_RGBLED_setRGB_CNTRL(enum _NX4IO_rgbleds led, u32 cntrl);

void NX4IO_RGBLED_setDutyCycle(enum _NX4IO_rgbleds led, u8 redDC, u8 greenDC, u8 blueDC);
void NX4IO_RGBLED_setChnlEn(enum _NX4IO_rgbleds led, bool en_red, bool en_green, bool en_blue);

// Seven Segment Display functions
u32 NX4IO_SSEG_getSSEG_DATA(enum _NX4IO_ssegbanks bank);
void NX4IO_SSEG_setSSEG_DATA(enum _NX4IO_ssegbanks bank, u32 data);

int NX4IO_SSEG_setDigit(enum _NX4IO_ssegbanks bank, enum _NX4IO_ssegdigits digit, enum _NX4IO_charcodes cc);
int NX4IO_SSEG_setDecPt(enum _NX4IO_ssegbanks bank, enum _NX4IO_ssegdigits digit, bool on);
int NX410_SSEG_setAllDigits(enum _NX4IO_ssegbanks bank, u8 dig3, u8 dig2, u8 dig1, u8 dig0, u8 dp);

int NX4IO_SSEG_putU16Hex(enum _NX4IO_ssegbanks bank, u16 data);
int NX4IO_SSEG_putU32Hex(u32 data);
int NX4IO_SSEG_putU32Dec(u32 data, bool trim);

#endif // NEXYS4IO_H
