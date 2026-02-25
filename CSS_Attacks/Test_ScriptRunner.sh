#!/bin/bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "export DISPLAY=:1; cd ~/CSS_Attacks; python3 Script_Runner_Agent.py" &
