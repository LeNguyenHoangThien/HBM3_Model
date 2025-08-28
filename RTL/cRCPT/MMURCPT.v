
module MMURCPT #(parameter ADDR_WIDTH  = 35,
                 parameter TLB_ENTRIES = 32,
				 parameter VPN_WIDTH   = 23,
				 parameter PPN_WIDTH   = VPN_WIDTH,
				 parameter ATT_WIDTH   = 10,
				 parameter BSIZE_WIDTH = 9,
				 parameter PTE_LOG2    = 1)(
  input  wire                                  i_clk,
  input  wire                                  i_rst_n,
  input  wire                                  i_lookup,
  input  wire [ADDR_WIDTH - 1 : 0]             i_virtualAddr,
  output reg                                   o_hit,
  output reg                                   o_fault,
  output wire [ADDR_WIDTH - 1 : 0]             o_physicalAddr,
  output wire                                  o_busy,

  // Page Table Walker
  input  wire                                  i_ptwUpdate,
  input  wire [63 : 0]                         i_ptwPTE,
  input  wire [4  : 0]                         i_indexVictim
);

// State
localparam LOOKUP_STATE           = 3'd0;
localparam PTW_STATE              = 3'd1;
localparam UPDATE_STATE_RCPT_APTE = 3'd3;
localparam UPDATE_STATE_RCPT_B1_c = 3'd2;   // cRCPT Block 1
localparam UPDATE_STATE_RCPT_B2_c = 3'd6;   // cRCPT Block 2
localparam UPDATE_STATE_RCPT_B3_c = 3'd4;   // cRCPT Block 3
localparam UPDATE_STATE_RCPT_B1   = 3'd7;   // RCPT Block 1
localparam UPDATE_STATE_RCPT_B2   = 3'd5;   // RCPT Block 2

reg [2:0] state;
reg [2:0] state_next;

integer i;
integer indexHit;

wire cRCPT    = i_ptwPTE[63];
wire validB02 = i_ptwPTE[0];
wire validB1  = i_ptwPTE[1];
wire validB3  = i_ptwPTE[62];

wire [PPN_WIDTH - 1 : 0] tlbLinesPPN[0 : TLB_ENTRIES - 1];
wire                     tlbLinesHit[0 : TLB_ENTRIES - 1];
wire                     tlbLinesFault[0 : TLB_ENTRIES - 1];
reg                      tlbLinesUpdate[0 : TLB_ENTRIES - 1];

reg  [PPN_WIDTH   - 1 : 0] ptwPPN;


reg [6:0] ptwAPPN2217;

