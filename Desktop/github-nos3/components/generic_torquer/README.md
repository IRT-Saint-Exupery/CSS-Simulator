# Generic Torquer – NOS3 Component
This repository contains the NOS3 Generic Torquer Component. This includes flight software (fsw), ground software (gsw), and simulation (sim) directories.

## Overview
Torquers, or magnetorquers, are commonly used on spacecraft as a secondary attitude control mechanism. They consist of electromagnets placed asymmetrically. Different currents running through them then generate different magnetic fields, and the torque created by the interaction of the spacecraft magnetic field and the Earth’s magnetic field rotates the vehicle. Because of the small amounts of torque available, magnetorquers are only infrequently used as the primary attitude control mechanism; they frequently serve as a secondary mechanism, though, and are used to provide small rotational changes or to gradually unload reaction wheels. 
The generic torquer is a device which accepts as input a current (as a percent of the maximum) and returns a value of the generated magnetic field. 
The available flight software is for use in the core Flight System (cFS) while the ground software supports COSMOS.
A NOS3 simulation is available which uses a 42 data provider.

## Mechanical
### Reference System
The +z axis is down the length of the magnetorquer (in the same direction as the coil). The +x and +y axes are perpendicular to the coil, and together with the +z axis make an orthogonal, right-handed coordinate system.

## Communications
The NOS3 Generic Torquer communicates by means of the hardware library (hwlib) defined in [hwlib/tree/dev](https://github.com/nasa-itc/hwlib/tree/dev). Specifically, it makes use of two functions from the torquer library at [libtrq.h](https://github.com/nasa-itc/hwlib/blob/dev/fsw/public_inc/libtrq.h) to send commands to the torquers. These commands wrap the specific functionality of an FPGA or other interface which is used to send a PWM signal.


### Commands
There are two commands defined in libtrq which this generic torquer uses. They are:
- trq_set_direction(trq_info_t* device, bool direction)
- trq_command(trq_info_t *device, uint8_t percent_high, bool pos_dir)


#### Command trq_set_direction
Command for the current to the torquers to be set in one direction or the other.
- Command
    - trq_info_t* device = the address of the device being controlled
    - bool direction = the direction the current takes through the torquer

- Response
    - int32_t response = an integer which indicates the success or failure of the command

#### Command trq_command
Command for a torquer to be set to a percentage of its maximum power.  
- Command
    - trq_info_t* device = the address of the device being controlled
    - uint8_t percent_high = percent power to send through the torquer
    - bool direction = the direction the current should take

- Response
    - int32_t response = an integer which indicates the success or failure of the command

Error codes:
- 0 = No error.  The torquer is set to the given power.
- 1 = Error.  The magnetorquer was not able to be set to the given power value for some reason.


# Software Configuration

## Flight Software
Refer to the file [fsw/platform_inc/generic_torquer_app_platform_cfg.h](fsw/platform_inc/generic_torquer_app_platform_cfg.h) for the default configuration settings to the component hardware bus, as well as a summary on overriding parameters in mission-specific repositories.  In addition, to build the flight software, the component must be listed in the ```APPLICATION_LIST``` setting of the cFS ```targets.cmake``` file (typically in a ```*_defs``` directory).  Also, to run the generic_torquer app as part of the flight software, the component must be listed in the cFE startup script (```*.scr```) file (also typically found in a ```*_defs``` directory).  A typical line to add the generic torquer app looks like:

```
CFE_APP, /cf/generic_torquer.so,	GENERIC_TORQUER_AppMain,  TORQUER,          60, 8192,  0x0, 0;
```

For more information on the format, please refer to the cFE startup script (```*.scr```) file.


## Simulator
Refer to the file [sim/cfg/nos3-torquer-simulator.xml](sim/cfg/nos3-torquer-simulator.xml) for the default configuration settings.  Note that the block of XML within this file should be placed within the `<simulators></simulators>` section of the main ```nos3-simulator.xml``` configuration file which is read by the ```sim_common code``` and used by the simulator.  For more information, refer to the example configuration file in ```nos3/sims/cfg/nos3-simulator.xml```.

## 42
The ```GENERIC_TORQUER_42_PROVIDER``` simulation data provider depends on receiving data for a fine sun sensor over a TCP/IP socket (specified in the simulator configuration) from the 42 spacecraft simulator.  To provide this data, 42 must be configured with a fine sun sensor block in the 42 spacecraft configuration file (typically named to begin with ```SC_``` but whose name is ultimately specified in the Spacecraft section of the 42 ```Inp_Sim.txt``` file).  Refer to the file [sim/cfg/SC_torquer_NOS3.txt](sim/cfg/SC_torquer_NOS3.txt) for what this configuration block should look like.  This block should be placed within the `************************* Generic Torquer *******************************` section of the ```SC_*.xml``` configuration file which is read by 42 for spacecraft information.  For more information, refer to the example 42 spacecraft configuration file in ```nos3/sims/cfg/InOut/SC_NOS3.txt```.

In addition, the 42 ```Inp_IPC.txt``` configuration file must specify information about the TCP/IP socket connection to the simulation data provider.  Refer to the file [sim/cfg/Inp_torquer_IPC.txt](sim/cfg/Inp_torquer_IPC.txt) for what this configuration block should look like.  This block should be placed in the ```Inp_IPC.txt``` configuration file which is read by 42 for IPC information.  For more information, refer to the example 42 spacecraft configuration file in ```nos3/sims/cfg/InOut/Inp_IPC.txt```.

Important for communication with the simulation provider:
- The ```IPC Mode``` must be ```TX```
- The ```Socket Role``` must be ```SERVER```
- The ```Server Host Name, Port``` must be specified and match the setting in the simulator XML configuration file
- ```Allow Blocking``` must be ```FALSE```
- ```Prefix```(es) must be set based on the data to send from 42 and based on what is expected to be parsed by the simulation data provider

To get a better understanding of the data that can be provided by 42, try running 42 with ```Echo to stdout``` set to ```TRUE```, 1 ```TX``` prefix, and a prefix of ```SC```.  If you then run 42, the main terminal window will echo the data being sent out from 42 to the TCP/IP socket.  You can then choose the prefix(es) to filter out and determine how to parse the 42 data in the simulation data provider (or the data point that provides the interface between the simulation data provider and the simulation hardware model).
