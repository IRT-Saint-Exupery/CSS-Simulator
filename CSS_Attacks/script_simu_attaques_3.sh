#!/bin/bash

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

##CA_1 SB
#
#i=0
#
#while [ $i -lt 50 ] #50
#do
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/stop_sb.json
#	now="$(date +"%T")"
#	echo "CA_1 SB : $now"
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

#/bin/bash Attack_A13_logging.sh
#python3 ../Input_Generator/Main.py send ../Input_Generator/cam_hw_check.json

###############EMBEDDED MALICIOUS CAM ATTACKS################

#FLOODING ATTACK ! (FA_2) --- TODO WHEN TESTING AI

#/bin/bash Attack_A13_hk.sh

#sleep 60s

#i=0

#while [ $i -lt 50 ] #50
#do
#	#SEND EXP3_CC TO TRIGGER ATTACK
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/cam_exp3_cc.json
#	now="$(date +"%T")"
#	echo "FA_2 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 30s
#	i=`expr $i + 1`
#done

#CYBER-ASAT CAM ! (CA_2)

#/bin/bash Attack_A14.sh

#sleep 60s

#i=0

#while [ $i -lt 50 ] #50
#do
#	#SEND EXP3_CC TO TRIGGER ATTACK
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/cam_exp3_cc.json
#	now="$(date +"%T")"
#	echo "CA_2 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 40s
#	i=`expr $i + 1`
#done

#sleep 10s

#APP DELETE CAM (AD_2)

#/bin/bash Attack_A14_A8.sh

#sleep 60s

#i=0

#while [ $i -lt 50 ] #50
#do
#	#SEND EXP3_CC TO TRIGGER ATTACK
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/cam_exp3_cc.json
#	now="$(date +"%T")"
#	echo "AD_2 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 40s
#	i=`expr $i + 1`
#done

#sleep 120s

#SYSTEM WIPE CAM (SW_2)

#/bin/bash Attack_A15.sh

#sleep 60s

#i=0

#while [ $i -lt 50 ] #50
#do
#	#SEND EXP3_CC TO TRIGGER ATTACK
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/cam_exp3_cc.json
#	now="$(date +"%T")"
#	echo "SW_2 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 40s
#	i=`expr $i + 1`
#done

#sleep 120s

#SAT SPIN CAM (SP_2)

#/bin/bash Attack_A14_A12.sh

#sleep 60s

#i=0

#while [ $i -lt 50 ] #50
#do
#	#SEND EXP3_CC TO TRIGGER ATTACK
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/cam_exp3_cc.json
#	now="$(date +"%T")"
#	echo "SP_2 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 40s
#	i=`expr $i + 1`
#done

#sleep 120s

#CAM CRASH CAM (CR_2)

#/bin/bash Attack_A14_A5.sh

#sleep 60s

#i=0

#while [ $i -lt 50 ] #50
#do
#	#SEND EXP3_CC TO TRIGGER ATTACK
#	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/cam_exp3_cc.json
#	now="$(date +"%T")"
#	echo "CR_2 : $now"
#	#echo time + attack
#	#sleep 3s
#	sleep 40s
#	i=`expr $i + 1`
#done

#HIJACKING (HJ_1)

i=0

while [ $i -lt 50 ] #50
do
	python3 Attack_A21.py
	now="$(date +"%T")"
	echo "HJ_1 : $now"
#	#echo time + attack
#	#sleep 3s
	sleep 30s
	i=`expr $i + 1`
done

echo "Attacks executed"
