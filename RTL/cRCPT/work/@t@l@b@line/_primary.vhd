library verilog;
use verilog.vl_types.all;
entity TLBLine is
    generic(
        ADDR_WIDTH      : integer := 35;
        VPN_WIDTH       : integer := 23;
        PPN_WIDTH       : vl_notype;
        ATT_WIDTH       : integer := 10
    );
    port(
        i_clk           : in     vl_logic;
        i_rst_n         : in     vl_logic;
        i_lookup        : in     vl_logic;
        i_VPN           : in     vl_logic_vector;
        o_PPN           : out    vl_logic_vector;
        o_hit           : out    vl_logic;
        o_fault         : out    vl_logic;
        tlb_SVPN        : out    vl_logic_vector;
        i_ptwUpdate     : in     vl_logic;
        i_ptwVPN        : in     vl_logic_vector;
        i_ptwVPN_END    : in     vl_logic_vector;
        i_ptwPPN        : in     vl_logic_vector;
        i_ptwATT        : in     vl_logic_vector
    );
    attribute mti_svvh_generic_type : integer;
    attribute mti_svvh_generic_type of ADDR_WIDTH : constant is 1;
    attribute mti_svvh_generic_type of VPN_WIDTH : constant is 1;
    attribute mti_svvh_generic_type of PPN_WIDTH : constant is 3;
    attribute mti_svvh_generic_type of ATT_WIDTH : constant is 1;
end TLBLine;
