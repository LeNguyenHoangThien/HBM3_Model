library verilog;
use verilog.vl_types.all;
entity MMURCPT is
    generic(
        ADDR_WIDTH      : integer := 35;
        TLB_ENTRIES     : integer := 32;
        VPN_WIDTH       : integer := 23;
        PPN_WIDTH       : vl_notype;
        ATT_WIDTH       : integer := 10;
        BSIZE_WIDTH     : integer := 9;
        PTE_LOG2        : integer := 1
    );
    port(
        i_clk           : in     vl_logic;
        i_rst_n         : in     vl_logic;
        i_lookup        : in     vl_logic;
        i_virtualAddr   : in     vl_logic_vector;
        o_hit           : out    vl_logic;
        o_fault         : out    vl_logic;
        o_physicalAddr  : out    vl_logic_vector;
        o_busy          : out    vl_logic;
        i_ptwUpdate     : in     vl_logic;
        i_ptwPTE        : in     vl_logic_vector(63 downto 0);
        i_indexVictim   : in     vl_logic_vector(4 downto 0)
    );
    attribute mti_svvh_generic_type : integer;
    attribute mti_svvh_generic_type of ADDR_WIDTH : constant is 1;
    attribute mti_svvh_generic_type of TLB_ENTRIES : constant is 1;
    attribute mti_svvh_generic_type of VPN_WIDTH : constant is 1;
    attribute mti_svvh_generic_type of PPN_WIDTH : constant is 3;
    attribute mti_svvh_generic_type of ATT_WIDTH : constant is 1;
    attribute mti_svvh_generic_type of BSIZE_WIDTH : constant is 1;
    attribute mti_svvh_generic_type of PTE_LOG2 : constant is 1;
end MMURCPT;
