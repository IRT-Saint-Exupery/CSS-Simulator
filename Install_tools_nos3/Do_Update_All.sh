#!/bin/bash

source IP_Scenario.sh

if [ "$IP_LOCAL_ADDR" != "$IPSAT1" ]; then
echo "#################### $0 Shoud be called from the main VM - SAT1 #################"
exit
fi

if [ $NBSAT -ge 2 ]; then 
	if ping -c 1 "$IPSAT2" &> /dev/null
	then 
		echo "########## UPDATE SAT2 #########"
		./update1Sat.sh 1 2 $IPSAT1 $IPSAT2
	fi
fi

if [ $NBSAT -ge 3 ]; then 
	if ping -c 1 "$IPSAT3" &> /dev/null
	then
		echo "########## UPDATE SAT3 #########"
		./update1Sat.sh 1 3 $IPSAT1 $IPSAT3
	fi
fi

if [ $NBSAT -ge 4 ]; then 
	if ping -c 1 "$IPSAT4" &> /dev/null
	then
		echo "########## UPDATE SAT4 #########"
		./update1Sat.sh 1 4 $IPSAT1 $IPSAT4
	fi
fi

if [ $NBSAT -ge 5 ]; then 
	if ping -c 1 "$IPSAT5" &> /dev/null
	then
		echo "########## UPDATE SAT5 #########"
		./update1Sat.sh 1 5 $IPSAT1 $IPSAT5
	fi
fi

if [ $NBSAT -ge 6 ]; then 
	if ping -c 1 "$IPSAT6" &> /dev/null
	then
		echo "########## UPDATE SAT6 #########"
		./update1Sat.sh 1 6 $IPSAT1 $IPSAT6
	fi
fi

if [ $NBSAT -ge 7 ]; then 
	if ping -c 1 "$IPSAT7" &> /dev/null
	then
		echo "########## UPDATE SAT7 #########"
		./update1Sat.sh 1 7 $IPSAT1 $IPSAT7
	fi
fi

echo "########## UPDATE MCS #########"
./updateMCS.sh $IPMCS
echo "########## UPDATE 42 #########"
./update42.sh $IP42
