// debounce.v - debounces pushbuttons and switches
//
// Copyright Roy Kravitz, 2014, 2015
// 
// Created By:		Roy Kravitz
// Last Modified:	20-Aug-2014 (RK)
//
// Revision History:
// -----------------
// Sep-2008		RK		Created this module for the Digilent S3E Starter Board
// Sep-2012		RK		Modified module for the Digilent Nexys 3 board
// Dec-2014		RK		Cleaned up the formatting.  No functional changes
// Mar-2014		CZ		Modified module for the Digilent Nexys 4 board
// Aug-2014		RK		Parameterized module.  Modified for Vivado and Nexys4	
//
// Description:
// ------------
// This circuit filters out mechanical bounce. It works by taking
// several time samples of the pushbutton and changing its output
// only after several sequential samples are the same value
// 
///////////////////////////////////////////////////////////////////////////

module debounce
#(
	// parameters
	parameter integer	CLK_FREQUENCY_HZ		= 100000000, 
	parameter integer	DEBOUNCE_FREQUENCY_HZ	= 250,
	parameter integer	RESET_POLARITY_LOW		= 1,
	parameter integer 	CNTR_WIDTH 				= 32,
	
	parameter integer	SIMULATE				= 0,
	parameter integer	SIMULATE_FREQUENCY_CNT	= 5
)
(
	// ports
	input				clk,				// clock	
	input 		[5:0]	pbtn_in,			// pushbutton inputs - including CPU RESET button
	input 		[15:0]	switch_in,			// slider switch inputs

	output reg	[5:0]	pbtn_db  = 6'h0, 	// debounced outputs of pushbuttons	
	output reg	[15:0]	swtch_db = 16'h0	// debounced outputs of slider switches
);

	// CPU reset is on pb0.  need to take its polarity into account
	localparam [3:0]		pb0_in = RESET_POLARITY_LOW ? 4'h1 : 4'h0;
	
	// debounce clock divider 
	reg			[CNTR_WIDTH-1:0]	db_count = 0;
	wire		[CNTR_WIDTH-1:0]	top_cnt = SIMULATE ? SIMULATE_FREQUENCY_CNT : ((CLK_FREQUENCY_HZ / DEBOUNCE_FREQUENCY_HZ) - 1);


	//shift registers used to debounce switches and buttons	
	reg [3:0]	shift_pb0 = pb0_in;	
	reg [3:0]	shift_pb1 = 4'h0, 		shift_pb2 = 4'h0, 		shift_pb3 = 4'h0, 		shift_pb4 = 4'h0, 		shift_pb5 = 4'h0; 
	
	reg [3:0]	shift_swtch0 = 4'h0, 	shift_swtch1 = 4'h0, 	shift_swtch2 = 4'h0, 	shift_swtch3 = 4'h0;	
    reg [3:0]	shift_swtch4 = 4'h0, 	shift_swtch5 = 4'h0, 	shift_swtch6 = 4'h0, 	shift_swtch7 = 4'h0;
	reg [3:0]	shift_swtch8 = 4'h0, 	shift_swtch9 = 4'h0, 	shift_swtch10 = 4'h0, 	shift_swtch11 = 4'h0;	
    reg [3:0]	shift_swtch12 = 4'h0, 	shift_swtch13 = 4'h0, 	shift_swtch14 = 4'h0, 	shift_swtch15 = 4'h0;	
	
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
			//shift registers for pushbuttons
			shift_pb0	<= (shift_pb0 << 1) | pbtn_in[0];		
			shift_pb1	<= (shift_pb1 << 1) | pbtn_in[1];		
			shift_pb2	<= (shift_pb2 << 1) | pbtn_in[2];		
			shift_pb3	<= (shift_pb3 << 1) | pbtn_in[3];
			shift_pb4 	<= (shift_pb4 << 1) | pbtn_in[4]; 
			shift_pb5 	<= (shift_pb5 << 1) | pbtn_in[5];
			
			//shift registers for slider switches
			shift_swtch0 <= (shift_swtch0 << 1) | switch_in[0];
			shift_swtch1 <= (shift_swtch1 << 1) | switch_in[1];
			shift_swtch2 <= (shift_swtch2 << 1) | switch_in[2];
			shift_swtch3 <= (shift_swtch3 << 1) | switch_in[3];
			shift_swtch4 <= (shift_swtch4 << 1) | switch_in[4];
			shift_swtch5 <= (shift_swtch5 << 1) | switch_in[5];
			shift_swtch6 <= (shift_swtch6 << 1) | switch_in[6];
			shift_swtch7 <= (shift_swtch7 << 1) | switch_in[7];
			shift_swtch8 <= (shift_swtch8 << 1) | switch_in[8];
			shift_swtch9 <= (shift_swtch9 << 1) | switch_in[9];
			shift_swtch10 <= (shift_swtch10 << 1) | switch_in[10];
			shift_swtch11 <= (shift_swtch11 << 1) | switch_in[11];
			shift_swtch12 <= (shift_swtch12 << 1) | switch_in[12];
			shift_swtch13 <= (shift_swtch13 << 1) | switch_in[13];
			shift_swtch14 <= (shift_swtch14 << 1) | switch_in[14];
			shift_swtch15 <= (shift_swtch15 << 1) | switch_in[15];
		end
		
		//debounced pushbutton outputs
		case(shift_pb0) 4'b0000: pbtn_db[0] <= 0; 4'b1111: pbtn_db[0] <= 1; endcase
		case(shift_pb1) 4'b0000: pbtn_db[1] <= 0; 4'b1111: pbtn_db[1] <= 1; endcase
		case(shift_pb2) 4'b0000: pbtn_db[2] <= 0; 4'b1111: pbtn_db[2] <= 1; endcase
		case(shift_pb3) 4'b0000: pbtn_db[3] <= 0; 4'b1111: pbtn_db[3] <= 1; endcase
		case(shift_pb4) 4'b0000: pbtn_db[4] <= 0; 4'b1111: pbtn_db[4] <= 1; endcase
		case(shift_pb5) 4'b0000: pbtn_db[5] <= 0; 4'b1111: pbtn_db[5] <= 1; endcase
		
		//debounced slider switch outputs
		case(shift_swtch0) 4'b0000: swtch_db[0] <= 0;  4'b1111: swtch_db[0] <= 1; endcase
		case(shift_swtch1) 4'b0000: swtch_db[1] <= 0;  4'b1111: swtch_db[1] <= 1; endcase
		case(shift_swtch2) 4'b0000: swtch_db[2] <= 0;  4'b1111: swtch_db[2] <= 1; endcase
		case(shift_swtch3) 4'b0000: swtch_db[3] <= 0;  4'b1111: swtch_db[3] <= 1; endcase	
		case(shift_swtch4) 4'b0000: swtch_db[4] <= 0;  4'b1111: swtch_db[4] <= 1; endcase
		case(shift_swtch5) 4'b0000: swtch_db[5] <= 0;  4'b1111: swtch_db[5] <= 1; endcase
		case(shift_swtch6) 4'b0000: swtch_db[6] <= 0;  4'b1111: swtch_db[6] <= 1; endcase
		case(shift_swtch7) 4'b0000: swtch_db[7] <= 0;  4'b1111: swtch_db[7] <= 1; endcase
		case(shift_swtch8) 4'b0000: swtch_db[8] <= 0;  4'b1111: swtch_db[8] <= 1; endcase
		case(shift_swtch9) 4'b0000: swtch_db[9] <= 0;  4'b1111: swtch_db[9] <= 1; endcase
		case(shift_swtch10) 4'b0000: swtch_db[10] <= 0;  4'b1111: swtch_db[10] <= 1; endcase
		case(shift_swtch11) 4'b0000: swtch_db[11] <= 0;  4'b1111: swtch_db[11] <= 1; endcase	
		case(shift_swtch12) 4'b0000: swtch_db[12] <= 0;  4'b1111: swtch_db[12] <= 1; endcase
		case(shift_swtch13) 4'b0000: swtch_db[13] <= 0;  4'b1111: swtch_db[13] <= 1; endcase
		case(shift_swtch14) 4'b0000: swtch_db[14] <= 0;  4'b1111: swtch_db[14] <= 1; endcase
		case(shift_swtch15) 4'b0000: swtch_db[15] <= 0;  4'b1111: swtch_db[15] <= 1; endcase
	end
	
endmodule
