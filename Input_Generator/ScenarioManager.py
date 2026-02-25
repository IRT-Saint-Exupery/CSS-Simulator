import json
import copy

#get dict from JSON file - filename can be relative path or absolute path of file
def getParamsFromFile(filename):

	with open(filename, 'r') as openfile:
		json_object = json.load(openfile)
		
	return json_object

#create file from a specified command list (1 or more)
def createFile(parsed_parameters_list, t_select, cmd_name, cmd_file):

	#add into file documentation about size of fields of command ??

	dict_command = {}

	#for every command
	for i in range(len(parsed_parameters_list)):

		cmd_args = {}

		#for every parameter in the command
		for j in range(len(parsed_parameters_list[i])):
			#create NAME:VALUE pairs for each parameter
			cmd_args[parsed_parameters_list[i][j]["NAME"]] = parsed_parameters_list[i][j]["VALUE"]
			
		#create the commands dict
		dict_command["command"+str(i+1)] = {"TARGET":t_select[i],cmd_name[i]:cmd_args, "TIMESTAMP": 0.00, "FUZZ":["None",0]}  # ADD TIMESTAMP DEFAULT BEHAVIOR ???

	#convert to json
	json_object = json.dumps(dict_command, indent=4)
	
	#write on disk
	with open(cmd_file, "w") as outfile:
		outfile.write(json_object)
	
	print("File created")
	#add return code of outfile.write to verify if write is successful ?

#get the command and target keys to be able to find param syntax and send to the right target(useful for endianness)
def getCommandKeys(input_dict, command_keys, target_names, fuzz):

	#for each command
	for k in list(input_dict.keys()):
		#get keys from commands in file
		keys_temp = list(input_dict[k].keys())

		#get the command name key
		command_keys.append(*[l for l in keys_temp if ((l != "TIMESTAMP") & (l != "TARGET") & (l != "FUZZ"))])
		
		#get the target name key
		target_names.append(input_dict[k]["TARGET"])
		
		#get the fuzz info
		fuzz.append(input_dict[k]["FUZZ"])

#prefill params with default values and replace the "VALUE" field of the dict with what is in the JSON file
def setParamsValues(parsed_data2, input_dict, input_parameters, command_keys):

	#for each command
	for c in command_keys:
		#get parameters data for the command (default values)
		input_parameters.append(copy.deepcopy(parsed_data2[c]))
	
	i = 0
	
	#for each command, replace value
	for c in command_keys:
		for param in input_parameters[i]:
			param["VALUE"] = input_dict["command"+str(i+1)][c][param["NAME"]]
		i += 1
		
