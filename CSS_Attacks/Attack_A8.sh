#!/bin/bash

if [ "$#" -eq 2 ]; then
	echo "arg 1: $1"
	echo "arg 2: $2"
	python3 ../Input_Generator/Change_json_field.py ../Input_Generator/scenarios/AD_1_custom.json --command_nb=1 FILENAME $1 --type=STRING
	python3 ../Input_Generator/Change_json_field.py ../Input_Generator/scenarios/AD_1_custom.json --command_nb=2 APPLICATION $2 --type=STRING
	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/AD_1_custom.json
else
	#no arg provided or bad syntax, default behavior
	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/AD_1.json
fi
#python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/AD_1.json
