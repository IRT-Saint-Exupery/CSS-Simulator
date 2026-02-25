#/bin/bash

# Adapt NBSAT
source IP_Scenario.sh

if [ "$IP_LOCAL_ADDR" != "$IPSAT1" ]; then
echo "#################### $0 Shoud be called from the main VM - SAT1 #################"
exit
fi

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

nom1=Inp_Cmd.txt.1SAT; 	cp $DIR/Specialize_42_InOut/${nom1//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Cmd.txt;
nom1=Inp_Sim.txt.1SAT; 	cp $DIR/Specialize_42_InOut/${nom1//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Sim.txt;
nom1=Inp_IPC.txt.1SAT; 	cp $DIR/Specialize_42_InOut/${nom1//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_IPC.txt;

nom1=Inp_IPC.txt.TX.1SAT; 	cp $DIR/Specialize_42_InOut/${nom1//1/$NBSAT} /opt/nos3/42/Tx/Inp_IPC.txt;
nom1=Inp_IPC.txt.RX.1SAT; 	cp $DIR/Specialize_42_InOut/${nom1//1/$NBSAT} /opt/nos3/42/Rx/Inp_IPC.txt;

nom1=MakefileMCS.1SAT; 	cp $DIR/Specialize_42_InOut/${nom1//1/$NBSAT} ~/Install_tools_nos3/MakefileMCS;

touch IP_Scenario.sh
