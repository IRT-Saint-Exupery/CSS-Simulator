#!/bin/bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

# arducam_Mal.so has been compiled on SAT1

/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "rm /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so"
/usr/bin/sshpass -p nos3123! scp arducam_Legit.so nos3@$IPSAT1:/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_Legit.so    # for more realistic attack use CFDP TC (not working with NOS3 v1.6.2)
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "mv /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam_Legit.so /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/arducam.so"

python3 Attack_A13.py

## NB: reset nominal version of .so after the attack !


    
    





