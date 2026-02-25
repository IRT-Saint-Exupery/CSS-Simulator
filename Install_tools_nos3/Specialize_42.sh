#!/bin/bash

if [ $# -ne 1 ]; then
  echo "*********************** Specialize_42.sh needs one argument  ************************"
  exit
fi

#Set NBSAT
source /home/nos3/Install_tools_nos3/IP_Scenario.sh

# copy from .42 to .42 of the right file for constellation configuration 
nom=Inp_Cmd.txt.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$1 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Cmd.txt" &

nom=Inp_IPC.txt.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$1 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_IPC.txt" &

nom=Inp_Sim.txt.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$1 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Sim.txt" &
#init to modify the time 
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$1 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Sim_init.txt" &

nom=Inp_IPC.txt.TX.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$1 "export DISPLAY=:0; sudo cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /opt/nos3/42/Tx/Inp_IPC.txt" &

nom=Inp_IPC.txt.RX.1SAT
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$1 "export DISPLAY=:0; sudo cp /home/nos3/Install_tools_nos3/Specialize_42_InOut/${nom//1/$NBSAT} /opt/nos3/42/Rx/Inp_IPC.txt" &




/usr/bin/sshpass -p nos3123! ssh -XY nos3@$1 "export DISPLAY=:0; sudo cp -r /home/nos3/Desktop/github-nos3/sims/cfg/InOut/* /opt/nos3/42/NOS3InOut/" &

# specialize makefile and launch
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$1 "export DISPLAY=:0; cp /home/nos3/Install_tools_nos3/Makefile42 /home/nos3/Desktop/github-nos3/Makefile" &


