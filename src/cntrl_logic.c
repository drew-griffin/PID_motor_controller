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
#include "microblaze_sleep.h"
#include "fit.h" // need access to the counter for sends

/********************Control Constants********************/
#define CONSTANT_STEP_MASK              0x00003
#define CONSTANT_SELECT_MASK            0x00007
#define NBTNS                           5
#define SPEED_OFF 	                    1			// Duty Cycle of 1% to stop motor
//#define SPEED_MIN 	                389			// Duty Cycle of 38% is minimum value to get motor to move
#define SPEED_MIN 	                    460			// Duty Cycle of 45% is minimum motor speed chosen
#define SPEED_MAX	                    614		    // Duty Cycle of 60% is max motor speed
#define RESOLUTION                      1023        // 10 bit resolution (absolute max duty cycle is 100% at 1023)
#define SPEED_STEP	                    6			// gives 100 steps between min and max speed ~0.587% Duty Cycle
#define ROT_BTN                         0x01        // mask for rotary push button
#define ROT_SW                          0x02        // mask for rotary switch
#define MIN_RPM                         38

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
static uint8_t set_rpm; 
static uint8_t read_rpm; 
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
                set_rpm = 0; 
               
    		}
    		xil_printf("Updated Rotary Count %d, setpoint: %d\r\n", count, setpoint);
            // write the value to the motor
    		HB3_setPWM(pwmEnable, setpoint);
        }
        if(prev_enc_BtnSw != uIO->enc_BtnSw_state){
        	prev_enc_BtnSw = uIO->enc_BtnSw_state;
    		if(prev_enc_BtnSw & ROT_BTN){
    			PMODENC544_clearRotaryCount(); // set rotary count to 0
    			count = 0;
                kp = 1;
                kd = ki = 0;
                set_rpm = 0;  
    		}
    		if(prev_enc_BtnSw & ROT_SW) {
    			xil_printf("switched\n\r");
                wdt_crash = true;
            }
        }
        // change state back to unchanged, want to be in this loop
        // as little as possible
        uIO->has_changed = false;
    }
}


/**
 * control_pid
 * @brief main pid control loop 
 * updates state based on file globals 
 */
void control_pid()
{
    static uint8_t preverror = 0; 
    static uint8_t i = 0; 

    if(set_mode || setpoint < SPEED_MIN || setpoint > SPEED_MAX)
    {
        return; 
    }
    else
    {
        usleep(100); 
        uint8_t i_control = 10; 
        uint8_t duty_cycle = setpoint_to_duty_cycle(setpoint); 
        set_rpm = duty_cycle_to_rpm(duty_cycle); 
        read_rpm = HB3_getRPM(); 
        int8_t error = set_rpm - read_rpm; 
        int8_t d = error - preverror; 
        preverror = error; 
        i = (error < set_rpm/i_control) ? i + error : 0; 

       
        int8_t GP = (PID_control_sel && 0x4) ? kp * error : 0;  //proportional control 
        GP = ((GP > 0) && (GP < 70)) ? GP : 0; 
        int8_t GI = (PID_control_sel && 0x2) ? (ki/i_control) * i  : 0;     //integral control
        GI = ((GI > 0) && (GI < 70)) ? GP : 0; 
        int8_t GD = (PID_control_sel && 0x1) ? kd * d : 0;      //derivative control
        GD = ((GD > 0) && (GD < 70)) ? GD : 0; 

        uint8_t output_rpm = set_rpm + GP + GI + GD; 
        uint16_t output_setpoint = setpoint_from_rpm(output_rpm); 
        if (output_setpoint < SPEED_MIN || output_setpoint > SPEED_MAX)
        {   
            xil_printf("Error: PID control loop producing out of range values\n\r");
            xil_printf("error = %d\n\r", error);  
            xil_printf("GP = %d, GI = %d, GD = %d\n\r", GP, GI, GD); 
            xil_printf("set rpm = %d, read rpm = %d\n\r", set_rpm, read_rpm); 
            xil_printf("setpoint = %d\n\r", setpoint);
            xil_printf("output setpoint = %d\nr", output_setpoint);
           return; 
        }
        HB3_setPWM(pwmEnable, output_setpoint); //change the motor speed by set PWM
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
		//uint32_t readPWM = PWM_Analyzer_GetDutyCycle_percent(PWM_BASEADDR);
		//PWM Analyzer HW is in design and was used for testing, but can be removed in final submission
		uint32_t HB3_RPM = HB3_getRPM();
		uint32_t HB3_ticks = HB3_getTicks();
// currently displays RPM on left side, and ticks/s on right
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT7, CC_BLANK);
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT6, CC_BLANK);
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT5, HB3_RPM/10);
	    NX4IO_SSEG_setDigit(SSEGHI, DIGIT4, HB3_RPM%10);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT3, CC_BLANK);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT2, HB3_ticks/100);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT1, (HB3_ticks/10) %10);
	    NX4IO_SSEG_setDigit(SSEGLO, DIGIT0, HB3_ticks%10);
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
{   if (set_mode)
    {
        return; 
    }
    if (send_uart_data == true && second_counter == 3 && curr_data_sent == false)
    {   
        uint8_t send_kp = (PID_control_sel & 0x4) ? kp : 0; 
        uint8_t send_ki = (PID_control_sel & 0x2) ? ki : 0; 
        uint8_t send_kd = (PID_control_sel & 0x1) ? kd : 0; 
        uint8_t send_read_rpm = (set_rpm == 0) ? 0 : read_rpm;
        send_data(set_rpm, send_read_rpm, send_kp, send_ki, send_kd); 
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
 * setpoint_to_duty_cycle
 * @brief Setpoint in 10bit range to duty cycle 
 * 
 * @param setpoint 
 * @return uint8_t 
 */
uint8_t setpoint_to_duty_cycle(uint16_t setpoint)
{
    float duty = (((float)setpoint / RESOLUTION) * 100);
    uint8_t duty_cycle = (uint8_t)(duty); 
    return duty_cycle; 
}

/**
 * duty_cycle_to_rpm
 * @brief duty cycle to rpm 
 * 
 * @param duty_cycle 
 * @return uint8_t 
 */
uint8_t duty_cycle_to_rpm(uint8_t duty_cycle)
{
    return duty_cycle - 3; //from characterization between duty cycle and rpm
}

/**
 * @brief convert from error rpm to setpoint 
 * @param rpm  
 * @return uint16_t setpoint 
 */
uint16_t setpoint_from_rpm(uint8_t rpm)
{
    uint8_t duty_cycle = rpm + 3; 
    uint16_t setpoint = (uint16_t)((float)(duty_cycle) / 100 * RESOLUTION); 

    return setpoint; 
}