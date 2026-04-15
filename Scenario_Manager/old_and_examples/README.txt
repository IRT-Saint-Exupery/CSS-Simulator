to use the scenario manager you can simply call python3 Manager.py scenarios/S1.json 

The script Manager.py will 

-start the simulation 
-start a tcpdump in /tmp of VM MCS
-launch the attacks defined in S1.json
-stop the tcpdump 
-save relevant files in /tmp of VM MCS
-stop the simulation. 

you can also simply call a bash script of the type 

./CSS_Test.sh 

to launch multiple scenarios in a sequence.

WARNING: use terminator because gnome-terminal is closed at the end of each sim !!!

EXAMPLE: 

-->BEFORE:  
- check scenarios .json in /scenarios (type and time of attack, scenario duration, etc) 
- check files to copy and VM addresses (see copy_files.py)
- create you own version of CSS_Test.sh, Ex. CSS_Test1.sh

--> launch terminator (double-click on desktop icon)
cd Scenario_Manager
./CSS_Test1.sh

--> in /tmp of VM MCS you will find:

- a .pcap for each selected scenario
- a new folder for each selected scenario with the files you decided to copy (ex. logids) 





