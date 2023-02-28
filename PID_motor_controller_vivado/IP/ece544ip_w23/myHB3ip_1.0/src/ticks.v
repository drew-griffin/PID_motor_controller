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
// Additional Comments: counts ticks per 0.25s using 100 MHz AXI clock as input
// samples every 0.25s, multiplies by 4 to give approximation for ticks/second
//////////////////////////////////////////////////////////////////////////////////


module ticks
#(
    parameter MAX_COUNT = 25000000  // 0.25s
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
                tick_out <= (tick_count << 2);
                clk_count <= 32'd0;
                tick_count <= 32'd0;
            end
        end
    end
endmodule


