#!/bin/bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

var="sed -i 's/ATTACK_16[ \t]0/ATTACK_16\t1/g' /home/nos3/eclipse-workspace/frontend/config/flagsConfig.txt"

/usr/bin/sshpass -p nos3123! ssh nos3@$IPMCS $var


/usr/bin/sshpass -p nos3123! ssh nos3@$IPMCS pkill -usr1 frontEnd

var="sed -i 's/ATTACK_16[ \t]1/ATTACK_16\t0/g' /home/nos3/eclipse-workspace/frontend/config/flagsConfig.txt"

/usr/bin/sshpass -p nos3123! ssh nos3@$IPMCS $var
