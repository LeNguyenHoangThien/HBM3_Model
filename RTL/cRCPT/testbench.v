`timescale 1ns/1ps

module testbench;

  // Parameters
  parameter ADDR_WIDTH  = 35;
  parameter TLB_ENTRIES = 32;
  parameter VPN_WIDTH   = 23;
  parameter PPN_WIDTH   = VPN_WIDTH;
  parameter ATT_WIDTH   = 10;
  parameter BSIZE_WIDTH = 9;
  parameter PTE_LOG2    = 1;
  parameter HALF        = 5;
  parameter CYCLE       = 2 * HALF;

  // Signals
  reg i_clk;
  reg i_rst_n;
  reg i_lookup;
  reg [ADDR_WIDTH - 1 : 0] i_virtualAddr;
  wire o_hit;
  wire o_fault;
  wire [ADDR_WIDTH - 1 : 0] o_physicalAddr;
  wire o_busy;

  reg i_ptwUpdate;
  reg [63 : 0] i_ptwPTE;
  reg [4 : 0] i_indexVictim;

  // Instantiate MMURCPT
  MMURCPT #(
    .ADDR_WIDTH(ADDR_WIDTH),
    .TLB_ENTRIES(TLB_ENTRIES),
    .VPN_WIDTH(VPN_WIDTH),
    .PPN_WIDTH(PPN_WIDTH),
    .ATT_WIDTH(ATT_WIDTH),
    .BSIZE_WIDTH(BSIZE_WIDTH),
    .PTE_LOG2(PTE_LOG2)
  ) dut (
    .i_clk(i_clk),
    .i_rst_n(i_rst_n),
    .i_lookup(i_lookup),
    .i_virtualAddr(i_virtualAddr),
    .o_hit(o_hit),
    .o_fault(o_fault),
    .o_physicalAddr(o_physicalAddr),
    .o_busy(o_busy),
    .i_ptwUpdate(i_ptwUpdate),
    .i_ptwPTE(i_ptwPTE),
    .i_indexVictim(i_indexVictim)
  );

  // Clock generation
  always #(HALF) i_clk = ~i_clk;

  // Test sequence
  initial begin
    // Initialize signals
    i_clk = 0;
    i_rst_n = 0;
    i_lookup = 0;
    i_virtualAddr = 0;
    i_ptwUpdate = 0;
    i_ptwPTE = 0;
    i_indexVictim = 0;

    // Reset sequence
    #(CYCLE) i_rst_n = 1;

    // Lookup a virtual address (no page table update yet)
    #(CYCLE) i_lookup = 1;
    i_virtualAddr = 36'h0000_1234;
    #(CYCLE);// i_lookup = 0;

    // Simulate a page table update
    #(CYCLE * 2) i_ptwUpdate = 1;
    i_ptwPTE = 64'h0000_0004_4321_03ff; // Example PTE
    i_indexVictim = 5'd0; // Example index
	@(posedge i_clk);
	@(negedge i_clk);
	i_ptwPTE = 64'h0022_4704_3524_0083; // Example PTE
    i_indexVictim = 5'd1; // Example index
	@(posedge i_clk);
	@(negedge i_clk);
	//i_ptwPTE = 64'h0022_4704_3524_0083; // Example PTE
    i_indexVictim = 5'd2; // Example index
	@(posedge i_clk);
	@(negedge i_clk);
    i_ptwUpdate = 0;

	while(o_hit == 1'b0) begin
	end
	@(posedge i_clk);
	@(negedge i_clk);
	i_lookup = 0;
	#(CYCLE * 5);

    // Lookup again after page table update
    #(CYCLE) i_lookup = 1;
    i_virtualAddr = 36'h0000_6234;
	
//	while(o_hit == 1'b0) begin
//	end
	@(posedge i_clk);
	@(negedge i_clk);
	i_lookup = 0;
	#(CYCLE * 5);
	
// Lookup a virtual address (no page table update yet)
    #(CYCLE) i_lookup = 1;
    i_virtualAddr = 36'h0000_F89A;

    // Simulate a page table update
    #(CYCLE * 2) i_ptwUpdate = 1;
    i_ptwPTE = 64'h0000_0006_7654_03ff; // Example PTE
    i_indexVictim = 5'd4; // Example index
	@(posedge i_clk);
	@(negedge i_clk);
	i_ptwPTE = 64'b1_1_000110100_1011110101010100_0101_0100110_1010110101010000_0010_0001_1_1; // Example PTE
    i_indexVictim = 5'd5; // Example index
	@(posedge i_clk);
	@(negedge i_clk);
	//i_ptwPTE = 64'h0022_4704_3524_0083; // Example PTE
    i_indexVictim = 5'd6; // Example index
	@(posedge i_clk);
	@(negedge i_clk);
	i_indexVictim = 5'd7; // Example index
	@(posedge i_clk);
    @(negedge i_clk);
    i_ptwUpdate = 0;

	while(o_hit == 1'b0) begin
	end
	@(posedge i_clk);
	@(negedge i_clk);
	i_lookup = 0;
	#(CYCLE * 5);	

    // End simulation
    #(CYCLE * 5) $finish;
  end

  // Monitor output
  initial begin
    $monitor("Time=%0t | VA=%h | Hit=%b | Fault=%b | PA=%h | Busy=%b", $time, i_virtualAddr, o_hit, o_fault, o_physicalAddr, o_busy);
  end

endmodule