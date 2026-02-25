#!/bin/bash

#wait before sending attacks, don't send them right away

sleep 10m

#sleep 1h -- not sure necesary for this kind of brutal mess

#random sleep of 1-5mn
MOD=5

while true
do
	nb4=$RANDOM

	let "nb4%=$MOD"

	sleep "${nb4}m"

	echo "DEBUG : ${nb4}m elapsed"

	echo "swapping CAM mode"
	python3 ../Input_Generator/Main.py send ../Input_Generator/cam_noop.json
	sleep 1m
	echo "sending EXP3_CC packet"
	python3 ../Input_Generator/Main.py send ../Input_Generator/cam_exp3_cc.json
	sleep 10m
done
