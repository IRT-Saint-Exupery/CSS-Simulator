#/bin/bash

#Set NBSAT
source $HOME/Install_tools_nos3/IP_Scenario.sh

/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "cd ~/Desktop/github-nos3; make stop" &

if [ $NBSAT -ge 2 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT2 "cd ~/Desktop/github-nos3; make stop" & fi 
if [ $NBSAT -ge 3 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT3 "cd ~/Desktop/github-nos3; make stop" & fi 
if [ $NBSAT -ge 4 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT4 "cd ~/Desktop/github-nos3; make stop" & fi  
if [ $NBSAT -ge 5 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT5 "cd ~/Desktop/github-nos3; make stop" & fi  
if [ $NBSAT -ge 6 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT6 "cd ~/Desktop/github-nos3; make stop" & fi  
if [ $NBSAT -ge 7 ]; then /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT7 "cd ~/Desktop/github-nos3; make stop" & fi  

/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IP42 "cd ~/Desktop/github-nos3; make stop" &
sudo -s make stop &
