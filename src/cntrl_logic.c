/**
 * @file cntrl_logic.c
 * 
 * @authors Stephen, Drew, Noah
 * @copyright Portland State University, 2023
 * 
 * @brief
 * This is the source file for control functionality to the
 * push buttons, switches, encoder, and display.
 * 
 * <pre>
 * MODIFICATION HISTORY:
 * ---------------------
 * Ver  Who Date    Changes
 * -----------------------------------
 * 1.00a SW 23-Feb-2023 First release
 * </pre>
************************************************************/

#include "cntrl_logic.h"
#include "logger.h"
#include "fit.h" // need access to the counter for sends

/********************Control Constants********************/
#define CONSTANT_STEP_MASK              0x00003
#define CONSTANT_SELECT_MASK            0x00007
#define NBTNS                           5
#define SPEED_OFF 	                    1			// Duty Cycle of 1% to stop motor
//#define SPEED_MIN 	                389			// Duty Cycle of 38% is minimum value to get motor to move
#define SPEED_MIN 	                    423			// Duty Cycle of 41% is minimum motor speed chosen
#define SPEED_MAX	                    1023		// Duty Cycle of 100% is max motor speed
#define SPEED_STEP	                    6			// gives 100 steps between min and max speed ~0.587% Duty Cycle
#define ROT_BTN                         0x01        // mask for rotary push button
#define ROT_SW                          0x02        // mask for rotary switch

/********************Local File Variables********************/
static uint8_t kp, kd, ki;
enum _const_select {kd_sel, ki_sel, kp_sel};
static uint8_t const_sel;
static uint16_t setpoint;
static bool pwmEnable = true;					// true to enable PWM output
static uint8_t count = 0;
static bool set_mode = true;
static bool send_uart_data = false; 
static bool curr_data_sent = false; 
static uint8_t PID_control_sel = 0x00;
/**
 * read_user_IO() - reads user IO
 * 
 * @brief       Function that takes in a user IO struct and updates if state has changed.
 *              Passed along to display/processing system to handle
 * 
 * @param       pointer to user IO struct
 * 
 * @returns     updated user IO struct if anything has changed
*/
void read_user_IO(ptr_user_io_t uIO) {
    uint8_t btns = NX4IO_getBtns();
    uint16_t sw = NX4IO_getSwitches();
    uint8_t count = PMODENC544_getRotaryCount();
    uint8_t encBnSw = PMODENC544_getBtnSwReg();

    // check if push buttons have changed
    if(uIO->button_state != btns) {
        uIO->button_state = btns;
        uIO->has_changed = true;
    }

    // check if switches have changed
    if(uIO->switch_state != sw) {
        uIO->switch_state = sw;
        uIO->has_changed = true;
    }

    // check if rotary has changed
    if(uIO->rotary_count != count) {
        uIO->rotary_count = count;
        uIO->has_changed = true;
    }

    // check if rotary switch or pbtn has
    // changed
    if(uIO->enc_BtnSw_state != encBnSw) {
        uIO->enc_BtnSw_state = encBnSw;
        uIO->has_changed = true;
    }
}

/**
 * init_IO_struct() - setus up IO struct for user
 * 
 * @brief       Function that initializes uIO struct at startup
 * 
 * @param       pointer to a user IO struct
 * 
 * @returns     initialized user IO struct
*/
void init_IO_struct(ptr_user_io_t uIO) {
    uIO->button_state = 0x00;
    uIO->switch_state = 0x0000;
    uIO->rotary_count = 0x00;
    uIO->enc_BtnSw_state = 0x00;
    uIO->has_changed = true;
    ki = kp = kd = 0;
    const_sel = kp_sel;
    wdt_crash = false;
}

