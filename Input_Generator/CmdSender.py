import socket
import struct
#FUZZ
import random

#DEFAULT ENDIANNESS IS DEPENDENT ON ENDIANNESS OF THE COMMAND. NEEDS TO BE MATCHED ON ? (edit regex on command names to add matching group for endianness ?)
#NEED TO PARSE THE BUILD DIR TO HAVE THE "CosmosCfsConfig::PROCESSOR_ENDIAN" RESOLVED TO WHATEVER IT IS ?
#For now we assume the endianness of the cFS is little-endian based on the current definitions of the command files (NOS3 v1.6.2)

# -> EDIT REGEX NOT TO EXCLUDE BUILD BUT TO KEEP BUILD OVER OTHER PATHS ?? NOT SURE
# -> THIS WOULD MEAN THAT WE NEED COSMOS AND NOS3 TO BE BUILT VIA MAKE BEFORE PARSING, OR GET THE INFO FOR THE MACRO ELSEWHERE ???
# -> info about macro is in the script from gsw/scripts defined in the makefile which builds the gsw target
	
def convertIntToBytes(arg, default_endian, is_header=0):
	
	#Negative unsigned ints are not supported, no difference for signed and unsigned ints

	value = arg["VALUE"]
	param_size = int(int(arg["SIZE"])/8) #convert size value in bytes
	
	endianness = arg["ENDIANNESS"]
	
	#if no endianness specified, revert to default
	if ((endianness != "BIG_ENDIAN")&(endianness != "LITTLE_ENDIAN")):
		endianness = default_endian
	
	#default big-endian for header
	if ((endianness == "BIG_ENDIAN")|(is_header == 1)):
		b_output = int(value,0).to_bytes(param_size, 'big') #padding managed by to_bytes with param_size, conversion from hex value managed by int(a, 0)
	else:
		b_output = int(value,0).to_bytes(param_size, 'little') #same
		
	return b_output

def convertFloatToBytes(arg, default_endian):

	value = arg["VALUE"]
	param_size = int(int(arg["SIZE"])/8) #convert size value in bytes
	
	endianness = arg["ENDIANNESS"]
	
	#if no endianness specified, revert to default
	if ((endianness != "BIG_ENDIAN")&(endianness != "LITTLE_ENDIAN")):
		endianness = default_endian

	if (endianness == "BIG_ENDIAN"):
		endian_char = ">"
	else:
		endian_char = "<"
	
	f = float(value)
	if (param_size == 4):
		b_output = struct.pack(endian_char+"f", f)
	elif (param_size == 8):
		b_output = struct.pack(endian_char+"d", f)
	else:
		print("Bad size parameter for float, check the command definition file")
		exit()
		
	return b_output

def convertBlockToBytes(arg, default_endian):

	#ADD SUPPORT FOR INTEGER INPUT ???
	value = arg["VALUE"][2:] #get rid of the 0x prefix because we cannot use int(a, 0)
	param_size = int(int(arg["SIZE"])/8) #convert size value in bytes
	
	endianness = arg["ENDIANNESS"]
	
	#if no endianness specified, revert to default
	if ((endianness != "BIG_ENDIAN")&(endianness != "LITTLE_ENDIAN")):
		endianness = default_endian
		
	#we consider the value is already in little-endian format if little-endian mode
	b_output = bytes.fromhex(value)
		
	#if the value is shorter than the total size
	if (len(b_output) < param_size): 
		#binary value, no permutations, only add padding
		if endianness == "LITTLE_ENDIAN":
			b_output = b_output+(b'\x00'*(param_size - len(b_output))) #add padding of zeros AFTER the value
		else:
			b_output = (b'\x00'*(param_size - len(b_output)))+b_output #add zeros BEFORE the value
	return b_output
	
def convertStrToBytes(arg):

	value = arg["VALUE"].strip('"') #remove quotation marks
	param_size = int(int(arg["SIZE"])/8) #convert size value in bytes
	b_output = bytes(value,'utf-8') #we assume that the string is in utf-8 / ASCII

	if (len(b_output) < param_size):
		#for both endianness in COSMOS the string is in normal order and the padding comes after the value
		b_output = b_output+(b'\x00'*(param_size - len(b_output))) #add padding of zeros AFTER the value
	return b_output

def sendCommand(args,target,port):
	
	if (target == "CFS"):
		default_endianness = "LITTLE_ENDIAN"
	else:
		default_endianness = "BIG_ENDIAN"
	
	#macro
	HEADER = 1

	ID = convertIntToBytes(args[0], default_endianness, HEADER)
	CTRL_SEQ = convertIntToBytes(args[1], default_endianness, HEADER)
	LEN = convertIntToBytes(args[2], default_endianness, HEADER)
	FC = convertIntToBytes(args[3], default_endianness, HEADER)
	CHECKSUM = convertIntToBytes(args[4], default_endianness, HEADER)
	
	additional_params_b = bytes()
	
	if len(args) > 5:
		extra_args = args[5:]
		for a in extra_args:
			arg_type = a["TYPE"]
			if arg_type == "STRING":
				additional_params_b = additional_params_b + convertStrToBytes(a)	
			elif arg_type == "BLOCK":
				additional_params_b = additional_params_b + convertBlockToBytes(a, default_endianness)
			elif arg_type == "FLOAT":
				additional_params_b = additional_params_b + convertFloatToBytes(a, default_endianness)
			else: #check if works with "ARRAY" types...
				additional_params_b = additional_params_b + convertIntToBytes(a, default_endianness)
	
	
	byte_message = ID + CTRL_SEQ + LEN + FC + CHECKSUM + additional_params_b

	#print(byte_message.hex())

	opened_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

	#6010 is for the cryptolib of SAT1
	opened_socket.sendto(byte_message, ("192.168.100.5", port))
	
def fuzzParam(param, times_counter):

	if param["TYPE"] == "STRING":
		#if "FILENAME" in param["NAME"]:
		#	output = "/toto.test"
		#elif ("APPLICATION" in param["NAME"]) or ("APPNAME" in param["NAME"]):
		#	output = "toto"
		if times_counter == 0:
			output = ""
		elif times_counter == 1:
			output = " "
		elif times_counter == 2:
			output = "."
		elif times_counter == 3:
			output = "\n"
		elif times_counter == 4:
			output = "echo \"TEST\""
		else:
			output = "toto"
	else:
		if times_counter == 0:
			#min unsigned
			output = "0"
		elif times_counter == 1:
			#max value - -1 if signed
			output = "0x"+(hex(15)[2:]*int(int(param["SIZE"])/4)).upper()                     #add a FF value for each byte (F for each half of byte) - FFFFF for SIZE bits
		elif times_counter == 2:
			#min + 1
			output = "1"                                                                        #if min is 0
		elif times_counter == 3:
			#max - 1
			output = "0x"+(hex(15)[2:]*int((int(param["SIZE"])/4)-1)).upper()+"E"             #FFFF..FE
		elif times_counter == 4:
			#min signed - mid value if unsigned 
			output = "0x8"+("0"*int((int(param["SIZE"])/4)-1)).upper()                        #10000..00 
		elif times_counter == 5:
			#mid value if signed
			output = "0x4"+("0"*int((int(param["SIZE"])/4)-1)).upper()                        #01000..00 // pertinent ? découpage en parties plus intéressant ?
		else:
			#print("max :"+str(2**int(param["SIZE"])))
			output = random.randint(0,2**int(param["SIZE"]))
	return output
	
