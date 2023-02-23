/**
*
* @file nexys4IO.c
*
* @author Roy Kravitz (roy.kravitz@pdx.edu)
* @copyright Portland State University, 2014-2017, 2018
*
* This file implements the driver functions for the Nexys4IO
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
* 1.01a	rhk	01/10/18	updates for SDK 2017.3
* </pre>
*
******************************************************************************/

/***************************** Include Files *******************************/
#include "nexys4IO.h"

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions ****************************/
u32 NX4IO_BaseAddress;	// Base Address of the NEXYS4IO register set

/************************** Function Prototypes *****************************/
void bin2bcd(unsigned long bin, unsigned char *bcd);
void bin2hex(u32 bin, u8 *hex);

/************************** Driver Functions ********************************/

/****************** INITIALIZATION AND CONFIGURATION ************************/

/****************************************************************************/
/**
* Initialize the NEXYS4IO peripheral driver
*
* Saves the Base address of the NEXYS4IO peripheral and runs the selftest
*
* @param	BaseAddr is the base address of the NEXYS4IO register set
*
* @return
* 		- XST_SUCCESS	Initialization was successful.
*
* @note		This function can hang if the peripheral was not created correctly
* @note		The Base Address of the NEXYS4IO peripheral will be in xparameters.h
*
*****************************************************************************/
int NX4IO_initialize(u32 BaseAddr)
{
	NX4IO_BaseAddress = BaseAddr;
	return NEXYS4IO_Reg_SelfTest(NX4IO_BaseAddress);
}


/************************ BUTTONS AND SWITCHES ******************************/

/****************************************************************************/
/**
* returns the current value BTNSW_IN.
*
* Returns the raw value of BTNSW_IN.  No formatting or bit masking is done
*
* @param	None
*
* @return	current value of the pushbutons and switches.  No error checking
*			is done and the bit formatting is as shown in the datasheet.
*
* @note		See the NEXYS4IO Datasheet for the format of the BTNSW_IN register
*
*****************************************************************************/
u32 NX4IO_getBTNSW_IN(void)
{
	u32 val;

	val =  NEXYS4IO_mReadReg(NX4IO_BaseAddress, NEXYS4IO_BTNSW_IN_OFFSET);
	return val;
}


/****************************************************************************/
/**
* returns the current value of the pushbuttons
*
* Reads BTNSW_IN, masks the buttons and right justifies them
*
* @param	None
*
* @return	current value of the pushbuttons right justified in an 8-bit field
*
* @note		Buttons are returned as follows: 0 0 0 BTNC BTNU BTND BTNL BTNR
*
*****************************************************************************/
u8 NX4IO_getBtns(void)
{
	u32 val, btns;

	val =  NEXYS4IO_mReadReg(NX4IO_BaseAddress, NEXYS4IO_BTNSW_IN_OFFSET);
	btns = (val & NEXYS4IO_ALLBTNS_MASK) >> 16;
	return (u8) btns;
}


/****************************************************************************/
/**
* returns the current value of the slide switches
*
* Reads BTNSW_IN, masks the switches and right justifies them
*
* @param	None
*
* @return	current value of the switches right justified in a 16-bit field
*
* @note		switches are returned as follows:  SW15..SW0
*
*****************************************************************************/
u16 NX4IO_getSwitches(void)
{
	u32 val, sw;

	val =  NEXYS4IO_mReadReg(NX4IO_BaseAddress, NEXYS4IO_BTNSW_IN_OFFSET);
	sw = (val & NEXYS4IO_ALLSWITCHES_MASK) >> 0;
	return (u16) sw;
}


/****************************************************************************/
/**
* returns the state of the selected pushbutton
*
* Reads the pushbuttons and checks if the selected button is pressed (i.e. 1)
*
* @param	btnslct selects which button to check
*
* @return	true if the button is pressed, false otherwise
*
* @note		No error checking is done on btnslct.  default returns false
*
*****************************************************************************/
bool NX4IO_isPressed(enum _NX4IO_btns btnslct)
{
	u8 btns, msk;

	btns = NX4IO_getBtns();
	switch (btnslct)
	{
		case BTNR:
			msk = NEXYS4IO_BTNR_MASK >> 16;
			break;
		case BTNL:
			msk = NEXYS4IO_BTNL_MASK >> 16;
			break;
		case BTND:
			msk = NEXYS4IO_BTND_MASK >> 16;
			break;
		case BTNU:
			msk = NEXYS4IO_BTNU_MASK >> 16;
			break;
		case BTNC:
			msk = NEXYS4IO_BTNC_MASK >> 16;
			break;
		default:		msk = 0x00;	break;
	}
	return ((btns & msk) != 0) ? true : false;
}


