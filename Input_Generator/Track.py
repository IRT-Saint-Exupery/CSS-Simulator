import subprocess
import time
import sys
import json
import os 

if len(sys.argv) != 4:
    print(f"Usage: python {sys.argv[0]} <port>")
    sys.exit(1)

port = sys.argv[1]

scriptdir = os.path.dirname(os.path.abspath(__file__))
# File path of your JSON file
json_file = os.path.join(scriptdir, "scenarios", "adcs_target.json")

# New latitude and longitude values 
lati = sys.argv[2]
longi = sys.argv[3]

# Read the existing JSON data
with open(json_file, "r") as f:
    data = json.load(f)

# Modify the latitude and longitude fields
data["command1"]["GENERIC_ADCS_SET_TARGET_CC"]["LATITUDE"] = str(lati)
data["command1"]["GENERIC_ADCS_SET_TARGET_CC"]["LONGITUDE"] = str(longi)

# Write the updated data back to the same file (or a new file if preferred)
with open(json_file, "w") as f:
    json.dump(data, f, indent=4)

# set ADCS Target
subprocess.run(["python3", "Main.py", "send", "scenarios/adcs_target.json", "--port", port],cwd="/home/nos3/Input_Generator/")
# wait for target to be in sight  
time.sleep(3)

