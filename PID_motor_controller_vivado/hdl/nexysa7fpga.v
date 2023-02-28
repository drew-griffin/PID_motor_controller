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
    // debug header/Motor
    JA_0,
    JA_1,
    // encoder header
    JC);
    
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
  output [1:0] JA_0;
  input  [1:0] JA_1;
  input  [7:4] JC;
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
  wire [3:0] JB;
  wire [31:0] control_reg;
  wire [31:0] gpio_pwm;
  wire [31:0] gpio_dir;
  // motor specific variables
  wire SA; 
  wire SB; 
  wire EN; 
  wire DIR; 
  //encoder specific variables
  wire EcA;
  wire EcB;
  wire EcBTN;
  wire EcSW;
  // assign signals to the JA debug header and motor
  assign JA_0[0] = DIR;
  assign JA_0[1] = EN;
  assign SA    = JA_1[0];
  assign SB    = JA_1[1]; //not used
  // assign signals to the JC header for encoder
  assign EcA   = JC[4]; // E7
  assign EcB   = JC[5]; // J3
  assign EcBTN = JC[6]; // J4
  assign EcSW  = JC[7]; // E6
  
  // wrap the gpio output to the Enable PWM control register
  assign control_reg = gpio_pwm;
  // wrap the gpio output to the Direction control (will only be one bit width)
  assign DIR = gpio_dir;
                  
  embsys embsys_i
       (.RGB1_Blue_0(RGB1_Blue),
        .RGB1_Green_0(RGB1_Green),
        .RGB1_Red_0(RGB1_Red),
        .RGB2_Blue_0(RGB2_Blue),
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
        .SA(SA),
        // switch these for directional readings
        .encA_0(EcB),
        .encB_0(EcA),
        .encBTN_0(EcBTN),
        .encSWT_0(EcSW),
        .gpio_dir_tri_o(gpio_dir),
        .gpio_pwm_tri_o(gpio_pwm));
endmodule
