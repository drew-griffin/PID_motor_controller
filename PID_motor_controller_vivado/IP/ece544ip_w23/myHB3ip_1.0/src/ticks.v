`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/26/2023 08:50:50 PM
// Design Name: 
// Module Name: ticks
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module ticks
#(
    parameter MAX_COUNT = 100000000  // 100 MHz AXI clock input
)
(
    input wire clk,
    input wire reset,
    input wire tachA,
    output reg [31:0] tick_out
);
    // internal variables
    reg [31:0] clk_count;
    reg [31:0] tick_count;  
    reg previous_tachA;  
    // determine how many ticks per second
    always @(posedge clk) begin
        if(~reset) begin
            clk_count <= 32'd0;
            tick_count <= 32'd0;
        end
        else begin
            clk_count <= clk_count + 1'b1;
            previous_tachA <= tachA;
            // if positive edge of tick, increment tick count
            if(previous_tachA == 0 && tachA == 1) begin
                tick_count <= tick_count + 1'b1;
            end
            if(clk_count == MAX_COUNT) begin
                tick_out <= tick_count;
                clk_count <= 32'd0;
                tick_count <= 32'd0;
            end
        end
    end
endmodule


