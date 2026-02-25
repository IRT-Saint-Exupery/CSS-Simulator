# Modern Satellite Vulnerability Analysis
Final year Capstone project at USNA. The scope of this project was looking for vulnerabilities in NASA's open source cFS (core flight software). 

## cFS Overview
The Core Flight System (cFS) is a generic flight software architecture framework used on flagship spacecraft, human spacecraft, cubesats, and Raspberry Pi. The cFS Framework is a core subset of cFS. There are additional OSALs, PSPs, and other tools available from a variety of sources. <br /> 

[cFS GitHub Repository](https://github.com/nasa/cFS) <br />
[OSK GitHub Repository](https://github.com/OpenSatKit/OpenSatKit)

## Successful Attacks
#### Replay Attack
 Send an existing command <br />
  `simple_packet_sender.py` <br />
  `simple_packet_sender_NOOP.py`
#### DOS Attack
  Send a large amount of commands quickly to cause shutdown <br />
  `Scapy_DOS_attack.py`
#### Targeted App Delete
  Sends a command to delete an app, can be used to delete the Command Ingest app so that the Satellite can no longer interpret packets <br />
  `I_killed_everyone.py` <br />
  `Cyber_ASAT.py`
#### Systematic Brute Force App Delete
  Sends a brute force style of packets, slowly deleting all apps on the system (up to 5 characters in name length) <br />
  `BruteForceAppKiller.py`
  
#### cFS Killer (Beta)
  A Satellite hacking tool for systems running cFS. We have combined a number of attacks we have developed for cFS into an easy to run command line tool with a simple UI. (note: `SatKillerDep.py` is also required to run) <br />
  `SatKiller.py`

  
 
  
  
  