/******************************** LEDS **************************************/

/****************************************************************************/
/**
* returns the current value LEDS_DATA.
*
* Returns the raw value of LEDS_DATA.  No formatting or bit masking is done
*
* @param	None
*
* @return	current value of the LEDs register.  No error checking
*			is done and the bit formatting is as shown in the datasheet.
*
* @note		See the NEXYS4IO Datasheet for the format of the LEDS_DATA register
*
*****************************************************************************/
u32 NX4IO_getLEDS_DATA(void)
{
	u32 val;

	val =  NEXYS4IO_mReadReg(NX4IO_BaseAddress, NEXYS4IO_LEDS_DATA_OFFSET);
	return val;
}


/****************************************************************************/
/**
* sets the LEDS_DATA register
*
* Lights (or not) the LEDS.
*
* @param	ledvalue is the value to write to the LEDS_DATA register.  The
*			unused bits are masked out and set to 0
*
* @return	NONE
*
* @note		See the NEXYS4IO Datasheet for the format of the LEDS_DATA register
*
*****************************************************************************/
void NX4IO_setLEDs(u32 ledvalue)
{
	u32 val;

	val = ledvalue & NEXYS4IO_LEDS_MASK;
	NEXYS4IO_mWriteReg(NX4IO_BaseAddress, NEXYS4IO_LEDS_DATA_OFFSET, val);
}


/******************************* RGB LEDS ***********************************/

/****************************************************************************/
/**
* returns the RGB_DATA register for the selected RGB LED
*
* Reads and returns the raw value of the selected RGB LED data register
*
* @param	led is used to select which of the RGB LED data registers to read
*
* @return	Raw (not formatted) value of the selected RGB LED data register
*
* @note		See the NEXYS4IO Datasheet for the format of the RGB_DATA register
* @note		No checking is done on the RGB LED select.  Returns 0 as default
*
*****************************************************************************/
u32 NX4IO_RGBLED_getRGB_DATA(enum _NX4IO_rgbleds led)
{
	u32 val;

	switch (led)
	{
		case RGB1:
			val =  NEXYS4IO_mReadReg(NX4IO_BaseAddress, NEXYS4IO_RGB1_DATA_OFFSET);
			break;
		case RGB2:
			val =  NEXYS4IO_mReadReg(NX4IO_BaseAddress, NEXYS4IO_RGB2_DATA_OFFSET);
			break;
		default:
			val = 0x00000000;
			break;
	}
	return val;
}


/****************************************************************************/
/**
* returns the RGB_CNTRL register for the selected RGB LED
*
* Reads and returns the raw value of the selected RGB LED control register
*
* @param	led is used to select which of the RGB LED control registers to read
*
* @return	Raw (not formatted) value of the selected RGB LED data register
*
* @note		See the NEXYS4IO Datasheet for the format of the RGB_CNTRL register
* @note		No checking is done on the RGB LED select.  Returns 0 as default
*
*****************************************************************************/
u32 NX4IO_RGBLED_getRGB_CNTRL(enum _NX4IO_rgbleds led)
{
	u32 val;

	switch (led)
	{
		case RGB1:
			val =  NEXYS4IO_mReadReg(NX4IO_BaseAddress, NEXYS4IO_RGB1_CNTRL_OFFSET);
			break;
		case RGB2:
			val =  NEXYS4IO_mReadReg(NX4IO_BaseAddress, NEXYS4IO_RGB2_CNTRL_OFFSET);
			break;
		default:
			val = 0x00000000;
			break;
	}
	return val;
}

