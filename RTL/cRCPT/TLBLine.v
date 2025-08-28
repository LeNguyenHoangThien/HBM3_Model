module TLBLine #(parameter ADDR_WIDTH  = 35,
				 parameter VPN_WIDTH   = 23,
				 parameter PPN_WIDTH   = VPN_WIDTH,
				 parameter ATT_WIDTH   = 10)(
  input  wire                                  i_clk,
  input  wire                                  i_rst_n,
  input  wire                                  i_lookup,
  input  wire [VPN_WIDTH - 1 : 0]              i_VPN,
  output wire [PPN_WIDTH - 1 : 0]              o_PPN,
  output wire                                  o_hit,
  output wire                                  o_fault,
    
	// Page Table Walker
  input  wire                                  i_ptwUpdate,
  input  wire [VPN_WIDTH - 1 : 0]              i_ptwVPN,
  input  wire [VPN_WIDTH - 1 : 0]              i_ptwVPN_END,
  input  wire [PPN_WIDTH - 1 : 0]              i_ptwPPN,
  input  wire [ATT_WIDTH - 1 : 0]              i_ptwATT
);

// TLB
reg [VPN_WIDTH - 1 : 0]              tlb_SVPN; // the start Virtual Page Number
reg [VPN_WIDTH - 1 : 0]              tlb_SVPN_END; // the end Virtual Page Number
reg [PPN_WIDTH - 1 : 0]              tlb_PPN; // Physical Page Number
reg [ATT_WIDTH - 1 : 0]              tlb_ATT; // Atrributes
reg                                  tlb_Val; // Valid

wire VPNeq = (i_VPN >= tlb_SVPN) & (i_VPN < tlb_SVPN_END);

assign o_hit   = i_lookup &  (&tlb_ATT) & VPNeq & tlb_Val;
assign o_fault = i_lookup & ~(&tlb_ATT) & VPNeq & tlb_Val;
//assign o_PPN   = {tlb_PPN + i_VPN - tlb_SVPN - 1}; //old RCPT
assign o_PPN   = {tlb_PPN + i_VPN - tlb_SVPN};

always @(posedge i_clk, negedge i_rst_n) begin
  if(i_rst_n == 1'b0) begin
     tlb_SVPN     <= {(VPN_WIDTH){1'b0}};
	 tlb_SVPN_END <= {(VPN_WIDTH){1'b0}};
	 tlb_PPN      <= {(PPN_WIDTH){1'b0}};
	 tlb_ATT      <= {(ATT_WIDTH){1'b0}};
	 tlb_Val      <= 1'b0;
  end else begin
     tlb_SVPN     <= (i_ptwUpdate == 1'b1) ? i_ptwVPN     : tlb_SVPN;
	 tlb_SVPN_END <= (i_ptwUpdate == 1'b1) ? i_ptwVPN_END : tlb_SVPN_END;
	 tlb_PPN      <= (i_ptwUpdate == 1'b1) ? i_ptwPPN     : tlb_PPN;
	 tlb_ATT      <= (i_ptwUpdate == 1'b1) ? i_ptwATT     : tlb_ATT;
	 tlb_Val      <= (i_ptwUpdate == 1'b1) ? 1'b1         : tlb_Val;
  end
end

endmodule