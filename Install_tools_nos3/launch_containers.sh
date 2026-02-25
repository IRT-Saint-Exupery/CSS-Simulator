#!/bin/bash

source IP_Scenario.sh

if ping -c 1 "$IPSAT1" &> /dev/null 
then
	if [ $NBSAT -ge 1 ]; then 
	echo "########## LAUNCH COSMOS SAT1 #########"
	cd /opt/nos3/cosmos1
	./openc3.sh cleanup
	./openc3.sh run
	fi
fi

if ping -c 1 "$IPSAT2" &> /dev/null
then 
	if [ $NBSAT -ge 2 ]; then 
	echo "########## LAUNCH COSMOS SAT2 #########"
	cd /opt/nos3/cosmos2
	./openc3.sh cleanup
	./openc3.sh run
	fi
fi

if ping -c 1 "$IPSAT3" &> /dev/null
then 
	if [ $NBSAT -ge 3 ]; then 
	echo "########## LAUNCH COSMOS SAT3 #########"
	cd /opt/nos3/cosmos3
	./openc3.sh cleanup
	./openc3.sh run
	fi
fi

if ping -c 1 "$IPSAT4" &> /dev/null
then 
	if [ $NBSAT -ge 4 ]; then 
	echo "########## LAUNCH COSMOS SAT4 #########"
	cd /opt/nos3/cosmos4
	./openc3.sh cleanup
	./openc3.sh run
	fi
fi

if ping -c 1 "$IPSAT5" &> /dev/null
then 
	if [ $NBSAT -ge 5 ]; then 
	echo "########## LAUNCH COSMOS SAT5 #########"
	cd /opt/nos3/cosmos5
	./openc3.sh cleanup
	./openc3.sh run
	fi
fi

if ping -c 1 "$IPSAT6" &> /dev/null
then 
	if [ $NBSAT -ge 6 ]; then 
	echo "########## LAUNCH COSMOS SAT6 #########"
	cd /opt/nos3/cosmos6
	./openc3.sh cleanup
	./openc3.sh run
	fi
fi

if ping -c 1 "$IPSAT7" &> /dev/null
then 
	if [ $NBSAT -ge 7 ]; then 
	echo "########## LAUNCH COSMOS SAT7 #########"
	cd /opt/nos3/cosmos7
	./openc3.sh cleanup
	./openc3.sh run
	fi
fi
