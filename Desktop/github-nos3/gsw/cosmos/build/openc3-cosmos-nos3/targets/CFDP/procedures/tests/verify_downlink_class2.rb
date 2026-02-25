# Verify Class 2 Downlink Capabilities
require 'cosmos'
require 'cosmos/script'

# Prompt user for file name/location
fsw_file_name = ask("Please enter the fsw path to the file to downlink via Class 2. e.g. '/cf/cfe_es_startup.scr'")
# Extract file name without rest of path
file_name = fsw_file_name.split('/')[-1].strip

# Alert the user to the destination file name
dst_file_name = "/tmp/" + file_name
prompt("Once completed, the file should be located on your machine at #{dst_file_name}. Press Ok to begin downlink.")

# Obtain starting number of successful transactions
start_eng_successtrans = tlm("CFDP CFDP_ENGINE_HK ENG_DOWN_SUCCESSDOWNLINKS")

# Start class 2 download
cmd("CFS CF_PLAYBACK_FILE with CLASS 2, CHAN 0, PRIORITY 1, PRESERVE 1, PEER_ENTITY_ID '0.21', SRCFILENAME #{fsw_file_name}, DSTFILENAME '/tmp/#{file_name}'")
status_bar("Downloading file...")

# Verify class 2 upload
wait_check("CFDP CFDP_ENGINE_HK ENG_DOWN_SUCCESSDOWNLINKS > #{start_eng_successtrans}", 60)

status_bar("Download complete!")
prompt("The transaction should be complete. Before pressing okay, please manually verify the downloaded file matches the original.")

