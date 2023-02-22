//Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2019.2 (lin64) Build 2708876 Wed Nov  6 21:39:14 MST 2019
//Date        : Wed Feb 22 11:51:14 2023
//Host        : parallels-Parallels-Virtual-Platform running 64-bit Ubuntu 22.04.1 LTS
//Command     : generate_target embys_wrapper.bd
//Design      : embys_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module embys_wrapper
   (RGB1_Blue_0,
    RGB1_Green_0,
    RGB1_Red_0,
    RGB2_Blue_0,
    RGB2_Green_0,
    RGB2_Red_0,
    an_0,
    btnC_0,
    btnD_0,
    btnL_0,
    btnR_0,
    btnU_0,
    clk_100MHz,
    dp_0,
    encA_0,
    encBTN_0,
    encB_0,
    encSWT_0,
    led_0,
    seg_0,
    sw_0,
    uart_rtl_0_rxd,
    uart_rtl_0_txd);
  output RGB1_Blue_0;
  output RGB1_Green_0;
  output RGB1_Red_0;
  output RGB2_Blue_0;
  output RGB2_Green_0;
  output RGB2_Red_0;
  output [7:0]an_0;
  input btnC_0;
  input btnD_0;
  input btnL_0;
  input btnR_0;
  input btnU_0;
  input clk_100MHz;
  output dp_0;
  input encA_0;
  input encBTN_0;
  input encB_0;
  input encSWT_0;
  output [15:0]led_0;
  output [6:0]seg_0;
  input [15:0]sw_0;
  input uart_rtl_0_rxd;
  output uart_rtl_0_txd;

  wire RGB1_Blue_0;
  wire RGB1_Green_0;
  wire RGB1_Red_0;
  wire RGB2_Blue_0;
  wire RGB2_Green_0;
  wire RGB2_Red_0;
  wire [7:0]an_0;
  wire btnC_0;
  wire btnD_0;
  wire btnL_0;
  wire btnR_0;
  wire btnU_0;
  wire clk_100MHz;
  wire dp_0;
  wire encA_0;
  wire encBTN_0;
  wire encB_0;
  wire encSWT_0;
  wire [15:0]led_0;
  wire [6:0]seg_0;
  wire [15:0]sw_0;
  wire uart_rtl_0_rxd;
  wire uart_rtl_0_txd;

  embys embys_i
       (.RGB1_Blue_0(RGB1_Blue_0),
        .RGB1_Green_0(RGB1_Green_0),
        .RGB1_Red_0(RGB1_Red_0),
        .RGB2_Blue_0(RGB2_Blue_0),
        .RGB2_Green_0(RGB2_Green_0),
        .RGB2_Red_0(RGB2_Red_0),
        .an_0(an_0),
        .btnC_0(btnC_0),
        .btnD_0(btnD_0),
        .btnL_0(btnL_0),
        .btnR_0(btnR_0),
        .btnU_0(btnU_0),
        .clk_100MHz(clk_100MHz),
        .dp_0(dp_0),
        .encA_0(encA_0),
        .encBTN_0(encBTN_0),
        .encB_0(encB_0),
        .encSWT_0(encSWT_0),
        .led_0(led_0),
        .seg_0(seg_0),
        .sw_0(sw_0),
        .uart_rtl_0_rxd(uart_rtl_0_rxd),
        .uart_rtl_0_txd(uart_rtl_0_txd));
endmodule
