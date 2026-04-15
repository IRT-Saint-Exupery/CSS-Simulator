if [ "$#" -eq 1 ]; then
	echo "arg : $1"
	python3 ../Input_Generator/Change_json_field.py ../Input_Generator/scenarios/SW_1_custom.json DIRECTORY $1 --type=STRING
	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/SW_1_custom.json
else
	#no arg provided or bad syntax, default behavior
	python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/SW_1.json
fi
#python3 ../Input_Generator/Main.py send ../Input_Generator/scenarios/SW_1.json
