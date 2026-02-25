#!/bin/bash

i=0

while [ $i -lt 300 ] #100
do
	echo "sending NOOP"
	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/cam_noop.json
	sleep 1s
	i=`expr $i + 1`
done
