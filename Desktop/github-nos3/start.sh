#/bin/bash

#Set NBSAT
source $HOME/Install_tools_nos3/IP_Scenario.sh

if [ "$IP_LOCAL_ADDR" != "$IPSAT1" ]; then
echo "#################### $0 Shoud be called from the main VM - SAT1 #################"
exit
fi

export DISPLAY=:0

## launch cosmos containers and cryptolib on MCS
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make launch" &

## launch 42 with InOut folder 
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IP42 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make launch" &
#/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IP42 "export DISPLAY=:0; cd ~/Desktop/github-nos3; make launch" &

## launch components and FSW on main SAT
#/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT1 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make launch" &
gnome-terminal -- make launch &
#make launch &

sleep 8 
if [ $NBSAT -ge 2 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT2 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make launch" & fi
sleep 8
if [ $NBSAT -ge 3 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT3 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make launch" & fi
sleep 8
if [ $NBSAT -ge 4 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT4 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make launch" & fi
sleep 8
if [ $NBSAT -ge 5 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT5 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make launch" & fi
sleep 8
if [ $NBSAT -ge 6 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT6 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make launch" & fi
sleep 8
if [ $NBSAT -ge 7 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT7 "export DISPLAY=:0; cd ~/Desktop/github-nos3; gnome-terminal -- make launch" & fi



