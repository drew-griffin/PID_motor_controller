/**
 * rgbPWM_r2.v - Implements a 3-channel PWM circuit to control an RGB LED
 *
 * @author		Roy Kravitz (roy.kravitz@pdx.edu)
 * @date		14-December 2022
 *
 * Implements a 3 channel PWM circuit targeted for a Microblaze MCS embedded system.
 * The implementation, as written, is configured to be included in
 * a Microblaze MCS system using one of the GPO's to hold the duty
 * cycles for the three LED segments and an enable bit.   
 *
 * The PWM clock can be provided two ways.  First is to connect the system clock on the clock
 * input and select the clock divider input.  The second way is to bring a clock from a FIT timer
 * or programmable timer that directly controls the PWM clock.
 *
 * The design is loosely based on Digilent's PWM_v2_0 circuit
 *
 * @note This version of the design assigns the duty cycle to 1 when enable is deasserted.  The
 * PWM Analyzer cannot analyze a 0% duty cycle
 *
 * @note	The design is implemented in Verilog 2001 instead of
 * SystemVerilog because Vivado does not directly support
 * inclusion of SystemVerilog modules as RTL modules in the IP Integrator.
 * A workaround would be to develop the RTL modules in SystemVerilog and then
 * wrap them in a Verilog 2001 "wrapper." 
 */
 module rgbPWM
#(
	parameter USE_DIVIDER = 1'b0,	// 1 to enable clock divider.  0 to provide a direct lcok
	parameter DIVIDE_COUNT = 500,	// Clock divider terminal count
	
	parameter POLARITY = 1'b1,		// 1 to drive PWM output high when active.  0 to invert the PWM output
	parameter MAX_COUNT = 2048		// maximum count for the PWM counters
)
(
	input wire 			clk,			// input clock	
	input wire 			reset,			// reset signal - asserted LOW to reset circuit
	input wire [31:0]	controlReg,		// control register - duty cycle and enable bit
	
	// PWM outputs
	output wire rgbRED,
	output wire rgbGREEN,
	output wire rgbBLUE,
	// PWM counter clock (a debug signal)
	output wire clkPWM
);

reg [9:0]	redDC, greenDC, blueDC;						// red, green, and blue duty cycles from ControlReg
reg [9:0]	redDC_latch, greenDC_latch, blueDC_latch;	// latched duty cycle registers
reg			enable;										// enable RGB outputs - only 1 for all 3 outputs
reg [31:0]	count;										// period counter
reg [31:0]  div_count;
reg			div_out;									// ouput of clock divider


// input clock divider
always @(posedge clk) begin
	if (~reset) begin
		div_count <= 32'd0;
		div_out <= 1'b0;
	end else begin
		div_count <= div_count + 1'b1;
		if (div_count >= DIVIDE_COUNT) begin
			div_out <= ~div_out;
			div_count <= 32'd0;
		end
	end
end // clock divider

// PWM Counter clock mux 
assign clkPWM = USE_DIVIDER ? div_out : clk;

// generate/latch the duty cycle and enable from the control register
always @(posedge clkPWM) begin
	if (~reset) begin
		blueDC <= 10'd0;
		greenDC <= 10'd0;
		redDC <= 10'd0;
		enable <= 1'b0;
	end
	else begin
		blueDC <= controlReg[9:0];
		greenDC <= controlReg[19:10];
		redDC <= controlReg[29:20];
		enable <= controlReg[31];
	end
end //generate/latch duty cycle and enable

// PWM period counter
always @(posedge clkPWM) begin
	if (~reset) begin
		count <= 32'd0;
	end
	else begin
		if (enable) begin
			count = (count < MAX_COUNT) ? count + 1'b1 : 32'd0;
		end
        else begin
            count= 32'd0;
        end
    end
end // PWM period counter

// latch the duty cycle register so they only change on PWM count boundaries
always @(posedge clkPWM)begin
    if (~reset) begin
		blueDC_latch <= 10'd0;
		greenDC_latch <= 10'd0;
		redDC_latch <= 10'd0;
	end
	else begin
		if (enable) begin
			if (count >= MAX_COUNT) begin
                    blueDC_latch <= blueDC;
					greenDC_latch <= greenDC;
					redDC_latch <= redDC;
            end
        end
        else begin
			blueDC_latch <= blueDC;
			greenDC_latch <= greenDC;
			redDC_latch <= redDC;
		end
	end
end // latch duty cycle registers

// generate the PWM outputs
assign rgbRED = (enable && (redDC_latch > count)) ? POLARITY : ~POLARITY;
assign rgbGREEN = (enable && (greenDC_latch > count)) ? POLARITY : ~POLARITY;
assign rgbBLUE = (enable && (blueDC_latch > count)) ? POLARITY : ~POLARITY;

endmodule

	
	
		