/****************************************************************************/
/**
* sets the RGB_DATA register for the selected RGB LED
*
* Writes a new value to the selected RGB LED data register
*
* @param	led is used to select which of the RGB LED data registers to write
*
* @param	data is the value to be written to the register
*
* @return	NONE
*
* @note		See the NEXYS4IO Datasheet for the format of the RGB_DATA register
* @note		No checking is done on the RGB LED select.  Doesn't write invalid register
*
*****************************************************************************/
void NX4IO_RGBLED_setRGB_DATA(enum _NX4IO_rgbleds led, u32 data)
{
	u32 val, msk;

	msk = NEXYS4IO_RGB_REDDC_MASK | NEXYS4IO_RGB_GREENDC_MASK | NEXYS4IO_RGB_BLUEDC_MASK;
	val = data & msk;
	switch (led)
	{
		case RGB1:
			NEXYS4IO_mWriteReg(NX4IO_BaseAddress, NEXYS4IO_RGB1_DATA_OFFSET, val);
			break;
		case RGB2:
			NEXYS4IO_mWriteReg(NX4IO_BaseAddress, NEXYS4IO_RGB2_DATA_OFFSET, val);
			break;
		default:
			// Do not write to an illegal register
			break;
	}
}


/****************************************************************************/
/**
* sets the RGB_CNTRL register for the selected RGB LED
*
* Writes a new value to the selected RGB LED channel enable register
*
* @param	led is used to select which of the RGB LED data registers to write
*
* @param	cntrl is the value to be written to the register
*
* @return	NONE
*
* @note		See the NEXYS4IO Datasheet for the format of the RGB_CNTRL register
* @note		No checking is done on the RGB LED select.  Doesn't write invalid register
*
*****************************************************************************/
void NX4IO_RGBLED_setRGB_CNTRL(enum _NX4IO_rgbleds led, u32 cntrl)
{
	u32 val, msk;

	msk = NEXYS4IO_RGB_CHEN_MASK;
	val = cntrl & msk;
	switch (led)
	{
		case RGB1:
			NEXYS4IO_mWriteReg(NX4IO_BaseAddress, NEXYS4IO_RGB1_CNTRL_OFFSET, val);
			break;
		case RGB2:
			NEXYS4IO_mWriteReg(NX4IO_BaseAddress, NEXYS4IO_RGB2_CNTRL_OFFSET, val);
			break;
		default:
			// Do not write to an illegal register
			break;
	}
}


/****************************************************************************/
/**
* sets the duty cycle of the Red, Green and Blue channels of the selected RBG LED
*
* Formats the PWM duty cycles per the RGB_DATA register specification and writes
* the new duty cycles to the RGB_DATA register.  Duty cycles should be expressed as
* an 8-bit unsigned number.
*
* Momentarily disables the R, G, and B channels then changes the values and re-enables
* the channels that were previously enabled.
*
* @param	led is used to select which of the RGB LED data registers to read
*
* @param	redDC us the new duty cycle for the red LED in the RGB LED
*
* @param	greenDC us the new duty cycle for the green LED in the RGB LED
*
* @param	blueDC us the new duty cycle for the blue LED in the RGB LED
*
* @return	NONE
*
* @note		See the NEXYS4IO Datasheet for the format of the RGB_DATA register
* @note		No checking is done on the RGB LED select.  Doesn't write invalid register
* @note
* The RGB PWM logic in Nexys4IO limits the duty cycle to 50% as recommended
* in the Digilent Nexys4 User guide
*
*****************************************************************************/
void NX4IO_RGBLED_setDutyCycle(enum _NX4IO_rgbleds led, u8 redDC, u8 greenDC, u8 blueDC)
{
	u32 val, ctrl;

	// get the current enables and format the new duty cycle data
	ctrl = NX4IO_RGBLED_getRGB_CNTRL(led);
	val = ((redDC << 16) & NEXYS4IO_RGB_REDDC_MASK) | ((greenDC << 8) & NEXYS4IO_RGB_GREENDC_MASK)
			| ((blueDC << 0) & NEXYS4IO_RGB_BLUEDC_MASK);

	// change the duty cycles and restart the channels that were enabled
	NX4IO_RGBLED_setRGB_CNTRL(led, 0x00000000);
	NX4IO_RGBLED_setRGB_DATA(led, val);
	NX4IO_RGBLED_setRGB_CNTRL(led, ctrl);
}


