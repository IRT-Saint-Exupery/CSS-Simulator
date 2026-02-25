import CmdParser
import CmdSender
import ScenarioManager
import ProcessMonitoring
import time
import argparse
import os
import sys

# FUNCTION DEFINITIONS
#-----------------------
def argsInit():
	parser = argparse.ArgumentParser()
	parser.add_argument("mode", choices=['create','send'], help="scenario mode") # si on veut rester sur 1 et 2 peut être plus pertinent d'utiliser int
	parser.add_argument("file", help=".json file")
	parser.add_argument("--port", type=int, default=6010, help="Port to use for sending commands")
	args = parser.parse_args()
	return args

def targetSelect(t_list):
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
			print("Selection "+t_select+" is not correct\n")
	return t_select

def cmdSelect(cmdlist, target):
	cmd_valid = False
	while not cmd_valid:
		print("\nAvailable commands for target "+target+" are :")
		for i in range(len(available_commands)):
			print(available_commands[i]+" ")

		print("Select a command : ")
		cmd = input().upper()
		if cmd not in cmdlist:
			print(cmd+" is not a valid command\n")
		else:
			cmd_valid = True
	return cmd	
#-----------------------

#get dirs ordered by target
targt = CmdParser.getDirectories()
t_list = list(targt.keys())

parsed_data = {}

for tgt in t_list:
	parsed_data[tgt] = CmdParser.parseCommands(targt[tgt])

args = argsInit()
if args.mode == "create":
	mode = '1'
else:
	mode = '2'
	
cmd_file = args.file
port = args.port

if mode == '1':
	#create file mode
	print("Enter number of commands to send :")
	nb_cmd = int(input())
	#add input control ? for large or negative values ?
	
	t_select = []
	cmd = []
	parsed_parameters = []
	
	for i in range(nb_cmd):                       #if 0 then do nothing
	
		t_select.append(targetSelect(t_list))

		cur_target = t_select[i]

		#REMOVE CFDP,CFDP_TEST & PDU ?? or tag as not supported ?
		#FOR CFS ADD SUB-CATEGORIES BY SUB-TARGET OR BY FILE ???
		available_commands = list(parsed_data[cur_target].keys())

		cmd.append(cmdSelect(available_commands, cur_target))

		parsed_parameters.append(parsed_data[cur_target][cmd[i]])

	for elem in parsed_parameters:
		for param in elem:
			print(param)
		print()


	ScenarioManager.createFile(parsed_parameters, t_select, cmd, cmd_file)

else:
	#parse file mode
	
	#REPLACE BY PARSED_DATA AT THE TOP OF THE FILE
	parsed_data_cmd = {}
	
	#get a dict with CMD:PARAMS for all commands across all targets (unlike parsed_data where we have TGT:CMD:PARAMS)
	for tgt in t_list:
		parsed_data_cmd.update(CmdParser.parseCommands(targt[tgt]))

	input_dict = ScenarioManager.getParamsFromFile(cmd_file)

	input_parameters = []
	command_keys = []
	target_names = []
	fuzzing = []

	#extract keys from command dict
	ScenarioManager.getCommandKeys(input_dict, command_keys, target_names, fuzzing)
	
	#create params from values in file
	ScenarioManager.setParamsValues(parsed_data_cmd, input_dict, input_parameters, command_keys)

	#for every command scanned from the file:
	for i in range(len(list(input_dict.keys()))):
		fuzz_target = fuzzing[i][0]
		
		if fuzz_target!= "None": # if there is a target to fuzz

			#clear previous logs
			if os.path.exists("fuzz_logs.txt"):
				os.remove("fuzz_logs.txt")
			
			log_file = open("fuzz_logs.txt", "a")
			
			pid = ProcessMonitoring.MonitorProcess()

			if pid == -1:
				print("FUZZING CANCELLED")
				break;
			
			max_fuzz_times = fuzzing[i][1] #number of times to fuzz
			param = [l for l in input_parameters[i] if l["NAME"] == fuzz_target] #targeted parameter
			counter = 0
			#for each time we want to fuzz
			for j in range(max_fuzz_times): 

				print(f'param : {param[0]["VALUE"]}, param_type : {type(param[0]["VALUE"])}')
				param[0]["VALUE"] = CmdSender.fuzzParam(param[0], j)
				print(f'param : {param[0]["VALUE"]}, param_type : {type(param[0]["VALUE"])}')

				print("fuzzed parameter :"+str(param[0]["VALUE"]))
				log_file.write("param :"+str(param[0]["VALUE"])+"\n")
				
				if not ProcessMonitoring.CheckProcessIsAlive(pid):
					print("EXITING FUZZING")
					sys.exit()
				
				CmdSender.sendCommand(input_parameters[i],target_names[i], port)
				counter+=1
				print("counter : "+str(counter))
				#time.sleep(0.05)
				#time.sleep(1)

			log_file.close()

		else:
			CmdSender.sendCommand(input_parameters[i],target_names[i], port)
			#time.sleep(1)	 # EDIT LATER, PROOF OF CONCEPT
