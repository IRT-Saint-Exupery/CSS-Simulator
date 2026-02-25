#!/bin/bash

# this shell will adapt the IP configuration of the scenario according to the IP defined in IP_Scenario.sh

source IP_Scenario.sh 

if [ "$IP_LOCAL_ADDR" != "$IPSAT1" ]; then
echo "#################### $0 Shoud be called from the main VM - SAT1 #################"
exit
fi

./Init_Vm_changes.sh

echo "Adapt IP Address for the following folders :"

cd /home/nos3/Install_tools_nos3/Specialize_42_InOut
echo `pwd`
export files=`fgrep -l -r 192.168.100.11`; for file in "${files[@]}"; do sed -i 's/192.168.100.11/'$IPSAT1'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.12`; for file in "${files[@]}"; do sed -i 's/192.168.100.12/'$IPSAT2'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.13`; for file in "${files[@]}"; do sed -i 's/192.168.100.13/'$IPSAT3'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.14`; for file in "${files[@]}"; do sed -i 's/192.168.100.14/'$IPSAT4'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.15`; for file in "${files[@]}"; do sed -i 's/192.168.100.15/'$IPSAT5'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.16`; for file in "${files[@]}"; do sed -i 's/192.168.100.16/'$IPSAT6'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.17`; for file in "${files[@]}"; do sed -i 's/192.168.100.17/'$IPSAT7'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.42`; for file in "${files[@]}"; do sed -i 's/192.168.100.42/'$IP42'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.5`;  for file in "${files[@]}"; do sed -i 's/192.168.100.5/'$IPMCS'/g' $file; done 2>/dev/null

# IP configuration 
cd ~/Desktop/github-nos3
echo `pwd`
export files=`fgrep -l -r 192.168.100.11`; for file in "${files[@]}"; do sed -i 's/192.168.100.11/'$IPSAT1'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.12`; for file in "${files[@]}"; do sed -i 's/192.168.100.12/'$IPSAT2'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.13`; for file in "${files[@]}"; do sed -i 's/192.168.100.13/'$IPSAT3'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.14`; for file in "${files[@]}"; do sed -i 's/192.168.100.14/'$IPSAT4'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.15`; for file in "${files[@]}"; do sed -i 's/192.168.100.15/'$IPSAT5'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.16`; for file in "${files[@]}"; do sed -i 's/192.168.100.16/'$IPSAT6'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.17`; for file in "${files[@]}"; do sed -i 's/192.168.100.17/'$IPSAT7'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.42`; for file in "${files[@]}"; do sed -i 's/192.168.100.42/'$IP42'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.5`;  for file in "${files[@]}"; do sed -i 's/192.168.100.5/'$IPMCS'/g' $file; done 2>/dev/null

cd /opt/nos3
echo `pwd`
export files=`fgrep -l -r 192.168.100.11`; for file in "${files[@]}"; do sed -i 's/192.168.100.11/'$IPSAT1'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.12`; for file in "${files[@]}"; do sed -i 's/192.168.100.12/'$IPSAT2'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.13`; for file in "${files[@]}"; do sed -i 's/192.168.100.13/'$IPSAT3'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.14`; for file in "${files[@]}"; do sed -i 's/192.168.100.14/'$IPSAT4'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.15`; for file in "${files[@]}"; do sed -i 's/192.168.100.15/'$IPSAT5'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.16`; for file in "${files[@]}"; do sed -i 's/192.168.100.16/'$IPSAT6'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.17`; for file in "${files[@]}"; do sed -i 's/192.168.100.17/'$IPSAT7'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.42`; for file in "${files[@]}"; do sed -i 's/192.168.100.42/'$IP42'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.5`;  for file in "${files[@]}"; do sed -i 's/192.168.100.5/'$IPMCS'/g' $file; done 2>/dev/null

cd ~/eclipse-workspace/frontend/config
echo `pwd`
export files=`fgrep -l -r 192.168.100.11`; for file in "${files[@]}"; do sed -i 's/192.168.100.11/'$IPSAT1'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.12`; for file in "${files[@]}"; do sed -i 's/192.168.100.12/'$IPSAT2'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.13`; for file in "${files[@]}"; do sed -i 's/192.168.100.13/'$IPSAT3'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.14`; for file in "${files[@]}"; do sed -i 's/192.168.100.14/'$IPSAT4'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.15`; for file in "${files[@]}"; do sed -i 's/192.168.100.15/'$IPSAT5'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.16`; for file in "${files[@]}"; do sed -i 's/192.168.100.16/'$IPSAT6'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.17`; for file in "${files[@]}"; do sed -i 's/192.168.100.17/'$IPSAT7'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.42`; for file in "${files[@]}"; do sed -i 's/192.168.100.42/'$IP42'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.5`;  for file in "${files[@]}"; do sed -i 's/192.168.100.5/'$IPMCS'/g' $file; done 2>/dev/null