/****************************************************************************/
/**
* sets the enables for the Red, Green and Blue channels of the selected RBG LED
*
* Formats the channel enables bits per the RGB_CNTRL register specification and writes
* the new duty cycles to the RGB_CNTRL register.  A channel is enabled by writing
* a 1 to its channel enable bit.
*
* @param	led is used to select which of the RGB LED data registers to read
*
* @param	en_red is the enable bit for the red LED in the RGB LED
*
* @param	en_green is the enable bit for the green LED in the RGB LED
*
* @param	en_blue is the enable bit for the blue LED in the RGB LED
*
* @return	NONE
*
* @note		See the NEXYS4IO Datasheet for the format of the RGB_CNTRL register
* @note		No checking is done on the RGB LED select.  Doesn't write invalid register
*
*****************************************************************************/
void NX4IO_RGBLED_setChnlEn(enum _NX4IO_rgbleds led, bool en_red, bool en_green, bool en_blue)
{
	u32 val;

	// set the channels based on their channel enable parameters
	val = 0x00000000;
	if (en_red)
		val |= 0x00000004;
	if (en_green)
		val |= 0x00000002;
	if (en_blue)
		val |= 0x00000001;
	NX4IO_RGBLED_setRGB_CNTRL(led, val);
}


/*************************** SEVEN SEGMENT DISPLAY **************************/

/****************************************************************************/
/**
* returns the SSEG_DATA register for the selected bank of digits
*
* Reads and returns the raw value of the selected SSEG_DATA data register.
* The Nexys4 board has two 4-digit seven segment display banks.  SSEGLO
* includes digits 3-0 (rightmost digits).  SSEGHI includes digits 7-4
* (leftmost digits)
*
* @param	bank is used to select which display bank register to read
*
* @return	Raw (not formatted) value of the selected data register
*
* @note		See the NEXYS4IO Datasheet for the format of the SSEG_DATA register
* @note		No checking is done on the bank select.  Returns 0 as default
*
*****************************************************************************/
u32 NX4IO_SSEG_getSSEG_DATA(enum _NX4IO_ssegbanks bank)
{
	u32 val;

	switch (bank)
	{
		case SSEGLO:
			val =  NEXYS4IO_mReadReg(NX4IO_BaseAddress, NEXYS4IO_SSEGLO_DATA_OFFSET);
			break;
		case SSEGHI:
			val =  NEXYS4IO_mReadReg(NX4IO_BaseAddress, NEXYS4IO_SSEGHI_DATA_OFFSET);
			break;
		default:
			val = 0x00000000;
			break;
	}
	return val;
}


/****************************************************************************/
/**
* sets the SSEG_DATA register for the selected bank of digits
*
* Writes a new value to the selected SSEG_DATA data register.
* The Nexys4 board has two 4-digit seven segment display banks.  SSEGLO
* includes digits 3-0 (rightmost digits).  SSEGHI includes digits 7-4
* (leftmost digits)
*
* @param	bank is used to select which of the SSEG_DATA data registers to write
*
* @param	data is the value to be written to the register
*
* @return	NONE
*
* @note		See the NEXYS4IO Datasheet for the format of the SSEG_DATA register
* @note		No checking is done on the bank select. Doesn't write invalid register
*
*****************************************************************************/
void NX4IO_SSEG_setSSEG_DATA(enum _NX4IO_ssegbanks bank, u32 data)
{
	u32 val, msk;

	msk = NEXYS4IO_SSEG_DECPTS_MASK | NEXYS4IO_SSEG_DIG3_MASK | NEXYS4IO_SSEG_DIG2_MASK
			| NEXYS4IO_SSEG_DIG1_MASK | NEXYS4IO_SSEG_DIG0_MASK;
	val = data & msk;
	switch (bank)
	{
		case SSEGLO:
			NEXYS4IO_mWriteReg(NX4IO_BaseAddress, NEXYS4IO_SSEGLO_DATA_OFFSET, val);
			break;
		case SSEGHI:
			NEXYS4IO_mWriteReg(NX4IO_BaseAddress, NEXYS4IO_SSEGHI_DATA_OFFSET, val);
			break;
		default:
			// Do not write to an illegal register
			break;
	}
}


