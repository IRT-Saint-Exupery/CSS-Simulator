# Verify Class 1 uplink capabilities
require 'cosmos'
require 'cosmos/script'

# Prompt user for file name/location
prompt("A file dialog window will open. Please select the file to upload for class 1 test.")
selected_file = open_file_dialog()
status_bar(selected_file)
# Extract file name without rest of system path
file_name = selected_file.split('/')[-1].strip

# Obtain starting number of successful transactions
start_eng_successtrans = tlm("CFDP CFDP_ENGINE_HK ENG_UP_SUCCESSCOUNTER")

# Start class 1 upload
cmd("CFDP SEND_FILE with CLASS 1, DEST_ID '24', SRCFILENAME '#{selected_file}', DSTFILENAME '/cf/#{file_name}'")
status_bar("Uploading file...")

# Verify class 1 upload
wait_check("CFDP CFDP_ENGINE_HK ENG_UP_SUCCESSCOUNTER > #{start_eng_successtrans}", 30)

prompt("The transaction should be complete. Before pressing okay, please manually verify the uploaded file matches the original.")

