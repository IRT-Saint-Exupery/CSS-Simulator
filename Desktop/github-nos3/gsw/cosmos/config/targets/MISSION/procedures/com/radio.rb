require 'cosmos'
require 'cosmos/script'
require "cfs_lib.rb"

# Enable radio interface
cmd("CFS_RADIO TO_ENABLE_OUTPUT with DEST_IP '192.168.100.8', DEST_PORT 5011")

# Check debug aliveness
get_sc_hk()
