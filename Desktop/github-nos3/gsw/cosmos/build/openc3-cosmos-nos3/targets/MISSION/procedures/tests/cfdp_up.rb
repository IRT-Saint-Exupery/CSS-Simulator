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

# Delete old files for test
cmd("CFS_RADIO FM_DELETE with FILENAME '/data/tmp0.so'")
cmd("CFS_RADIO FM_DELETE with FILENAME '/data/tmp1.so'")
sleep(CFS_CMD_SLEEP)

# Loop until success
INIT_SUCCESS = tlm("CFS_RADIO CF_HKPACKET UP_SUCCESSCOUNTER")
LOOP_COUNT = 0
while (tlm("CFS_RADIO CF_HKPACKET UP_SUCCESSCOUNTER") <= INIT_SUCCESS)
    puts "Attempt " + LOOP_COUNT.to_s + " started"
    
    UP_META = tlm("CFS_RADIO CF_HKPACKET UP_METACOUNT")
    sleep(CFS_CMD_SLEEP)
    
    # Pick a SINGLE file to uplink 
    # Class 1
    #cmd("CFDP SEND_FILE with CLASS 1, DEST_ID '24', SRCFILENAME '/tmp/uplink/tmp0.so', DSTFILENAME '/data/tmp0.so'")
    cmd("CFDP SEND_FILE with CLASS 1, DEST_ID '24', SRCFILENAME '/tmp/uplink/tmp1.so', DSTFILENAME '/data/tmp1.so'")
    # Class 2
    #cmd("CFDP SEND_FILE with CLASS 2, DEST_ID '24', SRCFILENAME '/tmp/uplink/tmp0.so', DSTFILENAME '/data/tmp0.so'")
    #cmd("CFDP SEND_FILE with CLASS 2, DEST_ID '24', SRCFILENAME '/tmp/uplink/tmp1.so', DSTFILENAME '/data/tmp1.so'")
    sleep(CFS_CMD_SLEEP)
    
    # Wait for transfer to complete  
    wait_check("CFS_RADIO CF_HKPACKET UP_METACOUNT > #{UP_META}", 30)
    wait_check("CFS_RADIO CF_HKPACKET UP_UPLINKACTIVEQFILECNT == 0", 300)
    sleep(CFS_CMD_SLEEP)
    LOOP_COUNT = LOOP_COUNT + 1
end
  
puts "Success after " + LOOP_COUNT.to_s + " attempt(s)!"
