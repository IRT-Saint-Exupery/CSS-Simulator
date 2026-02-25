#!/bin/bash

# diffuse_nose3_time.sh <argument> 
# $1 = delta to apply in sec from the init defined in Inp_Sim.txt and nos3-simulator.xml

source /home/nos3/Install_tools_nos3/IP_Scenario.sh

if [ "$IP_LOCAL_ADDR" != "$IPSAT1" ]; then
echo "#################### $0 Shoud be called from the main VM - SAT1 #################"
exit
fi

if [ $# -eq 0 ]; then
    echo "No argument provided for $0, using delta=0"
    DELTA=0
else
    DELTA="$1"
fi

#call for 42
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IP42 "export DISPLAY=:0; . /home/nos3/Desktop/github-nos3/gsw/scripts/update_nos3_time.sh $DELTA "

#call for MCS
/usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPMCS "export DISPLAY=:0; . /home/nos3/Desktop/github-nos3/gsw/scripts/update_nos3_time.sh $DELTA "

#call for SAT1
. /home/nos3/Desktop/github-nos3/gsw/scripts/update_nos3_time.sh $DELTA

# call for other SAT
if [ $NBSAT -ge 2 ]; then 
	if ping -c 1 "$IPSAT2" &> /dev/null
	then 
		echo "########## UPDATE TIME SAT2 #########"
	        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT2 "export DISPLAY=:0; . /home/nos3/Desktop/github-nos3/gsw/scripts/update_nos3_time.sh $DELTA " &
	fi
fi

if [ $NBSAT -ge 3 ]; then 
	if ping -c 1 "$IPSAT3" &> /dev/null
	then 
		echo "########## UPDATE TIME SAT3 #########"
	        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT3 "export DISPLAY=:0; . /home/nos3/Desktop/github-nos3/gsw/scripts/update_nos3_time.sh $DELTA " &
	fi
fi

if [ $NBSAT -ge 4 ]; then 
	if ping -c 1 "$IPSAT4" &> /dev/null
	then 
		echo "########## UPDATE TIME SAT4 #########"
	        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT4 "export DISPLAY=:0; . /home/nos3/Desktop/github-nos3/gsw/scripts/update_nos3_time.sh $DELTA " &
	fi
fi

if [ $NBSAT -ge 5 ]; then 
	if ping -c 1 "$IPSAT5" &> /dev/null
	then 
		echo "########## UPDATE TIME SAT5 #########"
	        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT5 "export DISPLAY=:0; . /home/nos3/Desktop/github-nos3/gsw/scripts/update_nos3_time.sh $DELTA " &
	fi
fi

if [ $NBSAT -ge 6 ]; then 
	if ping -c 1 "$IPSAT6" &> /dev/null
	then 
		echo "########## UPDATE TIME SAT6 #########"
	        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT6 "export DISPLAY=:0; . /home/nos3/Desktop/github-nos3/gsw/scripts/update_nos3_time.sh $DELTA " &
	fi
fi

if [ $NBSAT -ge 7 ]; then 
	if ping -c 1 "$IPSAT7" &> /dev/null
	then 
		echo "########## UPDATE TIME SAT7 #########"
	        /usr/bin/sshpass -p nos3123! ssh -XY nos3@$IPSAT7 "export DISPLAY=:0; . /home/nos3/Desktop/github-nos3/gsw/scripts/update_nos3_time.sh $DELTA " &
	fi
fi
