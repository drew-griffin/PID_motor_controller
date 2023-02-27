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

/********************Control Constants********************/
#define CONSTANT_STEP_MASK              0x00003
#define CONSTANT_SELECT_MASK            0x00007
#define NBTNS                           5
#define SPEED_OFF 	1			// Duty Cycle of 1% to stop motor
//#define SPEED_MIN 	389			// Duty Cycle of 38% is minimum value to get motor to move
#define SPEED_MIN 	423			// Duty Cycle of 41% is minimum motor speed chosen
#define SPEED_MAX	1023		// Duty Cycle of 100% is max motor speed
#define SPEED_STEP	6			// gives 100 steps between min and max speed ~0.587% Duty Cycle

/********************Local Variables********************/
static uint8_t kp, kd, ki;
enum _const_select {kd_sel, ki_sel, kp_sel};
static uint8_t const_sel;
static uint16_t setpoint;
static bool pwmEnable = true;					// true to enable PWM output
static uint8_t count = 0;
static bool display_mode = true;
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

    if(uIO->button_state != btns) {
        uIO->button_state = btns;
        uIO->has_changed = true;
    }

    if(uIO->switch_state != sw) {
        uIO->switch_state = sw;
        uIO->has_changed = true;
    }

    if(uIO->rotary_count != count) {
    	uIO->rotary_count = count;
        uIO->has_changed = true;
    }

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
}

void update_pid(ptr_user_io_t uIO) {
    static uint8_t prev_btn = 0xff;
    static uint16_t prev_sw = 0xffff;
    static uint8_t step_val = 1;
    static uint8_t step_val_enc = 1;
    static uint8_t prev_count = 0xff;
    static uint8_t prev_enc_BtnSw = 0xff;

    if(uIO->has_changed) {
        if(prev_sw != uIO->switch_state) {
            prev_sw = uIO->switch_state;
            xil_printf("PID value chosen for steps was ");//, ((prev_sw >> 5) & CONSTANT_STEP_MASK));
            // select step size Switches[6:5]
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
            xil_printf("value chosen to change was %d\r\n", (prev_sw & CONSTANT_SELECT_MASK));
            uint8_t select_val = (prev_sw & CONSTANT_SELECT_MASK);
            if(select_val == 1) {
                const_sel = kd_sel;
            }
            else if(select_val < 4 && select_val >= 2) {
                const_sel = ki_sel;
            }
            else {
                const_sel = kp_sel;
            }
            xil_printf("Setpoint value chosen for steps was ");//, ((prev_sw >> 5) & CONSTANT_STEP_MASK));
            // select step size Switches[6:5]
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
                        case 0: // btnR
                            break;

                        case 1: // btnL
                            break;

                        case 2: // btnD
                            switch(const_sel) {
                                case kd_sel:
                                    kd -= step_val;
                                    break;
                                case ki_sel:
                                    ki -= step_val;
                                    break;
                                case kp_sel:
                                    kp -= step_val;
                                    break;
                                default:
                                    break;
                            }
                            break;

                        case 3: // btnU
                            switch(const_sel) {
                                case kd_sel:
                                    kd += step_val;
                                    break;
                                case ki_sel:
                                    ki += step_val;
                                    break;
                                case kp_sel:
                                    kp += step_val;
                                    break;
                                default:
                                    break;
                            }  
                            break;

                        case 4: // btnC
                            display_mode = !display_mode;
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
           // uint8_t count = prev_count * step_val_enc;
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
    		HB3_setPWM(pwmEnable, setpoint);
        }
        if(prev_enc_BtnSw != uIO->enc_BtnSw_state){
        	prev_enc_BtnSw = uIO->enc_BtnSw_state;
    		if(prev_enc_BtnSw & 0x01){
    			PMODENC544_clearRotaryCount(); // set rotary count to 0
    			count = 0;
    			pwmEnable = false;
    		}
    		if(prev_enc_BtnSw & 0x02)
    			xil_printf("switched\n\r");
        }
        uIO->has_changed = false;
    }
}

void display(void) {
	if(display_mode){
		NX4IO_SSEG_setSSEG_DATA(SSEGHI, 0x0058E30E);
		NX4IO_SSEG_setSSEG_DATA(SSEGLO, 0x00144116);
		uint32_t readPWM = PWM_Analyzer_GetDutyCycle_percent(PWM_BASEADDR);
		uint32_t readHB3 = HB3_getTicks();
		xil_printf("%d%% duty cycle; %d ticks per second\r\n", readPWM, readHB3);
	}
	else{
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT7, kp/10);
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT6, kp%10);
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT5, CC_SPACE);
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT4, ki/10);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT3, ki%10);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT2, CC_BLANK);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT1, kd/10);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT0, kd%10);

	    NX4IO_SSEG_setDecPt(SSEGHI, DIGIT6, (const_sel == kp_sel));
	    NX4IO_SSEG_setDecPt(SSEGLO, DIGIT3, (const_sel == ki_sel));
	    NX4IO_SSEG_setDecPt(SSEGLO, DIGIT0, (const_sel == kd_sel));
	}
}
