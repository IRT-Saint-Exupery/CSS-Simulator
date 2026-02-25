import os
import re

#get the cmd definition files and strip tlm and other files from result
#cmd = os.popen('find `find /home/nos3/Desktop/github-nos3 -name "cmd_tlm" | grep -v build` -name "*.txt" | grep -vi tlm.txt', mode = 'r', buffering = -1)

def getDirectories():
	cmd = os.popen('find ../Desktop -path "*/cmd_tlm/*" | grep -vi -e "tlm.txt" -e "build"', mode = 'r', buffering = -1)

	cmd_files = cmd.readlines()
	cmd_files = [l.rstrip('\n') for l in cmd_files]
	
	targets = {}
	for f in cmd_files:
	
		#either path /gsw/cosmos/config/targets/[TARGET] or /components/[target]/gsw/[TARGET]
		target_name = re.search("(?:(?:\/gsw\/cosmos\/config\/targets\/)|(?:\/components\/[a-z0-9_]+\/gsw\/))([a-zA-Z0-9_]+)",f)
		name = target_name.group(1)
		if (name not in targets):
			#add file path
			targets[name] = [f]
		else:
			#add file path
			targets[name].append(f)	

			
	return targets
	
	
def parseCommands(f_list_to_parse):
	
	cmd_args = {}
	cur_name = ""
	#for every file related to target
	for path in f_list_to_parse:
		
		f = open(path,'r')
		config_file = f.readlines()
		
		params_to_find = False

		for line in config_file:

			#if a cmd has been found look for params
			if params_to_find == True:
				
				#ADD SUPPORT FOR COMMENTS EXCLUSION (#blabla) !!!!!
				#SEPARATE REGEX ?
				                          #3 start words possible  #name         #size      #type     #max   #numerical value | string val    #desc      #endianness
				match_param = re.search("(?:APPEND_.*PARAMETER)\s+([A-Z_0-9]+)\s+([0-9]+)\s+([A-Z]+)\s*.*\s((?:[0x]*[0-9.A-F]+)|(?:\".*\"))\s+(\".*\")\s*((?:[A-Z_]+ENDIAN)*)",line)
				
				if match_param != None:
					#add param to dict
					#CHANGER POUR UN DICT POUR METADATA ?
					param_dict = {
						"NAME":match_param.group(1),
						"SIZE":match_param.group(2),
						"TYPE":match_param.group(3),
						"VALUE":match_param.group(4),
						"DESC":match_param.group(5),
						"ENDIANNESS":match_param.group(6)
					}
					cmd_args[cur_name].append(param_dict)
				else:
					#there may be a param following a 'state' - ADD SUPPORT FOR UNITS, META etc ?
					if "STATE" not in line:
						#end of params, look for next command
						params_to_find = False	

			#if we are looking for a command definition
			if params_to_find == False:
				#match on command name and its description
				match_cmd = re.search("(?:COMMAND [A-Z0-9_]+ )([A-Z0-9_]+).*(\".*\")",line)
			
				if match_cmd != None:
					cur_name = match_cmd.group(1) #description not processed, maybe add it ?
					cmd_args[cur_name] = []
					params_to_find = True
	return cmd_args			

if __name__ == "__main__": 
	targt = getDirectories()
	t_list = list(targt.keys())


	#Target selection
	target_correct = False
	while not target_correct:
		print("Targets available : ")
		for t in t_list:
			print(t)
		print("\nSelect a target :")
		t_select = input().upper()
		if t_select in t_list:
			target_correct = True
		else:	
			print("Selection "+t_select+" is not correct, please select an available target !")

	parsed_data = parseCommands(targt[t_select])

	#REMOVE CFDP,CFDP_TEST & PDU ?? or tag as not supported ?
	#FOR CFS ADD SUB-CATEGORIES BY SUB-TARGET OR BY FILE ???
	available_commands = list(parsed_data.keys())

	print("\nAvailable commands for target "+t_select+" are :")
	print(*available_commands)

	print("Enter a command :")
	test = input().upper()

	print(parsed_data[test])