always @(posedge i_clk or negedge i_rst_n) begin
  if(i_rst_n == 1'b0) begin
    ptwAPPN2217 <= 7'd0;
  end else begin
    if(state_next == UPDATE_STATE_RCPT_APTE) begin
      ptwAPPN2217 <= i_ptwPTE[32 : 26];
    end
  end
end


always @(*) begin
  case(state_next)
	 UPDATE_STATE_RCPT_APTE : ptwPPN = i_ptwPTE[32 : 33 - PPN_WIDTH];
	 UPDATE_STATE_RCPT_B1_c : ptwPPN = {ptwAPPN2217, i_ptwPTE[52 : 37]};
	 UPDATE_STATE_RCPT_B2_c : ptwPPN = {ptwAPPN2217, i_ptwPTE[25 : 10]};
	 UPDATE_STATE_RCPT_B3_c : ptwPPN = {ptwAPPN2217, i_ptwPTE[32 : 26], i_ptwPTE[61 : 53]};
	 UPDATE_STATE_RCPT_B1   : ptwPPN = i_ptwPTE[59 : 60 - PPN_WIDTH];
	 UPDATE_STATE_RCPT_B2   : ptwPPN = i_ptwPTE[32 : 33 - PPN_WIDTH];
	 default                : ptwPPN = LOOKUP_STATE;  
  endcase
end


reg  [              3 : 0] ptwBSize;
//wire [BSIZE_WIDTH - 1 : 0] ptwBlockSize       = {1 << ptwBSize};
wire [BSIZE_WIDTH - 1 : 0] ptwBlockSize;
assign ptwBlockSize[0] = ~|ptwBSize;
assign ptwBlockSize[1] = ~ptwBSize[2] & ~ptwBSize[1] &  ptwBSize[0];
assign ptwBlockSize[2] = ~ptwBSize[2] &  ptwBSize[1] & ~ptwBSize[0];
assign ptwBlockSize[3] = ~ptwBSize[2] &  ptwBSize[1] &  ptwBSize[0];
assign ptwBlockSize[4] =  ptwBSize[2] & ~ptwBSize[1] & ~ptwBSize[0];
assign ptwBlockSize[5] =  ptwBSize[2] & ~ptwBSize[1] &  ptwBSize[0];
assign ptwBlockSize[6] =  ptwBSize[2] &  ptwBSize[1] &  ptwBSize[0];
assign ptwBlockSize[7] =  ptwBSize[2] &  ptwBSize[1] &  ptwBSize[0];
assign ptwBlockSize[8] =  ptwBSize[3];

wire [BSIZE_WIDTH - 1 : 0] ptwBlockSizeSubOne = {ptwBlockSize - 1'b1}; 
reg  [ATT_WIDTH   - 1 : 0] ptwATT;  // Attributes

wire [ADDR_WIDTH - VPN_WIDTH - 1 : 0] tlbOFFSET  = i_virtualAddr[ADDR_WIDTH - VPN_WIDTH - 1 : 0];

wire [3:0] BSize02 = i_ptwPTE[36:33];
wire [3:0] BSize1  = i_ptwPTE[ 9: 6];
wire [3:0] BSize3  = i_ptwPTE[ 5: 2];

//wire block3c = (state_next == UPDATE_STATE_RCPT_B3_c) ? 1'b1 : 1'b0; // Can optimize
wire block3c = state_next[2] & ~state_next[0] & ~state_next[0]; // Optimized
//wire block1  = (state_next == UPDATE_STATE_RCPT_B1_c) | (state_next == UPDATE_STATE_RCPT_B1);
wire block1  = state_next[1] & (state_next[2] ~^ state_next[0]);
//wire block02 = (state_next == UPDATE_STATE_RCPT_APTE) | (state_next == UPDATE_STATE_RCPT_B2_c) | (state_next == UPDATE_STATE_RCPT_B2);
wire block02 = (state_next[2]) ? (state_next[1] ^ state_next[0]) : (state_next[1] & state_next[0]);

always @(*) begin
  case({block02, block1, block3c})
    3'b100 : ptwBSize = BSize02;
    3'b010 : ptwBSize = BSize1;
    3'b001 : ptwBSize = BSize3;
    default: ptwBSize = BSize02;
  endcase
end

always @(posedge i_clk, negedge i_rst_n) begin
  if(i_rst_n == 1'b0) begin
    ptwATT <= {(ATT_WIDTH){1'b0}};
  end else begin
	if(state_next == UPDATE_STATE_RCPT_APTE) begin
	   ptwATT <= i_ptwPTE[ATT_WIDTH - 1 : 0];
    end
  end
end

wire [VPN_WIDTH   - 1 : 0] tlbVPN               = i_virtualAddr[ADDR_WIDTH - 1 : ADDR_WIDTH - VPN_WIDTH];
wire [ATT_WIDTH   - 1 : 0] tlbATT               = (state_next == UPDATE_STATE_RCPT_APTE) ? i_ptwPTE[ATT_WIDTH - 1 : 0] : ptwATT; // Can optimize
wire [VPN_WIDTH   - 1 : 0] AVPN                 = {{tlbVPN[VPN_WIDTH - 1 : PTE_LOG2]}, {(PTE_LOG2){1'b0}}};
wire [VPN_WIDTH   - 1 : 0] StartVPN             = AVPN & ~ptwBlockSizeSubOne;
reg  [VPN_WIDTH   - 1 : 0] ptwVPN_END_previous;
wire [VPN_WIDTH   - 1 : 0] ptwVPN               = (state_next == UPDATE_STATE_RCPT_APTE) ? StartVPN : ptwVPN_END_previous; // Can optimize

wire [VPN_WIDTH   - 1 : 0] ptwVPN_END           = {ptwVPN + ptwBlockSize};

always @(posedge i_clk, negedge i_rst_n) begin
  if(i_rst_n == 1'b0) begin
    ptwVPN_END_previous <= {(VPN_WIDTH){1'b0}};
  end else begin
	ptwVPN_END_previous <= ptwVPN_END;
  end
end

// Instance TLB Lines
generate
  genvar index;
  for(index = 0; index < TLB_ENTRIES; index = index + 1) begin : genTLBLine
	 TLBLine #(
	  .ADDR_WIDTH(ADDR_WIDTH),
	  .VPN_WIDTH(VPN_WIDTH),
	  .PPN_WIDTH(PPN_WIDTH)) TLBLine (
      .i_clk(i_clk),
      .i_rst_n(i_rst_n),
      .i_lookup(i_lookup),
      .i_VPN(tlbVPN),
      .o_PPN(tlbLinesPPN[index]),
      .o_hit(tlbLinesHit[index]),
      .o_fault(tlbLinesFault[index]),
      .i_ptwUpdate(tlbLinesUpdate[index]),
      .i_ptwVPN(ptwVPN),
      .i_ptwVPN_END(ptwVPN_END),
      .i_ptwPPN(ptwPPN),
      .i_ptwATT(tlbATT)
    );
  end
endgenerate

reg [PTE_LOG2 : 0] remainPTE;

//always @(posedge i_clk, negedge i_rst_n) begin
//  if(i_rst_n == 1'b0) begin
//    remainPTE <= {{1'b1, {(PTE_LOG2 + 1){1'b0}}} - 1'b1};
//  end else begin
//    if(i_ptwUpdate == 1'b1) begin
//	   remainPTE <= remainPTE - 1'b1;
//	 end else begin
//	   if(|remainPTE == 1'b0) begin
//		  remainPTE <= {{1'b1, {(PTE_LOG2 + 1){1'b0}}} - 1'b1};
//		end
//	 end
//  end
//end

always @(posedge i_clk, negedge i_rst_n) begin
  if(i_rst_n == 1'b0) begin
    remainPTE <= {{1'b1, {(PTE_LOG2 + 1){1'b0}}} - 1'b1};
  end else begin
    if(i_ptwUpdate == 1'b1) begin
       case(state_next)
	     UPDATE_STATE_RCPT_APTE : remainPTE <= {remainPTE - 1'b1};
    	 UPDATE_STATE_RCPT_B1_c : remainPTE <= remainPTE;
	     UPDATE_STATE_RCPT_B2_c : remainPTE <= {remainPTE - 1'b1};
	     UPDATE_STATE_RCPT_B3_c : remainPTE <= {remainPTE - 1'b1};
	     UPDATE_STATE_RCPT_B1   : remainPTE <= {remainPTE - 1'b1};
	     UPDATE_STATE_RCPT_B2   : remainPTE <= {remainPTE - 1'b1};
	     default                : remainPTE <= remainPTE;           
       endcase
	 end else begin
	   if(|remainPTE == 1'b0) begin
		  remainPTE <= {{1'b1, {(PTE_LOG2 + 1){1'b0}}} - 1'b1};
		end
	 end
  end
end


always @(posedge i_clk, negedge i_rst_n) begin
  if(i_rst_n == 1'b0) begin
    state <= LOOKUP_STATE;
  end else begin
    state <= state_next;
  end
end

always @(*) begin
  case(state)
	 LOOKUP_STATE           : state_next = (~o_hit & i_lookup) ? PTW_STATE              : state;
	 PTW_STATE              : state_next = (i_ptwUpdate == 1'b1)                       ? UPDATE_STATE_RCPT_APTE : state;
	 UPDATE_STATE_RCPT_APTE : state_next = (cRCPT == 1'b1) ? UPDATE_STATE_RCPT_B1_c : UPDATE_STATE_RCPT_B1;
	 UPDATE_STATE_RCPT_B1_c : state_next = UPDATE_STATE_RCPT_B2_c;
	 UPDATE_STATE_RCPT_B2_c : state_next = UPDATE_STATE_RCPT_B3_c;
	 UPDATE_STATE_RCPT_B3_c : state_next = ~(|remainPTE) ? LOOKUP_STATE : {(cRCPT) ? UPDATE_STATE_RCPT_B1_c : UPDATE_STATE_RCPT_B1};
	 UPDATE_STATE_RCPT_B1   : state_next = UPDATE_STATE_RCPT_B2;
	 UPDATE_STATE_RCPT_B2   : state_next = ~(|remainPTE) ? LOOKUP_STATE : {(cRCPT) ? UPDATE_STATE_RCPT_B1_c : UPDATE_STATE_RCPT_B1};
	 default                : state_next = LOOKUP_STATE;
  endcase
end

assign o_busy = (state == LOOKUP_STATE) ? 1'b0 : 1'b1;

always @(*) begin
  indexHit = 0;
  for(i = 1; i < TLB_ENTRIES; i = i + 1) begin
    if(tlbLinesHit[i] == 1'b1) begin
	   indexHit = i;
	 end
  end
end

always @(*) begin
  for(i = 0; i < TLB_ENTRIES; i = i + 1) begin
    if((i == i_indexVictim)         &
       (i_ptwUpdate == 1'b1)        &
	   (((state_next == UPDATE_STATE_RCPT_APTE) & validB02) |
	    ((state_next == UPDATE_STATE_RCPT_B1_c) & validB1)  |
	    ((state_next == UPDATE_STATE_RCPT_B2_c) & validB02) |
	    ((state_next == UPDATE_STATE_RCPT_B3_c) & validB3)  |
	    ((state_next == UPDATE_STATE_RCPT_B1)   & validB1)  |
	    ((state_next == UPDATE_STATE_RCPT_B2)   & validB02))) begin
	     tlbLinesUpdate[i] = 1'b1;
	 end else begin
	   tlbLinesUpdate[i] = 1'b0;
	 end
  end
end

always @(*) begin
  o_hit = tlbLinesHit[0];
  for(i = 1; i < TLB_ENTRIES; i = i + 1) begin
    o_hit = o_hit | tlbLinesHit[i];
  end
end

always @(*) begin
  o_fault = tlbLinesFault[0];
  for(i = 1; i < TLB_ENTRIES; i = i + 1) begin
    o_fault = o_fault | tlbLinesFault[i];
  end
end

wire [PPN_WIDTH   - 1 : 0] PPN = tlbLinesPPN[indexHit];

assign  o_physicalAddr = {PPN, tlbOFFSET};

endmodule

