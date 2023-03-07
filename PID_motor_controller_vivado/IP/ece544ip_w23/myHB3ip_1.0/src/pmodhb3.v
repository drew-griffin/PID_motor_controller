`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: PSU ECE 544 Winter 2023
// Engineer: Stephen, Drew, Noah
// 
// Create Date: 02/26/2023 08:50:50 PM
// Module Name: pmodhb3

// Revision 0.01 - File Created
// Additional Comments: creates the PWM duty cycle for a 2KHz output using 100MHz
//  					AXI clock. Based on the rgbPWM module provided by Roy Kravitz
//////////////////////////////////////////////////////////////////////////////////


module pmodhb3
#(
	parameter DIVIDE_COUNT = 49,	// Clock divider terminal count
	parameter POLARITY = 1'b1,		// 1 to drive PWM output high when active.  0 to invert the PWM output
	parameter MAX_COUNT = 1024		// maximum count for the PWM counters
)
(
    input wire clk,
    input wire reset,
    input wire tachA,
    input wire tachB,
    input wire [31:0]	controlReg,		// control register - duty cycle and enable bit
    output wire enable,
    output wire direction
    );
    
reg [9:0]	DC;			// red, green, and blue duty cycles from ControlReg
reg [9:0]	DC_latch;	// latched duty cycle registers
reg			enablePWM;		// enable RGB outputs - only 1 for all 3 outputs
reg [31:0]	count;	    // period counter
reg [31:0]  div_count;
reg			div_out;	// ouput of clock divider

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

// generate/latch the duty cycle and enable from the control register
always @(posedge div_out) begin
	if (~reset) begin
		DC <= 10'd0;
		enablePWM <= 1'b0;
	end
	else begin
		DC <= controlReg[29:20];
		enablePWM <= controlReg[31];
	end
end //generate/latch duty cycle and enable

// PWM period counter
always @(posedge div_out) begin
	if (~reset) begin
		count <= 32'd0;
	end
	else begin
		if (enablePWM) begin
			count = (count < MAX_COUNT) ? count + 1'b1 : 32'd0;
		end
        else begin
            count= 32'd0;
        end
    end
end // PWM period counter

// latch the duty cycle register so they only change on PWM count boundaries
always @(posedge div_out)begin
    if (~reset) begin
		DC_latch <= 10'd0;
	end
	else begin
		if (enablePWM) begin
			if (count >= MAX_COUNT) begin
					DC_latch <= DC;
            end
        end
        else begin
			DC_latch <= DC;
		end
	end
end // latch duty cycle registers
// generate the PWM output
assign enable = (enablePWM && (DC_latch > count)) ? POLARITY : ~POLARITY;
endmodule
