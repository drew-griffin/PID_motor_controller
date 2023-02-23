
`timescale 1 ns / 1 ps

	module nexys4io_v3_0 #
	(
		// Users to add parameters here
        parameter integer    SIMULATE = 0,
		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 6
	)
	(
		// Users to add ports here
		// clock signal for the RGB PWM controller for the LEDs
        input wire  RGBLED_Clock,
         		
        // pushbutton and switch inputs from the Nexys4.  Input is 1 when the button is pressed
        input wire  btnU, btnD, btnR, btnL, btnC,   
        input wire [15:0] sw,
        
        // LED (LD17 - LD0) outputs to the Nexys4.  Set output to 1 to light the LED        
        output wire [15:0] led,
        
        // red, green, and blue PWM outputs to RGB1 (LD16) and RGB2 (LD17)       
        output wire RGB1_Red, RGB1_Green, RGB1_Blue,     
        output wire RGB2_Red, RGB2_Green, RGB2_Blue,
        
        // outputs to 7-segment display digits 7-0
        output wire [6:0] seg,
        output wire dp,
        output wire [7:0] an,
		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,
		output wire  s00_axi_awready,
		input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready
	);
// Instantiation of Axi Bus Interface S00_AXI
	nexys4io_v3_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) nexys4io_v3_0_S00_AXI_inst (
	    .RGBLED_Clock(RGBLED_Clock),
		.btnU(btnU),
        .btnD(btnD),
        .btnR(btnR),
        .btnL(btnL),
        .btnC(btnC),         
        .sw(sw),     
        .led(led),     
        .RGB1_Red(RGB1_Red),
        .RGB1_Green(RGB1_Green),
        .RGB1_Blue(RGB1_Blue),      
        .RGB2_Red(RGB2_Red),
        .RGB2_Green(RGB2_Green),
        .RGB2_Blue(RGB2_Blue),
        .seg(seg),
        .dp(dp),
        .an(an),
            
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready)
	);

	// Add user logic here

	// User logic ends

	endmodule

