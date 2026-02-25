require 'cosmos'
require 'cosmos/script'
require "cfs_lib.rb"

#
# CFDP Up the Sample Files
#

# Note that this assumes both of the follow are populated:
#   Startup Script - /tmp/uplink/tmp0.so
#   Sample App - /tmp/uplink/tmp1.so

# Enable Radio
cmd("CFS_RADIO TO_ENABLE_OUTPUT with DEST_IP '192.168.100.8', DEST_PORT 5011")

# Confirm old files deleted
continue = ask "Could you confirm the old /tmp/data/tmp*.so files are deleted? Press 'Y' if so."
while continue != "y" and continue != "Y"
    continue = ask "Could you confirm the old /tmp/data/tmp*.so files are deleted? Press 'Y' if so."
end

# Loop until success
INIT_SUCCESS = tlm("CFS_RADIO CF_HKPACKET UP_SUCCESSCOUNTER")
LOOP_COUNT = 0
while (tlm("CFS_RADIO CF_HKPACKET CH0_SUCCESS_COUNTER") <= INIT_SUCCESS)
    puts "Attempt " + LOOP_COUNT.to_s + " started"
    
    DOWN_META = tlm("CFS_RADIO CF_HKPACKET CH0_ACTIVE_Q_FILECNT")
  
    # Pick a SINGLE file to downlink 
    # Class 1
    #cmd("CFS_RADIO CF_PLAYBACK_FILE with CLASS 1, CHAN 0, PRIORITY 1, PRESERVE KEEP, PEER_ENTITY_ID '0.21', SRCFILENAME '/data/tmp0.so', DSTFILENAME '/tmp/data/tmp0.so'")
    #cmd("CFS_RADIO CF_PLAYBACK_FILE with CLASS 1, CHAN 0, PRIORITY 1, PRESERVE KEEP, PEER_ENTITY_ID '0.21', SRCFILENAME '/data/tmp1.so', DSTFILENAME '/tmp/data/tmp1.so'")
    # Class 2
    #cmd("CFS_RADIO CF_PLAYBACK_FILE with CLASS 2, CHAN 0, PRIORITY 1, PRESERVE KEEP, PEER_ENTITY_ID '0.21', SRCFILENAME '/data/tmp0.so', DSTFILENAME '/tmp/data/tmp0.so'")
    cmd("CFS_RADIO CF_PLAYBACK_FILE with CLASS 2, CHAN 0, PRIORITY 1, PRESERVE KEEP, PEER_ENTITY_ID '0.21', SRCFILENAME '/data/tmp1.so', DSTFILENAME '/tmp/data/tmp1.so'")
    sleep(CFS_CMD_SLEEP)
  
    # Wait for transfer to complete  
    wait_check("CFS_RADIO CF_HKPACKET CH0_ACTIVE_Q_FILECNT == 0", 300)
    sleep(CFS_CMD_SLEEP)
    LOOP_COUNT = LOOP_COUNT + 1
  end
  
  puts "Success after " + LOOP_COUNT.to_s + " attempt(s)!"
  
