#!/bin/bash

RANDOM=$(date +%s)

i=0
while [ $i -lt 1000 ]
do
	r=$(($RANDOM % 7))
	#r=$i
	#echo $r

	if (($r == 0))
	then	
		#FLOODING ATTACK ! (FA_1)
		python3 Attack_A7.py
		now="$(date +"%T")"
		echo "FA_1 : $now"
	fi

	if (($r == 1))
	then	
		#CA_1 ISL
		python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/stop_isl.json
		now="$(date +"%T")"
		echo "CA_1 ISL : $now"	
		#echo time + attack
	fi

	if (($r == 2))
	then
		#CA_1 CI
		python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/stop_ci.json
		now="$(date +"%T")"
		echo "CA_1 CI : $now"
		#echo time + attack
	fi

	if (($r == 3))
	then
		#AD_1
		python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/AD_1.json	
		now="$(date +"%T")"
		echo "AD_1 : $now"
		#echo time + attack
	fi

	if (($r == 4))
	then
		#SW_1
		python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/SW_1.json	
		now="$(date +"%T")"
		echo "SW_1 : $now"
		#echo time + attack
	fi

	if (($r == 5))
	then
		#CRYPTOLIB CRASH ! (CR_3)
		python3 Attack_A6.py
		now="$(date +"%T")"
		echo "CR_3 : $now"
		#echo time + attack
	fi

	if (($r == 6))
	then
		#HIJACKING (HJ_1)
		python3 Attack_A21.py
		now="$(date +"%T")"
		echo "HJ_1 : $now"
		#echo time + attack
	fi

	sleep 5s

	i=$((i + 1))
done

#FLOODING ATTACK ! (FA_1)

#i=0
#
#while [ $i -lt 50 ] #50
#do
#	python3 Attack_A7.py
#	now="$(date +"%T")"
#	echo "FA_1 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 30s
#	i=`expr $i + 1`
#done

#CA_1 ISL

#i=0
#
#while [ $i -lt 50 ] #50
#do
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/stop_isl.json
#	now="$(date +"%T")"
#	echo "CA_1 ISL : $now"	
#	#echo time + attack
#	#sleep 3s
#	sleep 30s
#	i=`expr $i + 1`
#done

#CA_1 CI

#i=0
#
#while [ $i -lt 50 ] #50
#do
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/stop_ci.json
#	now="$(date +"%T")"
#	echo "CA_1 CI : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 30s
#	i=`expr $i + 1`
#done


#AD_1

#i=0

#while [ $i -lt 50 ] #50
#do
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/AD_1.json	
#	now="$(date +"%T")"
#	echo "AD_1 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 30s
#	i=`expr $i + 1`
#done

#SW_1

#i=0
#
#while [ $i -lt 50 ] #50
#do
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/SW_1.json	
#	now="$(date +"%T")"
#	echo "SW_1 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 30s
#	i=`expr $i + 1`
#done
#echo "DEBUG : ${nb}m elapsed"

#SATELLITE SPIN (SP_1)

#i=0
#
#while [ $i -lt 50 ] #50
#do
#	python3 Attack_A12.py
#	now="$(date +"%T")"
#	echo "SP_1 : $now"
#	#echo time + attack
#	#sleep 30s
#	sleep 20s
#	#DEFUSE (SWITCH ADCS TO NORMAL MODE BEFORE ATTACKING AGAIN TO COME BACK TO START)
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/spin_defuse.json
#	sleep 10s
#	i=`expr $i + 1`
#done

#CAM CRASH (CR_1)

#i=0
#
#while [ $i -lt 50 ] #50
#do
#	python3 Attack_A5.py
#	now="$(date +"%T")"
#	echo "CR_1 : $now"
#	#echo time + attack
#	sleep 60s
#	i=`expr $i + 1`
#done

#CRYPTOLIB CRASH ! (CR_3)

#i=0
#
#while [ $i -lt 50 ] #50
#do
#	python3 Attack_A6.py
#	now="$(date +"%T")"
#	echo "CR_3 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 30s
#	i=`expr $i + 1`
#done

#HIJACKING (HJ_1)

#i=0

#while [ $i -lt 50 ] #50
#do
#	python3 Attack_A21.py
#	now="$(date +"%T")"
#	echo "HJ_1 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 30s
#	i=`expr $i + 1`
#done

#AD_1 esquive

#i=0
#
#while [ $i -lt 10 ] #10
#do
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/AD_1_esquive.json	
#	now="$(date +"%T")"
#	echo "AD_1 esquive : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 30s
#	i=`expr $i + 1`
#done

#/bin/bash Attack_A13_logging.sh
#python3 ../Input_Generator/Main.py send ../Input_Generator/cam_hw_check.json

#echo "Attacks executed"
