# Enable telemetry streaming over UDP
require 'cosmos'
require 'cosmos/script'

cmd("CFS TO_ENABLE_OUTPUT with DEST_IP '192.168.100.8', DEST_PORT 5011")
wait_check_packet("CFS", "CFE_ES_HKPACKET", 1 , 10)
