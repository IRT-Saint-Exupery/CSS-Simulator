#!/bin/bash

source IP_Scenario.sh

#cd $HOME


if [ "$IP_LOCAL_ADDR" != "$IPSAT1" ]; then
echo "#################### $0 Shoud be called from the main VM - SAT1 #################"
exit
fi

# diffuse on all VM all the new IP addresses related to your configuration 
#find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT1 -exec ./diffusefile.sh {} $IPSAT1 \; \) >/dev/null 2>&1
#find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT2 -exec ./diffusefile.sh {} $IPSAT2 \; \) >/dev/null 2>&1
#find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT3 -exec ./diffusefile.sh {} $IPSAT3 \; \) >/dev/null 2>&1
#find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT4 -exec ./diffusefile.sh {} $IPSAT4 \; \) >/dev/null 2>&1
#find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT5 -exec ./diffusefile.sh {} $IPSAT5 \; \) >/dev/null 2>&1
#find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT6 -exec ./diffusefile.sh {} $IPSAT6 \; \) >/dev/null 2>&1
#find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT7 -exec ./diffusefile.sh {} $IPSAT7 \; \) >/dev/null 2>&1
#find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IP42 -exec ./diffusefile.sh {} $IP42 \; \) >/dev/null 2>&1
#find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPMCS -exec ./diffusefile.sh {} $IPMCS \; \) >/dev/null 2>&1

nom=routeConfig.txt.1SAT
cp /home/nos3/Install_tools_nos3/RouteConfig/${nom//1/$NBSAT} /home/nos3/Desktop/github-nos3/sims/cfg/routeConfig.txt 

# Specialize configuration for each satellite VM
if [ $NBSAT -ge 2 ]; then 
	if ping -c 1 "$IPSAT2" &> /dev/null
	then 
		echo "======== Specialize sat 2"
		find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT2 -exec ./diffusefile.sh {} $IPSAT2 \; \) >/dev/null 2>&1
		./Specialize_SAT.sh 1 2 $IPSAT1 $IPSAT2 
	fi
fi

if [ $NBSAT -ge 3 ]; then 
	if ping -c 1 "$IPSAT3" &> /dev/null
	then 
		echo "======== Specialize sat 3"
		find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT3 -exec ./diffusefile.sh {} $IPSAT3 \; \) >/dev/null 2>&1
		./Specialize_SAT.sh 1 3 $IPSAT1 $IPSAT3 
	fi
fi

if [ $NBSAT -ge 4 ]; then 
	if ping -c 1 "$IPSAT4" &> /dev/null
	then 
		echo "======== Specialize sat 4"
		find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT4 -exec ./diffusefile.sh {} $IPSAT4 \; \) >/dev/null 2>&1
		./Specialize_SAT.sh 1 4 $IPSAT1 $IPSAT4 
	fi
fi

if [ $NBSAT -ge 5 ]; then 
	if ping -c 1 "$IPSAT5" &> /dev/null
	then 
		echo "======== Specialize sat 5"
		find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT5 -exec ./diffusefile.sh {} $IPSAT5 \; \) >/dev/null 2>&1
		./Specialize_SAT.sh 1 5 $IPSAT1 $IPSAT5 
	fi
fi

if [ $NBSAT -ge 6 ]; then 
	if ping -c 1 "$IPSAT6" &> /dev/null
	then 
		echo "======== Specialize sat 6"
		find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT6 -exec ./diffusefile.sh {} $IPSAT6 \; \) >/dev/null 2>&1
		./Specialize_SAT.sh 1 6 $IPSAT1 $IPSAT6 
	fi
fi

if [ $NBSAT -ge 7 ]; then 
	if ping -c 1 "$IPSAT7" &> /dev/null
	then 
		echo "======== Specialize sat 7"
		find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPSAT7 -exec ./diffusefile.sh {} $IPSAT7 \; \) >/dev/null 2>&1
		./Specialize_SAT.sh 1 7 $IPSAT1 $IPSAT7
	fi
fi


# Specialize configuration for 42 VM
echo "======== Specialize 42 at $IP42"
find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IP42 -exec ./diffusefile.sh {} $IP42 \; \) >/dev/null 2>&1
./Specialize_42.sh $IP42

# Specialize configuration for MCS VM
echo "======== Specialize MCS at $IPMCS"
find . -name '*build*' -prune -o \( -newer ~/cookies/maj$IPMCS -exec ./diffusefile.sh {} $IPMCS \; \) >/dev/null 2>&1
./Specialize_MCS.sh $IPMCS


#to be refined !


