////////////
// Note - modified by Drew Seidel (dseidel@pdx.edu)
// For ECE 544 Project 1  
//Top-level module for ECE 544 Project #1
// May have to be modified for your specific embedded system implementation
///////////
`timescale 1 ps / 1 ps

module nexysa7fpga
   (
    RGB2_Blue,
    RGB2_Green,
    RGB2_Red,
    an,
    btnC,
    btnD,
    btnL,
    btnR,
    btnU,
    dp,
    led,
    btnCpuReset,
    RGB1_Blue,
    RGB1_Green,
    RGB1_Red,
    seg,
    sw,
    clk,
    // debug header
    JA);
    
  output RGB2_Blue;
  output RGB2_Green;
  output RGB2_Red;
  output [7:0]an;
  input btnC;
  input btnD;
  input btnL;
  input btnR;
  input btnU;
  output dp;
  output [15:0]led;
  input btnCpuReset;
  output RGB1_Blue;
  output RGB1_Green;
  output RGB1_Red;
  output [6:0]seg;
  input [15:0]sw;
  input clk;
  output [3:0] JA;

  wire RGB2_Blue;
  wire RGB2_Green;
  wire RGB2_Red;
  wire [7:0]an;
  wire btnC;
  wire btnD;
  wire btnL;
  wire btnR;
  wire btnU;
  wire clkPWM;
  wire dp;
  wire [15:0]led;
  wire btnCpuReset;
  wire RGB1_Blue;
  wire RGB1_Green;
  wire RGB1_Red;
  wire [6:0]seg;
  wire [15:0]sw;
  wire clk;
  wire [3:0] JA;
  wire [31:0] control_reg;
  wire [31:0] gpio_rtl_0;
    
  // assign signals to the JA debug header
  assign JA[0] = clkPWM;
  assign JA[1] = RGB1_Blue;
  assign JA[2] = RGB1_Green;
  assign JA[3] = RGB1_Red;
  
  // wrap the gpio output to the rgbPWM control register
  assign control_reg = gpio_rtl_0;
                  
  embsys embsys_i
       (.RGB2_Blue_0(RGB2_Blue),
        .RGB2_Green_0(RGB2_Green),
        .RGB2_Red_0(RGB2_Red),
        .an_0(an),
        .btnC_0(btnC),
        .btnD_0(btnD),
        .btnL_0(btnL),
        .btnR_0(btnR),
        .btnU_0(btnU),
        .clkPWM_0(clkPWM),
        .clk_100MHz(clk),
        .controlReg_0(control_reg),
        .dp_0(dp),
        .gpio_rtl_0_tri_o(gpio_rtl_0),
        .led_0(led),
        .resetn(btnCpuReset),
        .rgbBLUE_0(RGB1_Blue),
        .rgbGREEN_0(RGB1_Green),
        .rgbRED_0(RGB1_Red),
        .seg_0(seg),
        .sw_0(sw));
endmodule
