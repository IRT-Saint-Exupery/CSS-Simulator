How to use the Input Generator ? 

Step 1)
Example : python3 Main.py create scenarios/test.json   --> generate .json file corresponding to the desired commands (follow the instructions in the terminal to find the desired command)

Step 2)
Adapt the .json file manually (if necessary) to modify the default parameters of the commands

Step 3)
Example: python3 Main.py send scenarios/test.json --> send the commands to cryptolib input port 


NB: 
if you use the "--port" option you can specify the port. 
Example: python3 Main.py send scenarios/test.json --port 6012 --> send the commands to cryptolib sat2  input port 6012

NB: 
To be used on the VM representing the MCS!

NB:
Fuzzing and timestamp fields development is in progress.
The feature is not fully available.  

NB: 
you can create combinations of commands directly with the create argument or by using a dedicated script.
Example of dedicated script: python3 Init_mission.py 6010 --> send a series of commands to port 6010
