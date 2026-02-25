#!/usr/bin/env bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

if [ $NBSAT -ge 2 ]; 
 then 
 /usr/bin/sshpass -p nos3123! scp /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Sim.txt nos3@$IPSAT2:/home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Sim.txt;
 /usr/bin/sshpass -p nos3123! scp /home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml nos3@$IPSAT2:/home/nos3/Desktop/github-nos3/sims/cfg/nos3-simulator.xml;
fi