/****************************************************************************/
/**
* sets a single digit in the selected bank of digits
*
* Writes a new character code to the specified digit in the SSEG_DATA for the
* selected bank.  The character code to write is checked to make sure it is
* in the range of the entries in the _NX4IO_charcodes table.  The remaining digits
* (those not specified) and the decimal points are not modified. Use NX4IO_SSEG_setSSEG_DATA()
* if you want to change more than one digit, or digit(s) and decimal points in a single operation
*
* The Nexys4 board has two 4-digit seven segment display banks.  SSEGLO
* includes digits 3-0 (rightmost digits).  SSEGHI includes digits 7-4
* (leftmost digits)
*
* @param	bank is used to select which of the SSEG_DATA data registers to write
*
* @param	digit specifies which digit (7-4 or 3-0) will be changed
*
* @param	cc is the new character code for the digit
*
* @return	XST_SUCCESS if the digit was changed.  XST_FAILURE if the operation
*			failed (i.e. one of the parameters was invalid)
*
* @note		See the NEXYS4IO Datasheet for the character code table and the
*			format of the SSEG_DATA registers
* @note		No checking is done on the bank select. The cc is checked to
*			see if it is range and only written if it is in range.  Doesn't write
*			invalid register
*
*****************************************************************************/
int NX4IO_SSEG_setDigit(enum _NX4IO_ssegbanks bank, enum _NX4IO_ssegdigits digit, enum _NX4IO_charcodes cc)
{
	u32 sseg_data;		// current value of the selected SSEG_DATA register
	u32 digit_data;		// digit data for the new character code

	// check the character code to make sure it is in range.  Fail and return if it isn't
	if ((cc < CC_0) || (cc > CC_BLANK1))
	{
		// out of range; we're finished here
		return XST_FAILURE;
	}

	// get the current SSEG_DATA register.  We will replace only
	// the bits of the digit that we're changing.  Fail if the bank
	// selection is invalid.
	switch (bank)
	{
		case SSEGLO:
			sseg_data = NX4IO_SSEG_getSSEG_DATA(SSEGLO);
			break;
		case SSEGHI:
			sseg_data = NX4IO_SSEG_getSSEG_DATA(SSEGHI);
			break;
		default:
			// Invalid bank.  Operation failed
			return XST_FAILURE;
	}

	// put the new character code in the correct position. Fail
	// if the digit specifier is invalid
	switch (digit)
	{
		case DIGIT0:
		case DIGIT4:
			// clear the bits we're going to change, leave the other bits alone
			digit_data = sseg_data & ~NEXYS4IO_SSEG_DIG0_MASK;
			// and put the new character code in place
			digit_data |= cc << 0;
			break;
		case DIGIT1:
		case DIGIT5:
			digit_data = sseg_data & ~NEXYS4IO_SSEG_DIG1_MASK;
			digit_data |= cc << 6;
			break;
		case DIGIT2:
		case DIGIT6:
			digit_data = sseg_data & ~NEXYS4IO_SSEG_DIG2_MASK;
			digit_data |= cc << 12;
			break;
		case DIGIT3:
		case DIGIT7:
			digit_data = sseg_data & ~NEXYS4IO_SSEG_DIG3_MASK;
			digit_data |= cc << 18;
			break;
		default:
			// invalid digit number;  we're finished here
			return XST_FAILURE;
	}

	// update the selected digit.  Fail if the bank is invalid (which should
	// not occur since we would have failed earlier on the register read
	switch (bank)
	{
		case SSEGLO:
			NX4IO_SSEG_setSSEG_DATA(SSEGLO, digit_data);
			break;
		case SSEGHI:
			NX4IO_SSEG_setSSEG_DATA(SSEGHI, digit_data);
			break;
		default:
			// Invalid bank.  Operation failed
			return XST_FAILURE;
	}

	// we made it!!
	return XST_SUCCESS;
}


