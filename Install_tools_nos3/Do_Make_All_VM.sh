#!/bin/bash

#set NBSAT
source /home/nos3/Install_tools_nos3/IP_Scenario.sh

if [ "$IP_LOCAL_ADDR" != "$IPSAT1" ]; then
echo "#################### $0 Shoud be called from the main VM - SAT1 #################"
exit
fi

# local make 
cd ~/Desktop/github-nos3
make -j 6 &

# remote make 
if [ $NBSAT -ge 2 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT2 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make -j 6" & fi
if [ $NBSAT -ge 3 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT3 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make -j 6" & fi
if [ $NBSAT -ge 4 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT4 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make -j 6" & fi
if [ $NBSAT -ge 5 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT5 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make -j 6" & fi
if [ $NBSAT -ge 6 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT6 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make -j 6" & fi
if [ $NBSAT -ge 7 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT7 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make -j 6" & fi
if [ $NBSAT -ge 1 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IP42 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make -j 6"  & fi
if [ $NBSAT -ge 1 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "export DISPLAY=:0; cd ~/eclipse-workspace/frontend/build/default; gnome-terminal -- make -j 6"  & fi
#/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make" & >/dev/null 2>&1

