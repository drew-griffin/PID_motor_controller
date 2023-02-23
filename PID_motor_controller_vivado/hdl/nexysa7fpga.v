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
    uart_rtl_rxd,
    uart_rtl_txd,
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
  output uart_rtl_txd;
  input  uart_rtl_rxd;

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
  wire SA; 
  wire SB; 
  wire EN; 
  wire DIR; 
  // assign signals to the JA debug header
  assign JA[0] = DIR;
  assign JA[1] = EN;
  assign SA = JA[2];
  assign SB = JA[2]; //not used
  
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
        .led_0(led),
        .resetn(btnCpuReset),
        .seg_0(seg),
        .sw_0(sw),
        .uart_rtl_0_rxd(uart_rtl_rxd),
        .uart_rtl_0_txd(uart_rtl_txd),
        .EN(EN), 
        .SA(SA));
endmodule