/****************************************************************************/
/**
* sets a single decimal point in the selected bank of digits
*
* Changes the decimal point in the specified digit in the SSEG_DATA register for the
* selected bank.  The boolean 'on' defines whether the decimal point is lit or not.
* If 'on' is true the decimal point is lit.  If false, it is not.  The digits and
* the other decimal point values are unchanged.  Use NX4IO_SSEG_setSSEG_DATA()
* if you want to change more than one digit, or digit(s) and decimal points in a
* single operation
*
* The Nexys4 board has two 4-digit seven segment display banks.  SSEGLO
* includes digits 3-0 (rightmost digits).  SSEGHI includes digits 7-4
* (leftmost digits)
*
* @param	bank is used to select which of the SSEG_DATA data registers to write
*
* @param	digit specifies which digit (7-4 or 3-0) will be changed
*
* @param	on is the new state of the selected decimal point.
*
* @return	XST_SUCCESS if the decimal point was changed.  XST_FAILURE if the operation
*			failed (i.e. one of the parameters was invalid)
*
* @note		See the NEXYS4IO Datasheet for the character code table and the
*			format of the SSEG_DATA registers
* @note		No checking is done on the bank select. Doesn't write
*			invalid register
*
*****************************************************************************/
int NX4IO_SSEG_setDecPt(enum _NX4IO_ssegbanks bank, enum _NX4IO_ssegdigits digit, bool on)
{
	u32 sseg_data;		// current value of the selected SSEG_DATA register
	u32 decpt_data;		// decimal point data

	// get the current SSEG_DATA register.  We will replace only
	// the bits of the digit that we're changing.  Fail if the bank
	// selection is invalid.
	switch (bank)
	{
		case SSEGLO:
			sseg_data = NX4IO_SSEG_getSSEG_DATA(SSEGLO);
			break;
		case SSEGHI:
			sseg_data = NX4IO_SSEG_getSSEG_DATA(SSEGHI);
			break;
		default:
			// Invalid bank.  Operation failed
			return XST_FAILURE;
	}
	// put the new decimal point state in the correct position, leaving all of the other bits unchanged
	switch (digit)
	{
		case DIGIT0:
		case DIGIT4:
			// clear the bits we're going to change, leave the other bits alone
			decpt_data = sseg_data & ~NEXYS4IO_SSEG_DECPT0_MASK;
			// and put the new decimal point in place
			decpt_data |= (on ? 1 : 0) << 24;
			break;
		case DIGIT1:
		case DIGIT5:
			decpt_data = sseg_data & ~NEXYS4IO_SSEG_DECPT1_MASK;
			// and put the new decimal point in place
			decpt_data |= (on ? 1 : 0) << 25;
			break;
		case DIGIT2:
		case DIGIT6:
			decpt_data = sseg_data & ~NEXYS4IO_SSEG_DECPT2_MASK;
			// and put the new decimal point in place
			decpt_data |= (on ? 1 : 0) << 26;
			break;
		case DIGIT3:
		case DIGIT7:
			decpt_data = sseg_data & ~NEXYS4IO_SSEG_DECPT3_MASK;
			// and put the new decimal point in place
			decpt_data |= (on ? 1 : 0) << 27;
			break;
		default:
			// invalid digit number;  we're finished here
			return XST_FAILURE;
	}

	// update the selected digit.  Fail if the bank is invalid (which should
	// not occur since we would have failed earlier on the register read
	switch (bank)
	{
		case SSEGLO:
			NX4IO_SSEG_setSSEG_DATA(SSEGLO, decpt_data);
			break;
		case SSEGHI:
			NX4IO_SSEG_setSSEG_DATA(SSEGHI, decpt_data);
			break;
		default:
			// Invalid bank.  Operation failed
			return XST_FAILURE;
	}

	// we made it!!
	return XST_SUCCESS;
}


/****************************************************************************/
/**
* sets all of the digits and the decimal points in the selected bank of digits
*
* Writes a new to all of the digit in the SSEG_DATA for the  selected bank.  Also writes
* the decimal points.  It is expected that the digits be in the set specified by
* enum _NX4IO_ssegdigits but no checking is done.  Instead each digit will be written
* with the lower 5 bits of the 8-bit digit value(s) passed into the function.
*
* The Nexys4 board has two 4-digit seven segment display banks.  SSEGLO
* includes digits 3-0 (rightmost digits).  SSEGHI includes digits 7-4
* (leftmost digits)
*
* @param	bank is used to select which of the SSEG_DATA data registers to write
*
* @param	dig3, dig2, dig1 and dig0 are the new digit values.  dig3 is the leftmost
*			digit in the bank, dig0 is the rightmost digit in the bank.
*
* @param	dp is the enw value for the decimal points.  Only the least significant
*			four bits are used with bit[3] being the decimal point to the right of dig3
*			and so on to bit[0] and dig0.
*
* @return	XST_SUCCESS if the operation succeeds. XST_FAILURE if the operation
*			failed (i.e. one of the parameters was invalid)
*
* @note		No checking is done on the bank select. Doesn't write
*			invalid register
*
*****************************************************************************/
int NX410_SSEG_setAllDigits(enum _NX4IO_ssegbanks bank, u8 dig3, u8 dig2, u8 dig1, u8 dig0, u8 dp)
{
	u32 sseg_data;		// current value of the selected SSEG_DATA register
	u32 d3, d2, d1, d0, dpt;

	dpt = (dp & 0x0F)  << 24;
	d3 = (dig3 & 0x1F) << 18;
	d2 = (dig2 & 0x1F) << 12;
	d1 = (dig1 & 0x1F) << 6;
	d0 = (dig0 & 0x1F) << 0;

	sseg_data = dpt | d3 | d2 | d1 | d0;

	// update the selected baml.  Fail if the bank is invalid
	switch (bank)
	{
		case SSEGLO:
			NX4IO_SSEG_setSSEG_DATA(SSEGLO, sseg_data);
			break;
		case SSEGHI:
			NX4IO_SSEG_setSSEG_DATA(SSEGHI, sseg_data);
			break;
		default:
			// Invalid bank.  Operation failed
			return XST_FAILURE;
	}

	// we made it!!
	return XST_SUCCESS;
}


