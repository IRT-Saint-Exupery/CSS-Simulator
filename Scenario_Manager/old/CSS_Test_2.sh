#!/bin/bash

# S0
gnome-terminal --tab --title="Scenario0" -- bash -c "python3 Manager.py scenarios/S0.json; exec bash" &
echo "Scenario S0 in progress"
sleep 1000 # should be > time_end of S0
python3 copy_files.py /tmp/S0 > /dev/null
/usr/bin/sshpass -p nos3123! ssh nos3@192.168.100.5 "cp /home/nos3/eclipse-workspace/mission/src/Mission.png /tmp/S0/Mission_S0.png" #copy also Mission.png ?
echo "Scenario S0 completed"
sleep 10
####### if necessary (according to selected attacks) Do_Make_All before calling the next scenario
gnome-terminal --tab --title="Make All" -- bash -c "cd /home/nos3/Install_tools_nos3; ./Do_Make_All_VM.sh; exec bash"
sleep 100

# S1
gnome-terminal --tab --title="Scenario1" -- bash -c "python3 Manager.py scenarios/S1.json; exec bash" &
echo "Scenario 1 in progress"
sleep 700 # should be > time_end of S1
python3 copy_files.py /tmp/S1 > /dev/null
/usr/bin/sshpass -p nos3123! ssh nos3@192.168.100.5 "cp /home/nos3/eclipse-workspace/mission/src/Mission.png /tmp/S1/Mission_S1.png" #copy also Mission.png ?
echo "Scenario S1 completed"
sleep 10
####### if necessary (according to selected attacks) Do_Make_All before calling the next scenario
gnome-terminal --tab --title="Make All" -- bash -c "cd /home/nos3/Install_tools_nos3; ./Do_Make_All_VM.sh; exec bash"
sleep 100

#S2
gnome-terminal --tab --title="Scenario2" -- bash -c "python3 Manager.py scenarios/S2.json; exec bash"
echo "Scenario S2 in progress"
sleep 700 # should be > time_end of S2
python3 copy_files.py /tmp/S2 > /dev/null
/usr/bin/sshpass -p nos3123! ssh nos3@192.168.100.5 "cp /home/nos3/eclipse-workspace/mission/src/Mission.png /tmp/S2/Mission_S2.png" #copy also Mission.png ?
echo "Scenario S2 completed" 
sleep 10
######## if necessary (according to selected attacks) Do_Make_All before calling the next scenario
gnome-terminal --tab --title="Make All" -- bash -c "cd /home/nos3/Install_tools_nos3; ./Do_Make_All_VM.sh; exec bash"
sleep 100

#S3
gnome-terminal --tab --title="Scenario3" -- bash -c "python3 Manager.py scenarios/S3.json; exec bash"
echo "Scenario S3 in progress"
sleep 1000 # should be > time_end of S2
python3 copy_files.py /tmp/S3 > /dev/null
/usr/bin/sshpass -p nos3123! ssh nos3@192.168.100.5 "cp /home/nos3/eclipse-workspace/mission/src/Mission.png /tmp/S3/Mission_S3.png" #copy also Mission.png ?
echo "Scenario S3 completed" 
sleep 10
echo "All scenarios are completed!"
