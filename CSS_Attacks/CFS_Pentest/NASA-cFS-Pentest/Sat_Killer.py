
import readline
import shlex
import socket
import time

from sat_killer_dep import *

def startup_config():
    print('\n')
    print('~~CONFIGURATION~~')
    print('[1] Use default')
    print('[2] Custom config')
    print('[3] Exit')
    print('\n')

    while True:
        cmd, *args = shlex.split(input('<>'))

        if cmd == '1':
            return([1234, '127.0.0.1'])
        
        if cmd == '2':
            t_port = input('TARGET PORT>')
            t_port = int(t_port)
            print('Target port set to > ' + str(t_port))

            t_ip = input('TARGET IP>')
            t_ip = str('Target IP set to > ' + str(t_ip))
            print(t_ip)

            return([t_port, t_ip])

        if cmd in ['3', 'exit', 'escape', 'return']:
            exit()
            return



def jammingAttack(C1):
    print('\n')
    print('~~cFS KILLER~~') 
    print('<<~~JAMMER~~>>')
    print('Select a command')
    print('[1] Quick Jam')
    print('[2] Custom Jam')
    print('[7] BACK')
    print('\n')

    while True:
        cmd, *args = shlex.split(input('>>'))

        if cmd == '1':
            print('\n')
            print('~~cFS KILLER~~') 
            print('<<~~JAMMER~~>>')  
            print('> QUICK JAM <')
            C1.quick_jam()
        
        if cmd == '2':
            customJam(C1)

        if cmd in ['7', 'exit', 'escape', 'return']:
            main_menu(C1)

def customJam(C1):
    print('\n')
    print('~~cFS KILLER~~') 
    print('<<~~JAMMER~~>>')  
    print('> CUSTOM JAM <')
    print('Select a command')
    print('[1] Show Configuration')
    print('[2] Edit Configuration')
    print('[3] Launch Attack')
    print('[3] BACK')
    print('\n')


    while True:
        cmd, *args = shlex.split(input('>>>'))

        if cmd == '1':
            C1.show_custom_jam()  
            print('\n')
            print('[1] Show Configuration')
            print('[2] Edit Configuration')
            print('[3] Launch Attack')
            print('[3] BACK') 
        
        if cmd == '2':
            newJamCom = input('Path to command.txt --> ')
            newJamDelay = float(input('Delay between packet send --> '))
            newJamCount = input('Number of packets to send --> ')

            if newJamCom != None:
                C1.jamCom = newJamCom
            if newJamDelay != None:    
                C1.jamDelay = newJamDelay
            if newJamCount != None:
                C1.jamCount = newJamCount
            
            print('\n')
            print('[1] Show Configuration')
            print('[2] Edit Configuration')
            print('[3] Launch Attack')
            print('[3] BACK')
            print('\n')
            

        if cmd == '3':
            print('~~cFS KILLER~~') 
            print('<<~~JAMMER~~>>')  
            print('> CUSTOM JAM <')
            print('Initiating attack...')
            C1.custom_jam()
            print('Jamming Complete')
            print('\n')
            time.sleep(3)
            print('~~cFS KILLER~~') 
            print('<<~~JAMMER~~>>')  
            print('> CUSTOM JAM <')
            print('Select a command')
            print('[1] Show Configuration')
            print('[2] Edit Configuration')
            print('[3] Launch Attack')
            print('[3] BACK')

        if cmd in ['BACK', 'back', 'escape', 'return', 'exit', 'q']:
            jammingAttack(C1)







def startup():
    banner()
    c_conf = startup_config()
    C1 = sender(c_conf[0], c_conf[1])
    main_menu(C1)


def main_menu(C1):
    print('\n')
    print('~~cFS KILLER~~') 
    print('Select a command')
    print('[1] Initiate Jamming attack')
    print('[2] Initiate Replay/Custom attack')
    print('[3] Initiate OSK ASAT')
    print('[4] Initiate custom ASAT')
    print('[5] Edit Configuration')
    print('[6] Help')
    print('[7] Exit')
    print('\n')

    while True:
        cmd, *args = shlex.split(input('>'))

        if cmd == '1':
            jammingAttack(C1)
        
        if cmd == '3':
            pass

        if cmd == '4':
            pass

        if cmd == '5':
            pass

        if cmd == '6':
            pass

        if cmd in ['7', 'exit', 'escape', 'return']:
            exit()
            return



    

startup()






