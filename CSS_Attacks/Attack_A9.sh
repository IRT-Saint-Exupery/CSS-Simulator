#!/bin/bash

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

/usr/bin/sshpass -p nos3123! ssh nos3@$IPMCS "touch /tmp/cookie"

while true; do 

file=$(/usr/bin/sshpass -p nos3123! ssh nos3@$IPMCS "find /tmp -name '*.jpg' -newer /tmp/cookie | head -n 1 " 2>/dev/null)

echo $file 

if [ "$file" != "" ]; then
break
else 
sleep 1
fi

done 

#xhost +

/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "firefox $file"

sleep 120 #TBD time to take the picture before scp.

/usr/bin/sshpass -p nos3123! scp nos3@$IPMCS:$file ~/
   
	
