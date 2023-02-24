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

/********************Local Variabales********************/
static uint8_t kp, kd, ki;
enum _const_select {kd_sel, ki_sel, kp_sel};

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

    if(uIO->button_state != btns) {
        uIO->button_state = btns;
        uIO->has_changed = true;
    }

    if(uIO->switch_state != sw) {
        uIO->switch_state = sw;
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
    uIO->has_changed = true;
    ki = kp = kd = 0;
}

void update_pid(ptr_user_io_t uIO) {
    static uint8_t prev_btn = 0xff;
    static uint16_t prev_sw = 0xffff;
    static uint8_t step_val = 1;
    static uint8_t const_sel = kp_sel; // default to kp constant

    if(uIO->has_changed) {
        if(prev_sw != uIO->switch_state) {
            prev_sw = uIO->switch_state;
            xil_printf("value chosen for steps was %d\r\n", ((prev_sw >> 5) & CONSTANT_STEP_MASK));
            // select step size
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
                            // TODO: change this to the set/control signal later
                            kd = 0;
                            ki = 1;
                            kp = 2;
                            break;

                        default: //shouldn't get here
                            break;
                    }
                }
            }
        }
        uIO->has_changed = false;
    }
}

void display(void) {
    NX4IO_SSEG_setDigit(SSEGHI, DIGIT7, kp/10);
    NX4IO_SSEG_setDigit(SSEGHI, DIGIT6, kp%10);

    NX4IO_SSEG_setDigit(SSEGHI, DIGIT4, ki/10);
    NX4IO_SSEG_setDigit(SSEGLO, DIGIT3, ki%10);

    NX4IO_SSEG_setDigit(SSEGLO, DIGIT1, kd/10);
    NX4IO_SSEG_setDigit(SSEGLO, DIGIT0, kd%10);
}
