library verilog;
use verilog.vl_types.all;
entity testbench is
    generic(
        ADDR_WIDTH      : integer := 35;
        TLB_ENTRIES     : integer := 32;
        VPN_WIDTH       : integer := 23;
        PPN_WIDTH       : vl_notype;
        ATT_WIDTH       : integer := 10;
        BSIZE_WIDTH     : integer := 9;
        PTE_LOG2        : integer := 1;
        HALF            : integer := 5;
        CYCLE           : vl_notype
    );
    attribute mti_svvh_generic_type : integer;
    attribute mti_svvh_generic_type of ADDR_WIDTH : constant is 1;
    attribute mti_svvh_generic_type of TLB_ENTRIES : constant is 1;
    attribute mti_svvh_generic_type of VPN_WIDTH : constant is 1;
    attribute mti_svvh_generic_type of PPN_WIDTH : constant is 3;
    attribute mti_svvh_generic_type of ATT_WIDTH : constant is 1;
    attribute mti_svvh_generic_type of BSIZE_WIDTH : constant is 1;
    attribute mti_svvh_generic_type of PTE_LOG2 : constant is 1;
    attribute mti_svvh_generic_type of HALF : constant is 1;
    attribute mti_svvh_generic_type of CYCLE : constant is 3;
end testbench;