/**
 * update_pid() - function loop that updates info from
 * incoming user interfaces of the switches, push buttons,
 * and encoder
 * 
 * @brief       Function that controls the user interfaces for
 *              changing functionality for the PID controller.
 *              Consumes a user IO struct to see if state has changed
 *              in either the switches, buttons, or encoder and
 *              updates for appropriate functionality.
 * 
 * @param       pointer to a user IO struct
*/
void update_pid(ptr_user_io_t uIO) {
    static uint8_t prev_btn = 0xff;         // holds previous buttons state
    static uint16_t prev_sw = 0xffff;       // holds previous switches state
    static uint8_t step_val = 1;            // step value for the k-constants
    static uint8_t step_val_enc = 1;        // step value for the set point
    static uint8_t prev_count = 0xff;       // previous encoder count
    static uint8_t prev_enc_BtnSw = 0xff;   // previous encoder btn/switch state

    // if user interface has changed process the change
    if(uIO->has_changed) {
        // if switches have changed proccess the new switch state
        if(prev_sw != uIO->switch_state) {
            prev_sw = uIO->switch_state;
            // updates the LEDs without interfereing with the WDT
            // LED status
            uint16_t leds = NX4IO_getLEDS_DATA() & 0x8000;
            NX4IO_setLEDs(leds | (prev_sw & 0x007F));
            xil_printf("PID value chosen for steps was ");
            // switch over the new step mask for k-constants Switches[2:0]
            switch((prev_sw >> 5) & CONSTANT_STEP_MASK) {
                case 0:
                    step_val = 1;
                    break;
                case 1:
                    step_val = 5;
                    break;
                case 2:
                    step_val = 10;
                    break;
                case 3:
                    step_val = 10;
                    break;
                default: // shouldn't get here but to be safe set back to single step
                    step_val = 1;
                    break;
            }
            xil_printf("%d\r\n", step_val);
            xil_printf("values chosen to use for PID was %d\r\n", (prev_sw & CONSTANT_SELECT_MASK));
            // get new state for PID control loop and display Switches[4:3]
            PID_control_sel = (prev_sw & CONSTANT_SELECT_MASK);

            xil_printf("Setpoint value chosen for steps was ");//, ((prev_sw >> 5) & CONSTANT_STEP_MASK));
            // select step size Switches[6:5] for set point
            switch((prev_sw >> 3) & CONSTANT_STEP_MASK) {
                case 0:
                    step_val_enc = 1;
                    break;
                case 1:
                    step_val_enc = 5;
                    break;
                case 2:
                    step_val_enc = 10;
                    break;
                case 3:
                    step_val_enc = 10;
                    break;
                default: // shouldn't get here but to be safe set back to single step
                    step_val_enc = 1;
                    break;
            }
            xil_printf("%d\r\n", step_val_enc);
        }
        if(prev_btn != uIO->button_state) {
            prev_btn = uIO->button_state;
            uint8_t btnMask = 0x01;
            for (int i = 0; i < NBTNS; i++) {
                // look at the buttons one at a time. If a button was pushed
                // we run control loop logic
                if (prev_btn & (btnMask << i)) {
                    // check btn[i] for changes
                    switch (i) {
                        //iterate through the buttons and modify global PID
                        // controller values
                        case 0: // btnR - change FSM for k-constant to change
                            if(set_mode) { // only do if we are in set mode
                                if(const_sel == kd_sel) {
                                    const_sel = ki_sel;
                                }
                                else if(const_sel == ki_sel) {
                                    const_sel = kp_sel;
                                }
                                else {
                                    const_sel = kd_sel;
                                }
                            }
                            break;

                        case 1: // btnL - turn off or on sending plot data
                                send_uart_data = !send_uart_data;
                            break;

                        case 2: // btnD - decrease k-constant value
                            if(set_mode) { // only do it when we are in set mode
                                switch(const_sel) { // choice based on FSM state
                                    case kd_sel:
                                        kd -= step_val;
                                        if(kd > 99) // we rolled over
                                            kd = 99;
                                        break;
                                    case ki_sel:
                                        ki -= step_val;
                                        if(ki > 99) // we rolled over
                                            ki = 99;
                                        break;
                                    case kp_sel:
                                        kp -= step_val;
                                        if(kp > 99) // we rolled over
                                            kp = 99;
                                        break;
                                    default:
                                        break;
                                }
                            }
                            break;

                        case 3: // btnU - increase k-constant value
                            if(set_mode) { // only do it when we are in set mode
                                switch(const_sel) { // choice based on FSM state
                                    case kd_sel:
                                        kd += step_val;
                                        if(kd > 99) // we rolled over
                                            kd = 1;
                                        break;
                                    case ki_sel:
                                        ki += step_val;
                                        if(ki > 99)  // we rolled over
                                            ki = 1;
                                        break;
                                    case kp_sel:
                                        kp += step_val;
                                        if(kp > 99)  // we rolled over 
                                            kp = 1;
                                        break;
                                    default:
                                        break;
                                }
                            } 
                            break;

                        case 4: // btnC - change between set mode or run mode
                            set_mode = !set_mode;
                            break;

                        default: //shouldn't get here
                            break;
                    }
                }
            }
        }
        if(prev_count != uIO->rotary_count) { // has the knob turned?
            if((prev_count + 1) == uIO->rotary_count){
            	count += step_val_enc;
        	}
            if((prev_count - 1) == uIO->rotary_count){
            	count -= step_val_enc;
            }
            prev_count = uIO->rotary_count;
    		if(count == 0){
    			setpoint = SPEED_OFF;
    			//pwmEnable = false; // not sure if needed
    		}
    		else if((count > 0) && (count <= 100)){
    			setpoint = SPEED_MIN + (count * SPEED_STEP);
    			pwmEnable = true;
    		}
    		else{
    			PMODENC544_clearRotaryCount(); // set rotary count to 0
    			setpoint = SPEED_OFF;
    			pwmEnable = false; // specifically stated in project description
    			count = 0;
    		}
    		xil_printf("Updated Rotary Count %d, setpoint: %d\r\n", count, setpoint);
            // write the value to the motor
    		u32 PWMCntrlReg = buildPWMCtrlReg(pwmEnable, setpoint, 0, 0);
            XGpio_DiscreteWrite(&Gpio, 1, PWMCntrlReg);
        }
        if(prev_enc_BtnSw != uIO->enc_BtnSw_state){
        	prev_enc_BtnSw = uIO->enc_BtnSw_state;
    		if(prev_enc_BtnSw & ROT_BTN){
    			PMODENC544_clearRotaryCount(); // set rotary count to 0
    			count = 0;
                kp = 1;
                kd = ki = 0;
    		}
    		if(prev_enc_BtnSw & ROT_SW) {
    			xil_printf("switched\n\r");
                wdt_crash = true;
            }
        }
        // change state back to unchaged, want to be in this loop
        // as little as possible
        uIO->has_changed = false;
    }
}