/****************************************************************************/
/**
* writes a 16-bit unsigned hex number to the selected display bank
*
* Breaks a 16-bit binary number (u16) into individual digits and displays
* them on the selected seven segment display bank.
*
* The Nexys4 board has two 4-digit seven segment display banks.  SSEGLO
* includes digits 3-0 (rightmost digits).  SSEGHI includes digits 7-4
* (leftmost digits)
*
* @param	bank is used to select which of the SSEG_DATA data registers to write
*
* @param	data is the 16-bit unsigned number that will be displayed in hex
*
*
* @return	XST_SUCCESS if the number was displayed correctly.  XST_FAILURE if the operation
*			failed (i.e. one of the parameters was invalid)
*
* @note		See the NEXYS4IO Datasheet for the character code table and the
*			format of the SSEG_DATA registers
* @note		No checking is done on the bank select. Doesn't write
*			invalid registers.
*
*****************************************************************************/
int NX4IO_SSEG_putU16Hex(enum _NX4IO_ssegbanks bank, u16 data)
{
	u8 cc[8];		// character codes for each of the nibbles in data
	u8 dp;			// current decimal points.  We don't want to change them

	// convert data to hex and display it on all the selected bank of digits
	bin2hex((u32) data, cc);
	switch (bank)
	{
		case SSEGLO:
			dp = (u8) (NX4IO_SSEG_getSSEG_DATA(SSEGLO)  >> 24);
			NX410_SSEG_setAllDigits(SSEGLO, cc[3], cc[2], cc[1], cc[0], dp);
			break;
		case SSEGHI:
			dp = (u8) (NX4IO_SSEG_getSSEG_DATA(SSEGHI)  >> 24);
			NX410_SSEG_setAllDigits(SSEGHI, cc[7], cc[6], cc[5], cc[4], dp);
			break;
		default:
			// Invalid bank.  Operation failed
			return XST_FAILURE;
	}

	// we made it!!
	return XST_SUCCESS;
}


/****************************************************************************/
/**
* writes a 32-bit unsigned hex number to the selected display bank
*
* Breaks a 32-bit binary number (u32) into individual digits and displays
* them on all 8 digits of the segment display
*
* The Nexys4 board has two 4-digit seven segment display banks.  SSEGLO
* includes digits 3-0 (rightmost digits).  SSEGHI includes digits 7-4
* (leftmost digits)
*
*
* @param	data is the 32-bit unsigned number that will be displayed in hex
*
*
* @return	XST_SUCCESS if the number was displayed correctly.  XST_FAILURE if the
*			operation failed (i.e. one of the parameters was invalid)
*
* @note		See the NEXYS4IO Datasheet for the character code table and the
*			format of the SSEG_DATA registers
*
*****************************************************************************/
int NX4IO_SSEG_putU32Hex(u32 data)
{
	u8 cc[8];		// character codes for each of the nibbles in data
	u8 dplo, dphi;	// current decimal points

	// get the existing current decimal points.  We don't want to change them
	dplo = (u8) (NX4IO_SSEG_getSSEG_DATA(SSEGLO)  >> 24);
	dphi = (u8) (NX4IO_SSEG_getSSEG_DATA(SSEGHI)  >> 24);

	// convert data to hex and display it on all 8 digits of the display
	bin2hex(data, cc);
	NX410_SSEG_setAllDigits(SSEGHI, cc[7], cc[6], cc[5], cc[4], dphi);
	NX410_SSEG_setAllDigits(SSEGLO, cc[3], cc[2], cc[1], cc[0], dplo);

	return XST_SUCCESS;
}


