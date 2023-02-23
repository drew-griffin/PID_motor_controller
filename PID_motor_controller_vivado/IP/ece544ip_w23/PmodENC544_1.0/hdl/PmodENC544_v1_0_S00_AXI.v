
`timescale 1 ns / 1 ps

	module PmodENC544_v1_0_S00_AXI #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line

		// Width of S_AXI data bus
		parameter integer C_S_AXI_DATA_WIDTH	= 32,
		// Width of S_AXI address bus
		parameter integer C_S_AXI_ADDR_WIDTH	= 4
	)
	(
		// Users to add ports here
        input wire encA,     // A and B quadrature inputs from PmodENC
        input wire encB,
        input wire encBTN,   // pushbutton input from PmodENC
        input wire encSWT,   // slide switch input from PmodENC
		// User ports ends
		// Do not modify the ports beyond this line

		// Global Clock Signal
		input wire  S_AXI_ACLK,
		// Global Reset Signal. This Signal is Active LOW
		input wire  S_AXI_ARESETN,
		// Write address (issued by master, acceped by Slave)
		input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_AWADDR,
		// Write channel Protection type. This signal indicates the
    		// privilege and security level of the transaction, and whether
    		// the transaction is a data access or an instruction access.
		input wire [2 : 0] S_AXI_AWPROT,
		// Write address valid. This signal indicates that the master signaling
    		// valid write address and control information.
		input wire  S_AXI_AWVALID,
		// Write address ready. This signal indicates that the slave is ready
    		// to accept an address and associated control signals.
		output wire  S_AXI_AWREADY,
		// Write data (issued by master, acceped by Slave) 
		input wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_WDATA,
		// Write strobes. This signal indicates which byte lanes hold
    		// valid data. There is one write strobe bit for each eight
    		// bits of the write data bus.    
		input wire [(C_S_AXI_DATA_WIDTH/8)-1 : 0] S_AXI_WSTRB,
		// Write valid. This signal indicates that valid write
    		// data and strobes are available.
		input wire  S_AXI_WVALID,
		// Write ready. This signal indicates that the slave
    		// can accept the write data.
		output wire  S_AXI_WREADY,
		// Write response. This signal indicates the status
    		// of the write transaction.
		output wire [1 : 0] S_AXI_BRESP,
		// Write response valid. This signal indicates that the channel
    		// is signaling a valid write response.
		output wire  S_AXI_BVALID,
		// Response ready. This signal indicates that the master
    		// can accept a write response.
		input wire  S_AXI_BREADY,
		// Read address (issued by master, acceped by Slave)
		input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_ARADDR,
		// Protection type. This signal indicates the privilege
    		// and security level of the transaction, and whether the
    		// transaction is a data access or an instruction access.
		input wire [2 : 0] S_AXI_ARPROT,
		// Read address valid. This signal indicates that the channel
    		// is signaling valid read address and control information.
		input wire  S_AXI_ARVALID,
		// Read address ready. This signal indicates that the slave is
    		// ready to accept an address and associated control signals.
		output wire  S_AXI_ARREADY,
		// Read data (issued by slave)
		output wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_RDATA,
		// Read response. This signal indicates the status of the
    		// read transfer.
		output wire [1 : 0] S_AXI_RRESP,
		// Read valid. This signal indicates that the channel is
    		// signaling the required read data.
		output wire  S_AXI_RVALID,
		// Read ready. This signal indicates that the master can
    		// accept the read data and response information.
		input wire  S_AXI_RREADY
	);

	// AXI4LITE signals
	reg [C_S_AXI_ADDR_WIDTH-1 : 0] 	axi_awaddr;
	reg  	axi_awready;
	reg  	axi_wready;
	reg [1 : 0] 	axi_bresp;
	reg  	axi_bvalid;
	reg [C_S_AXI_ADDR_WIDTH-1 : 0] 	axi_araddr;
	reg  	axi_arready;
	reg [C_S_AXI_DATA_WIDTH-1 : 0] 	axi_rdata;
	reg [1 : 0] 	axi_rresp;
	reg  	axi_rvalid;

	// Example-specific design signals
	// local parameter for addressing 32 bit / 64 bit C_S_AXI_DATA_WIDTH
	// ADDR_LSB is used for addressing 32/64 bit registers/memories
	// ADDR_LSB = 2 for 32 bits (n downto 2)
	// ADDR_LSB = 3 for 64 bits (n downto 3)
	localparam integer ADDR_LSB = (C_S_AXI_DATA_WIDTH/32) + 1;
	localparam integer OPT_MEM_ADDR_BITS = 1;
	//----------------------------------------------
	//-- Signals for user logic register space example
	//------------------------------------------------
	//-- Number of Slave Registers 4
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg0;     // rotary count - read-only
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg1;     // button [bit[0]) and slide switch (bit[1])
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg2;     // clear rotary count.  Set bit[0] to clear.  must set back to 0 after reset.
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg3;     // *RESERVED* (spare)
	
	wire	 slv_reg_rden;
	wire	 slv_reg_wren;
	reg [C_S_AXI_DATA_WIDTH-1:0]	 reg_data_out;
	integer	 byte_index;
	reg	 aw_en;

	// I/O Connections assignments

	assign S_AXI_AWREADY	= axi_awready;
	assign S_AXI_WREADY	= axi_wready;
	assign S_AXI_BRESP	= axi_bresp;
	assign S_AXI_BVALID	= axi_bvalid;
	assign S_AXI_ARREADY	= axi_arready;
	assign S_AXI_RDATA	= axi_rdata;
	assign S_AXI_RRESP	= axi_rresp;
	assign S_AXI_RVALID	= axi_rvalid;
	// Implement axi_awready generation
	// axi_awready is asserted for one S_AXI_ACLK clock cycle when both
	// S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_awready is
	// de-asserted when reset is low.

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_awready <= 1'b0;
	      aw_en <= 1'b1;
	    end 
	  else
	    begin    
	      if (~axi_awready && S_AXI_AWVALID && S_AXI_WVALID && aw_en)
	        begin
	          // slave is ready to accept write address when 
	          // there is a valid write address and write data
	          // on the write address and data bus. This design 
	          // expects no outstanding transactions. 
	          axi_awready <= 1'b1;
	          aw_en <= 1'b0;
	        end
	        else if (S_AXI_BREADY && axi_bvalid)
	            begin
	              aw_en <= 1'b1;
	              axi_awready <= 1'b0;
	            end
	      else           
	        begin
	          axi_awready <= 1'b0;
	        end
	    end 
	end       

	// Implement axi_awaddr latching
	// This process is used to latch the address when both 
	// S_AXI_AWVALID and S_AXI_WVALID are valid. 

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_awaddr <= 0;
	    end 
	  else
	    begin    
	      if (~axi_awready && S_AXI_AWVALID && S_AXI_WVALID && aw_en)
	        begin
	          // Write Address latching 
	          axi_awaddr <= S_AXI_AWADDR;
	        end
	    end 
	end       

	// Implement axi_wready generation
	// axi_wready is asserted for one S_AXI_ACLK clock cycle when both
	// S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_wready is 
	// de-asserted when reset is low. 

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_wready <= 1'b0;
	    end 
	  else
	    begin    
	      if (~axi_wready && S_AXI_WVALID && S_AXI_AWVALID && aw_en )
	        begin
	          // slave is ready to accept write data when 
	          // there is a valid write address and write data
	          // on the write address and data bus. This design 
	          // expects no outstanding transactions. 
	          axi_wready <= 1'b1;
	        end
	      else
	        begin
	          axi_wready <= 1'b0;
	        end
	    end 
	end       

	// Implement memory mapped register select and write logic generation
	// The write data is accepted and written to memory mapped registers when
	// axi_awready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted. Write strobes are used to
	// select byte enables of slave registers while writing.
	// These registers are cleared when reset (active low) is applied.
	// Slave register write enable is asserted when valid address and data are available
	// and the slave is ready to accept the write address and write data.
	assign slv_reg_wren = axi_wready && S_AXI_WVALID && axi_awready && S_AXI_AWVALID;

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      /* slv_reg0 and slv_reg1 are read-only
	      slv_reg0 <= 0;
	      slv_reg1 <= 0;
	      */
	      
	      slv_reg2 <= 0;
	      slv_reg3 <= 0;
	    end 
	  else begin
	    if (slv_reg_wren)
	      begin
	        case ( axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
	          /* slv_reg0 and slv_reg1 are read-only
	          2'h0:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 0
	                slv_reg0[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  
	          2'h1:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 1
	                slv_reg1[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  
	           */
	           
	          2'h2:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 2
	                slv_reg2[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  
	          2'h3:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 3
	                slv_reg3[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  
	          default : begin
	                      /* slv_reg0 and slv_reg1 are read-only
	                      slv_reg0 <= slv_reg0;
	                      slv_reg1 <= slv_reg1;
	                      */
	                      
	                      slv_reg2 <= slv_reg2;
	                      slv_reg3 <= slv_reg3;
	                    end
	        endcase
	      end
	  end
	end    

	// Implement write response logic generation
	// The write response and response valid signals are asserted by the slave 
	// when axi_wready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted.  
	// This marks the acceptance of address and indicates the status of 
	// write transaction.

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_bvalid  <= 0;
	      axi_bresp   <= 2'b0;
	    end 
	  else
	    begin    
	      if (axi_awready && S_AXI_AWVALID && ~axi_bvalid && axi_wready && S_AXI_WVALID)
	        begin
	          // indicates a valid write response is available
	          axi_bvalid <= 1'b1;
	          axi_bresp  <= 2'b0; // 'OKAY' response 
	        end                   // work error responses in future
	      else
	        begin
	          if (S_AXI_BREADY && axi_bvalid) 
	            //check if bready is asserted while bvalid is high) 
	            //(there is a possibility that bready is always asserted high)   
	            begin
	              axi_bvalid <= 1'b0; 
	            end  
	        end
	    end
	end   

	// Implement axi_arready generation
	// axi_arready is asserted for one S_AXI_ACLK clock cycle when
	// S_AXI_ARVALID is asserted. axi_awready is 
	// de-asserted when reset (active low) is asserted. 
	// The read address is also latched when S_AXI_ARVALID is 
	// asserted. axi_araddr is reset to zero on reset assertion.

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_arready <= 1'b0;
	      axi_araddr  <= 32'b0;
	    end 
	  else
	    begin    
	      if (~axi_arready && S_AXI_ARVALID)
	        begin
	          // indicates that the slave has acceped the valid read address
	          axi_arready <= 1'b1;
	          // Read address latching
	          axi_araddr  <= S_AXI_ARADDR;
	        end
	      else
	        begin
	          axi_arready <= 1'b0;
	        end
	    end 
	end       

	// Implement axi_arvalid generation
	// axi_rvalid is asserted for one S_AXI_ACLK clock cycle when both 
	// S_AXI_ARVALID and axi_arready are asserted. The slave registers 
	// data are available on the axi_rdata bus at this instance. The 
	// assertion of axi_rvalid marks the validity of read data on the 
	// bus and axi_rresp indicates the status of read transaction.axi_rvalid 
	// is deasserted on reset (active low). axi_rresp and axi_rdata are 
	// cleared to zero on reset (active low).  
	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_rvalid <= 0;
	      axi_rresp  <= 0;
	    end 
	  else
	    begin    
	      if (axi_arready && S_AXI_ARVALID && ~axi_rvalid)
	        begin
	          // Valid read data is available at the read data bus
	          axi_rvalid <= 1'b1;
	          axi_rresp  <= 2'b0; // 'OKAY' response
	        end   
	      else if (axi_rvalid && S_AXI_RREADY)
	        begin
	          // Read data is accepted by the master
	          axi_rvalid <= 1'b0;
	        end                
	    end
	end    

	// Implement memory mapped register select and read logic generation
	// Slave register read enable is asserted when valid address is available
	// and the slave is ready to accept the read address.
	assign slv_reg_rden = axi_arready & S_AXI_ARVALID & ~axi_rvalid;
	always @(*)
	begin
	      // Address decoding for reading registers
	      case ( axi_araddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
	        2'h0   : reg_data_out <= slv_reg0;
	        2'h1   : reg_data_out <= slv_reg1;
	        2'h2   : reg_data_out <= slv_reg2;
	        2'h3   : reg_data_out <= slv_reg3;
	        default : reg_data_out <= 0;
	      endcase
	end

	// Output register or memory read data
	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_rdata  <= 0;
	    end 
	  else
	    begin    
	      // When there is a valid read address (S_AXI_ARVALID) with 
	      // acceptance of read address by the slave (axi_arready), 
	      // output the read dada 
	      if (slv_reg_rden)
	        begin
	          axi_rdata <= reg_data_out;     // register read data
	        end   
	    end
	end    

	// Add user logic here
	// internal signals
	reg [31:0] rotary_count;           // rotary encoder count.  Will increment/decrement depending on which direction the rotary encoder shaft if turned.
	wire db_encBTN, db_encSWT;          // debounce encoder button and switch
	wire rotary_event, rotary_left;    // rotary_event toggled every time toe rotary encoder shaft is turned, rotary_left gives the direction of rotation
	wire clr_rotary_cnt;               // clear rotary count signal
	
	// instantiate the quadrature decoder for the rotary encoder shaft
	rotary_filter ROTFILTER (
        .rotary_a(encA),
        .rotary_b(encB),	
        .rotary_event(rotary_event),
        .rotary_left(rotary_left),	
        .clk(S_AXI_ACLK)
    );
    
    // debounce logic for encBTN (button) and encSWT (slide switch)
    debounce DEBOUNCER (
	   .clk(S_AXI_ACLK),
	   .in({encSWT, encBTN}),
       .db_out({db_encSWT, db_encBTN})
    );
    
    // implement the rotary count register
    always @(posedge S_AXI_ACLK or posedge clr_rotary_cnt) begin
        if (clr_rotary_cnt)
            rotary_count <= 32'd0;
        else if (rotary_event)
            if (rotary_left)
                rotary_count <= rotary_count - 1;
            else
                rotary_count <= rotary_count + 1;
        else
            rotary_count <= rotary_count;
     end
     
    // map the slave register
    always @* begin
        slv_reg0 = rotary_count;
        slv_reg1 = {30'b000000000000000000000000000000, db_encSWT, db_encBTN};
    end
    
    // implement the clr_rotary_count signal from bit[0] of slv_reg2.  1 to clear
    assign clr_rotary_cnt = slv_reg2[0];
	// User logic ends

	endmodule
	
	
	// rotary_filter.v - Interface to the Rotary Encoder on the PmodENC
//
// Copyright Roy Kravitz, Portland State University 2014, 2015
// Converted from Ken Chapman's VHDL code
//
// Created By:	Roy Kravitz
// Date:		15-March-2014
// Version:		1.0
//
// Revision History:
// -----------------
//	15-Mar-2014		RK		Created this module from the one I've used for years in ECE 540 and 544
//	
// Description:
// ------------
// This module encodes the A and B quadrature input from the rotary encoder to two signals
// The "rotary_event" output  is pulsed when the rotary encoder knob is turned in either
// direction.  The "rotary_left" output indicates which direction the knob was turned.  A 1 on
// "rotary_left" indicates the knob was turned to the left, a 0 indicates the knob was turned
// to the right
//
// NOTE:  This module assumes that the input clock is running much faster than the knob can
// be turned.
//
//////////////////////////////////////////////////////////////////////////////////////
module rotary_filter (
	input			rotary_a,			// A input from S3E Rotary Encoder
					rotary_b,			// B input from S3E Rotary Encoder
	
	output reg		rotary_event,		// Asserted high when rotary encoder changes position
					rotary_left,		// Asserted high when rotary direction is to the left
				
	input			clk					// input clock
);

	// declare internal variables
	reg				rotary_a_int,		// synchronization flip flops
					rotary_b_int;
				
	reg				rotary_q1,			// state flip-flops 
					rotary_q2,
					delay_rotary_q1;
										
	// The rotary switch contacts are filtered using their offset (one-hot) style to  
	// clean them. Circuit concept by Peter Alfke.
	always @(posedge clk) begin
		// Synchronize inputs to clock domain using flip-flops in input/output blocks.
		rotary_a_int <= rotary_a;
		rotary_b_int <= rotary_b;
		
		case ({rotary_b_int, rotary_a_int})
			2'b00: 	begin
						rotary_q1 <= 0;         
						rotary_q2 <= rotary_q2;
				   	end
			2'b01: 	begin
						rotary_q1 <= rotary_q1;         
						rotary_q2 <= 0;
				   	end
 			2'b10: 	begin
						rotary_q1 <= rotary_q1;         
						rotary_q2 <= 1;
				   	end
 			2'b11: 	begin
						rotary_q1 <= 1;         
						rotary_q2 <= rotary_q2;
				   	end
		endcase
	end //always
	
	// The rising edges of 'rotary_q1' indicate that a rotation has occurred and the 
	// state of 'rotary_q2' at that time will indicate the direction. 
	always @(posedge clk) begin
		// catch the first edge
	    delay_rotary_q1 <= rotary_q1;
      	if (rotary_q1 && ~delay_rotary_q1) begin
			// rotary position has changed
        	rotary_event <= 1;
       		rotary_left <= rotary_q2;
       	end
       	else begin
		// rotary position has not changed
        	rotary_event <= 0;
        	rotary_left <= rotary_left;
      	end
     end //always

endmodule


// debounce - debounces a quadrature encoder
//
// Created By:		Oliver Rew
// Last Modified:	09-April-2021 (RK)
//
// Revision History:
// -----------------
// Apr-2020     OR      Slimmed down Roy's debounce module for a single quadrature encoder
// Apr-2021     RK      Minor modifications for PmodENC544 IP block
//
// Description:
// ------------
// This circuit filters out mechanical bounce. It works by taking
// several time samples of the pushbutton and changing its output
// only after several sequential samples are the same value. It is
// taylored specifically for a single quadrature encoder
// 
///////////////////////////////////////////////////////////////////////////
module debounce
#(
	// parameters
	parameter integer	CLK_FREQUENCY_HZ		= 100000000, 
	
	// this calue determines how fast encoder ticks can be registered
	parameter integer	DEBOUNCE_FREQUENCY_HZ	= 5000, 
	parameter integer 	CNTR_WIDTH 				= 32
)
(
	// ports
	input				clk,		    // clock	
	input 		[1:0]	in,		        // pushbutton inputs - including CPU RESET button

	output reg	[1:0]	db_out  = 2'b00 	// debounced outputs of pushbuttons	
);

	// debounce clock divider 
	reg			[CNTR_WIDTH-1:0]	db_count = 0;
	wire		[CNTR_WIDTH-1:0]	top_cnt = ((CLK_FREQUENCY_HZ / DEBOUNCE_FREQUENCY_HZ) - 1);

	//shift registers used to debounce inputs
	reg [3:0]	in0 = 4'h0, 		in1 = 4'h0;
	
	// debounce clock
	always @(posedge clk)
	begin 
		if (db_count == top_cnt)
			db_count <= 1'b0;	
		else
			db_count <= db_count + 1'b1;
	end	// debounce clock
	
	always @(posedge clk) 
	begin
		if (db_count == top_cnt) begin	
			//shift registers for inputs
			in0	<= (in0 << 1) | in[0];		
			in1	<= (in1 << 1) | in[1];		
		end
		
		//debounced outputs
		case(in0) 4'b0000: db_out[0] <= 0; 4'b1111: db_out[0] <= 1; endcase
        case(in1) 4'b0000: db_out[1] <= 0; 4'b1111: db_out[1] <= 1; endcase
	end
	
endmodule