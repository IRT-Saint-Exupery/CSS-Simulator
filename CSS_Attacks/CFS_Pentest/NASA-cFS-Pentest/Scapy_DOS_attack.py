import sys
from scapy.all import *
import logging

Sat_IP = "127.0.0.1"

Sat_Port = '1234'

command = [0x18, 0x9f, 0xC0, 0x00, 0x00, 0x01, 0xb9, 0x00]
byte_message = bytes(command)

# while True:

#     IP1 = IP(source_IP = Sat_IP, destination = Sat_IP)

packet = IP(src = Sat_IP, dst = Sat_IP)/UDP(dport = 1234)/byte_message


send(packet, iface = 'lo', loop = 1)

