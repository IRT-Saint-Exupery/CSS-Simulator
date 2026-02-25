require 'cosmos'
require 'cosmos/script'
require "cfs_lib.rb"

# Enable debug interface
cmd("TO_DEBUG TO_DEBUG_ENABLE_OUTPUT_CC with DEST_IP '192.168.100.8', DEST_PORT 5013")

# Check debug aliveness
get_debug_sc_hk()