/****************************************************************************/
/**
* writes a 32-bit unsigned decimal number to the selected display bank
*
* Breaks a 32-bit binary number (u32) into individual digits and displays
* them on all 8 digits of the segment display.  Converts the number to packed
* BCD so that it can be displayed.  Does bounds checking on the maximum
* number that can be displayed (0 to 99.999.999) and fails if the number
* is out of range. Trims leading 0's
*
* The Nexys4 board has two 4-digit seven segment display banks.  SSEGLO
* includes digits 3-0 (rightmost digits).  SSEGHI includes digits 7-4
* (leftmost digits)
*
*
* @param	data is the 32-bit unsigned number that will be displayed in decimal
*
* @param	trim is a boolean.  If true, leading 0's ared converted to blanks
*
*
* @return	XST_SUCCESS if the number was displayed correctly.  XST_FAILURE if the
*			operation failed (i.e. one of the parameters was invalid)
*
* @note		See the NEXYS4IO Datasheet for the character code table and the
*			format of the SSEG_DATA registers
*
*****************************************************************************/
int NX4IO_SSEG_putU32Dec(u32 data, bool trim)
{
	u8 cc[10];
	int i;
	u8 dplo, dphi;		// current decimal points.  We don't want to change them

	// check if the number can be displayed
	if (data > 99999999)
	{
		// oops.  Out of range
		return XST_FAILURE;
	}

	// convert the number to BCD and break it into nibbles
	bin2bcd(data, cc);

	// convert leading 0's to blank if trim is enabled
	// cc[0] is the most significant BCD digit
	// we go one less than the size of cc[] so that the least significant 0
	// is not trimmed otherwise the display would be blank instead of saying 0
	if (trim)
	{
		for (i = 0; i < (int)(sizeof(cc) - 1); i++)
		{
			// check if the nibble is 0
			// keep going until the first non-zero digit
			if (cc[i] == 0)
			{
				// We found a leading 0.  make it a blank
				cc[i] = CC_BLANK;
			}
			else
			{
				// found a non-zero digit.  Break out of the loop
				break;
			}
		}
	}

	// update the display
	dplo = (u8) (NX4IO_SSEG_getSSEG_DATA(SSEGLO)  >> 24);
	dphi = (u8) (NX4IO_SSEG_getSSEG_DATA(SSEGHI)  >> 24);

	NX410_SSEG_setAllDigits(SSEGHI, cc[2], cc[3], cc[4], cc[5], dphi);
	NX410_SSEG_setAllDigits(SSEGLO, cc[6], cc[7], cc[8], cc[9], dplo);

	return XST_SUCCESS;

}

/************************** END OF DRIVER FUNCTIONS **************************/

/************************** Helper Functions ********************************/

/**
* Converts an unsigned integer to BCD so that it can be displayed
* on the Seven Segment display
*
* @param	bin is the unsigned integer to convert.
*
* @param	*bcd is a pointer to the buffer holding the result. The buffer
*			should be at least 10 bytes long. the BCD digits are returned
*			with the largest digit being in bcd[0] and the least signifcant
*			digit being in bcd[9]
*
* @return	NONE
*
* @note
*	Source:  http://www.keil.com/forum/14621/
*/

void bin2bcd(unsigned long bin, unsigned char *bcd)
{
	#define DIM(a)  (sizeof(a) / sizeof(a[0]))

    static const unsigned long pow_ten_tbl[] = {
        1000000000,
        100000000,
        10000000,
        1000000,
        100000,
        10000,
        1000,
        100,
        10,
        1
    };
    unsigned long pow_ten;
    unsigned char digit;
    unsigned char i;

    for (i = 0; i != DIM(pow_ten_tbl); i++) {
        digit = 0;
        pow_ten = pow_ten_tbl[i];

        while (bin >= pow_ten) {
            bin -= pow_ten;
            digit++;
        }
        *bcd++ = digit;
    }
}


/**
* Converts an unsigned integer to hex so that it can be displayed
* on the Seven Segment display
*
* @param	bin is the unsigned integer to convert
*
* @param	*hex is a pointer to the buffer holding the result.  The buffer
*			should be at least 8 bytes long. the hex values are returned with
*			the least significant nibble being in hex[0] and the most significant
*			being in hex[7].  This is different than bin2bcd() which returns
*			the most significant digits in the lower locations in the buffer
*
* @return	NONE
*
* @note
*	The hex digits map directly to character codes.  That is, the character 'F' is displayed for hex value of 0xF,
*	the character '4' is displayed for a hex value of 0x4, and so on.
*/
void bin2hex(u32 bin, u8 *hex)
{
	int i;

	// break the data into nibbles
	for (i = 0; i < 8; i++)
	{
		*hex++ = bin & 0x00000000F;
		bin = bin >> 4;
	}
}
