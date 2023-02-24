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
#define NBTNS                           5

/********************Local Variabales********************/
static uint8_t change_value[3] = {1, 5, 10};
static uint8_t kp, kd, ki;

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

    if(uIO->has_changed) {
        if(prev_sw != uIO->switch_state) {
            prev_sw = uIO->switch_state;
            NX4IO_setLEDs(prev_sw);
        }
        if(prev_btn != uIO->button_state) {
            prev_btn = uIO->button_state;
            uint8_t btnMask = 0x01;
            for (int i = 0; i < NBTNS; i++) {
                // look at the buttons one at a time. If a button was pushed
                // we increment the appropriate changes: PWM, enable/disable, clear
                if (prev_btn & (btnMask << i)) {
                    // check btn[i] for changes
                    switch (i) {
                        //iterate through the buttons and modify global PWM
                        // duty cycle variables
                        case 0: // btnR
                            break;

                        case 1: // btnL
                            break;

                        case 2: // btnD
                            kd -= 1;
                            ki -= 1;
                            kp -= 1;
                            break;

                        case 3: // btnU
                            kd += 1;
                            ki += 1;
                            kp += 1;
                            break;

                        case 4: // btnC
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