cd ~/CSS_Attacks
echo `pwd`
export files=`fgrep -l -r 192.168.100.11`; for file in "${files[@]}"; do sed -i 's/192.168.100.11/'$IPSAT1'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.12`; for file in "${files[@]}"; do sed -i 's/192.168.100.12/'$IPSAT2'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.13`; for file in "${files[@]}"; do sed -i 's/192.168.100.13/'$IPSAT3'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.14`; for file in "${files[@]}"; do sed -i 's/192.168.100.14/'$IPSAT4'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.15`; for file in "${files[@]}"; do sed -i 's/192.168.100.15/'$IPSAT5'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.16`; for file in "${files[@]}"; do sed -i 's/192.168.100.16/'$IPSAT6'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.17`; for file in "${files[@]}"; do sed -i 's/192.168.100.17/'$IPSAT7'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.42`; for file in "${files[@]}"; do sed -i 's/192.168.100.42/'$IP42'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.5`;  for file in "${files[@]}"; do sed -i 's/192.168.100.5/'$IPMCS'/g' $file; done 2>/dev/null

cd ~/Input_Generator
echo `pwd`
export files=`fgrep -l -r 192.168.100.11`; for file in "${files[@]}"; do sed -i 's/192.168.100.11/'$IPSAT1'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.12`; for file in "${files[@]}"; do sed -i 's/192.168.100.12/'$IPSAT2'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.13`; for file in "${files[@]}"; do sed -i 's/192.168.100.13/'$IPSAT3'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.14`; for file in "${files[@]}"; do sed -i 's/192.168.100.14/'$IPSAT4'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.15`; for file in "${files[@]}"; do sed -i 's/192.168.100.15/'$IPSAT5'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.16`; for file in "${files[@]}"; do sed -i 's/192.168.100.16/'$IPSAT6'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.17`; for file in "${files[@]}"; do sed -i 's/192.168.100.17/'$IPSAT7'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.42`; for file in "${files[@]}"; do sed -i 's/192.168.100.42/'$IP42'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.5`;  for file in "${files[@]}"; do sed -i 's/192.168.100.5/'$IPMCS'/g' $file; done 2>/dev/null

cd ~/Scenario_Manager
echo `pwd`
export files=`fgrep -l -r 192.168.100.11`; for file in "${files[@]}"; do sed -i 's/192.168.100.11/'$IPSAT1'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.12`; for file in "${files[@]}"; do sed -i 's/192.168.100.12/'$IPSAT2'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.13`; for file in "${files[@]}"; do sed -i 's/192.168.100.13/'$IPSAT3'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.14`; for file in "${files[@]}"; do sed -i 's/192.168.100.14/'$IPSAT4'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.15`; for file in "${files[@]}"; do sed -i 's/192.168.100.15/'$IPSAT5'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.16`; for file in "${files[@]}"; do sed -i 's/192.168.100.16/'$IPSAT6'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.17`; for file in "${files[@]}"; do sed -i 's/192.168.100.17/'$IPSAT7'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.42`; for file in "${files[@]}"; do sed -i 's/192.168.100.42/'$IP42'/g' $file; done 2>/dev/null
export files=`fgrep -l -r 192.168.100.5`;  for file in "${files[@]}"; do sed -i 's/192.168.100.5/'$IPMCS'/g' $file; done 2>/dev/null

echo "END"

#### TODO
#### add IP adaptation also for ISL component : SAT_IP_ADR_FMT "192.168.100.%d" 
#### be careful with ISL, see 'ISL_IpFromSatId' function (not adapted to generic IP in this version).