/**
 * display() - displays set mode or run mode for user
 * 
 * @brief       displays either the setpoint and current RPM
 *              when in run mode or shows the Kp/Ki/Kd values
 *              and which will be run during run mode when in
 *              set mode.
*/
void display(void) {
	if(!set_mode){ // run mode -- display set point and motor rpm
		NX4IO_SSEG_setSSEG_DATA(SSEGHI, 0x0058E30E);
		NX4IO_SSEG_setSSEG_DATA(SSEGLO, 0x00144116);
	}
	else{ // set mode -- display K-constants and selected constants
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT7, kp/10);
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT6, kp%10);
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT5, CC_SPACE);
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT4, ki/10);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT3, ki%10);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT2, CC_BLANK);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT1, kd/10);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT0, kd%10);

        switch(PID_control_sel) {
            case 0: // open loop
                NX4IO_SSEG_setDecPt(SSEGHI, DIGIT6, false);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT3, false);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT0, false);
                break;
            case 1: // derivative control only
                NX4IO_SSEG_setDecPt(SSEGHI, DIGIT6, false);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT3, false);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT0, true);
                break;
            case 2: // integral control only
                NX4IO_SSEG_setDecPt(SSEGHI, DIGIT6, false);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT3, true);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT0, false);
                break;
            case 3: // ID control
                NX4IO_SSEG_setDecPt(SSEGHI, DIGIT6, false);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT3, true);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT0, true);
                break;
            case 4: // proportional control only
                NX4IO_SSEG_setDecPt(SSEGHI, DIGIT6, true);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT3, false);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT0, false);
                break;
            case 5: // PD control
                NX4IO_SSEG_setDecPt(SSEGHI, DIGIT6, true);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT3, false);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT0, true);
                break;
            case 6: // PI control
                NX4IO_SSEG_setDecPt(SSEGHI, DIGIT6, true);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT3, true);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT0, false);
                break;
            case 7: // PID control
                NX4IO_SSEG_setDecPt(SSEGHI, DIGIT6, true);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT3, true);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT0, true);
                break;
            default: // shouldn't get here
                NX4IO_SSEG_setDecPt(SSEGHI, DIGIT6, false);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT3, false);
                NX4IO_SSEG_setDecPt(SSEGLO, DIGIT0, false);
                break;
        }
	}
}

/**
 * send_uartlite_data
 * @brief sends uartlite data every second
 * if BtnL has been changed to true 
 */
void send_uartlite_data()
{
    if (send_uart_data == true && second_counter == 3 && curr_data_sent == false)
    {
        send_data(50, 50, kp, ki, kd); 
        curr_data_sent = true; 
    }
    else if (second_counter != 3)
    {
        curr_data_sent = false; 
    } 
    else
    {
        return; 
    }
} 

/**
 * buildPWMCtrlReg() - returns a PWM ControlReg value
 *
 * @brief combines the enable and PWM duty cycle bits to create
 * a value that can be loaded into the PWM Control register
 * This control register will write RGB_1 in this application.
 *
 * @param enable	PWM enable.  True to enable the PWM outputs
 * @param RedDc		Duty cycle for RED pwm.  Range is 0..1023
 * @param GreenDc	Duty cycle for GREEN pwm.  Range is 0..1023
 * @param Blue		Duty cycle for BLUE pwm.  Range is 0..1023
 *
 * @return			A PWM Control register value
 */

u32 buildPWMCtrlReg(bool enable, u16 RedDC, u16 GreenDC, u16 BlueDC)
{
	u32 cntlreg;

	// initialize the value depending on whether PWM is enabled
	// enable is Control register[31]
	cntlreg = (enable) ? 0x80000000 : 0x0000000;

	// add the duty cycles
	cntlreg |= ((BlueDC & 0x03FF) << 0) |
			   ((GreenDC & 0x03FF) << 10) |
			   ((RedDC & 0x03FF) << 20);
	return cntlreg;
}

