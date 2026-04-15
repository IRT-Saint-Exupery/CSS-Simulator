#!/bin/bash

# Launch the first scenario and capture its PID
gnome-terminal --tab --title="Scenario1" -- bash -c "python3 Manager.py scenarios/S1.json; exec bash" &
echo "Scenario 1 in progress"
sleep 700 # should be > time_end of S1
python3 copy_files.py /tmp/S1 > /dev/null
echo "Scenario 1 completed"

## if necessary (according to selected attacks) Do_Make_All before calling the next scenario
gnome-terminal --tab --title="Make All" -- bash -c "cd /home/nos3/Install_tools_nos3; ./Do_Make_All_VM.sh; exec bash"
sleep 100

gnome-terminal --tab --title="Scenario2" -- bash -c "python3 Manager.py scenarios/S2.json; exec bash"
echo "Scenario 2 in progress"
sleep 700 # should be > time_end of S2
python3 copy_files.py /tmp/S2 > /dev/null
echo "Scenario 2 completed" 

echo "All scenarios are completed!"
