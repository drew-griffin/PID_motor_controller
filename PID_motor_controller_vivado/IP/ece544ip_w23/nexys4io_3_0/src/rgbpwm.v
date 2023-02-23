`timescale 1ns / 1ps
// rgbpwm.v - Parameterized PWM generator for RGB LEDs with individual outputs for each LED
//
// Copyright Copyright Roy Kravitz, Portland State University 2014, 2015
//
// ACKNOWLEDGEMENT:  This module is based on simple_pwm.v written by Kurt Wick, University of
// Minnesota.  The design has beee extended for multiple outputs but the
// algorithm comes from simple_pwm.v
//
// Created By:	Roy Kravitz
// Date:		24-December-2014
// Version:		2.0
//
//
// Revision History:
// -----------------
//	19-Apr-14		RK		Created this module from Digilent's Ssg_decoder_core.v
//  24-Dec-14       RK      Simplified by removing clock divider.  Changed PWM channel enables
//
//
// Description:
// ------------
// This module provides 3 separate PWM channels capable of driving RGB LEDs.  The duty cycle of each channel
// can be set independently, providing an almost unlimited number of color combinations...for an LED. 
// The PWM channels share the same clock which is the system clock (100MHz for Nexys4). The PWM channels each have their
// own channel enables.  Setting the channel enable for a channel to 1 enables its PWM output. Setting the channel
// enable to 0 keeps the PWM output at 0, essentially disabling the PWM channel.  The PWM counters are free-running
//
// NOTE: The PWM counters are 1 bit wider than the PWM duty cycle input.  This was done to limit
// the duty cycle for an LED to 50% per the Digilent Nexys4 User guide.		
//
// The controller Inputs and Outputs are as follows:
//
//	CLK						I					Clock input.  Defaults to 50MHz but can be changed
//												with the CLK_FREQUENCY_HZ parameter
//
//	RESET					I					Resets the PWM channels.  The reset signal
//												defaults to active-low but can be configured with
//												the RESET_POLARITY_LOW parameter
//
//	RED_DC					I					Duty cycle for red LED.  Setting RED_DC to 0
//												turns the red LED off.  Setting the duty cycle to
//												255 turns the red LED fully on
//
//	GREEN_DC				I					Duty cycle for green LED.  Setting GREEN_DC to 0
//												turns the green LED off.  Setting the duty cycle to
//												255 turns the green LED fully on
//
//	BLUE_DC					I					Duty cycle for blue LED.  Setting BLUE_DC to 0
//												turns the blue LED off.  Setting the duty cycle to
//												255 turns the blue LED fully on
//
// PWM_CHANNEL_EN			I					Channel enables.  Setting a channel enable to 1
//												enables PWM for that channel  Setting a channel enable to 0
//												disables PWM (output is always 0, off)  
//												PWM_CHANNEL_EN ={Red Channel, Green Channel, Blue Channel}
//					
//	RGB_OUT					O					PWM outputs for the channels.
//												RGB_OUT = {Red Channel PWM, Green Channel PWM, Blue Channel PWM}
//												
//////////////////////////////////////////////////////////////////////////////////////
module rgbpwm
#(
	//parameter declarations
	parameter 			RESET_POLARITY_LOW = 1,				// Reset is active-low?  (default is yes)
	parameter 			PWM_DC_WIDTH = 8,					// Number of duty cycle bits for each channel (Default = 8-bit or 1/256 resolution)
	parameter			PWM_CH_COUNT = 3					// Number of PWM channels (Default is 3 channels)
)
(
	// port declarations
	input 						CLK, 						// system clock
	input						RESET,						// system reset
	input	[PWM_DC_WIDTH-1:0]	RED_DC,						// duty cycle for Red LED PWM channel
	input	[PWM_DC_WIDTH-1:0]	GREEN_DC,					// duty cycle for Green LED PWM channel
	input	[PWM_DC_WIDTH-1:0]	BLUE_DC,					// duty cycle for Blue LED PWM channel	
	input 	[PWM_CH_COUNT-1:0]	PWM_CHANNEL_EN,				// PWM enables for each channel	
	output	[PWM_CH_COUNT-1:0]	RGB_OUT						// PWM outputs for each channel
);

// local parameters
localparam PWM_CNTR_WIDTH = PWM_DC_WIDTH + 1;               // make the PWM counter one bit wider than the duty cycle input to keep
                                                            // the duty cycle below 50% per the Nexys4 User manual
localparam RED_INDEX = 2, GREEN_INDEX = 1, BLUE_INDEX = 0;

// internal variables
// loop index
integer i;

// use the RESET_POLARITY_LOW parameter to set the RESET_Int slevel
wire RESET_Int = RESET_POLARITY_LOW ? ~RESET : RESET;
								
// counter array.  Each PWM channel gets its own counter
reg			[PWM_CNTR_WIDTH-1:0]	pwm_cntr_ary[PWM_CH_COUNT-1:0];

// output signals
reg			[PWM_CH_COUNT-1:0]		pwm_out_reg;

	
// PWM channel counters
// counters overflow to restart PWM period
always @(posedge CLK) begin
	for (i = 0; i < PWM_CH_COUNT; i = i + 1) begin
		if (RESET_Int) begin
			pwm_cntr_ary[i] <= {PWM_CNTR_WIDTH{1'b0}};
		end
		else begin
			pwm_cntr_ary[i] <= pwm_cntr_ary[i] + 1'b1;	
		end
	end // for loop
end // pwm counters

// PWM output generation
// Block can be combinational because the counters are synchronized to the clock
// NOTE:  THIS CODE IS SPECIFIC TO RGB PWM
always @* begin
    // control the red PWM channel
    if (PWM_CHANNEL_EN[RED_INDEX] && (pwm_cntr_ary[RED_INDEX] < RED_DC)) 
        pwm_out_reg[RED_INDEX] = 1'b1;
    else
        pwm_out_reg[RED_INDEX] = 1'b0;
        
   // control the greem PWM channel
   if (PWM_CHANNEL_EN[GREEN_INDEX] && (pwm_cntr_ary[GREEN_INDEX] < GREEN_DC)) 
        pwm_out_reg[GREEN_INDEX] = 1'b1;
   else
        pwm_out_reg[GREEN_INDEX] = 1'b0;
        
   // control the greem PWM channel
   if (PWM_CHANNEL_EN[BLUE_INDEX] && (pwm_cntr_ary[BLUE_INDEX] < BLUE_DC)) 
        pwm_out_reg[BLUE_INDEX] = 1'b1;
   else
        pwm_out_reg[BLUE_INDEX] = 1'b0; 
               
end // PWM output generation

// assign the outputs
assign RGB_OUT = pwm_out_reg;

// initialize the counters and signal outputs.  Synthesis tool uses initial block for this
initial begin
	for (i = 0; i < PWM_CH_COUNT; i = i + 1) begin
		pwm_cntr_ary[i] = {PWM_CNTR_WIDTH{1'b0}};
		pwm_out_reg[i] = 1'b0;
	end
end  // initialization block

endmodule
			
	
	
