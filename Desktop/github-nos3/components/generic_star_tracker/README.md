# Generic_star_tracker - NOS3 Component
This repository contains the NOS3 Generic_star_tracker Component.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Overview
This generic_star_tracker component is a UART device that accepts multiple commands, including requests for telemetry and data.
The available FSW is for use in the core Flight System (cFS) while the GSW supports COSMOS.
A NOS3 simulation is available which includes both generic_star_tracker and 42 data providers.


# Device Communications
The protocol, commands, and responses of the component are captured below.  The field of view is +/- 4 degrees.

Reference system:  The boresight is along the +z axis.  The x, y, and z axes are stamped on the sensor body.

## Protocol
The protocol in use is UART 115200 8N1.
The device is speak when spoken too.
All communications with the device require / contain a header of 0xDEAD and a trailer of 0xBEEF.

## Commands
All commands received by the device are echoed back to the sender to confirm receipt.
Should commmands involve a reply, the device immediately sends the reply after the command echo.
Device commands are all formatted in the same manner and are fixed in size:
* uint16, 0xDEAD
* uint8, command identifier
  - (0) Get Housekeeping
  - (1) Get Generic Star Tracker data
  - (2) Set Configuration
* uint32, command payload
  - Unused for all but set configuration command
* uint16, 0xBEEF

## Response
Response formats are as follows:
* Housekeeping
  - uint16, 0xDEAD
  - uint32, Command Counter
    * Increments for each command received
  - uint32, Configuration
    * Internal configuration number in use by the device
  - uint32, Status
    * Self reported status of the component where zero is completely healthy and each bit represents different errors
    * No means to clear / set status except for a power cycle to the device
  - uint16, 0xBEEF
* Generic_star_tracker
  - uint16, 0xDEAD
  - uint32, Command Counter
    * Increments for each command received
  - uint16, Data Q0
    * Q0 component of generic_star_tracker data
  - uint16, Data Q1
    * Q1 component of generic_star_tracker data
  - uint16, Data Q2
    * Q2 component of generic_star_tracker data
  - uint16, Data Q3
    * Q3 component of generic_star_tracker data
  - uint8, Valid flag
    * Indicates whether the quaternion is valid or not
  - uint16, 0xBEEF
* In order to interpret the generic star tracker quaternion data, each value must have 32768 subtracted from it and then be divided by 32767.  This is so that the data can be passed as a two byte unsigned integer.


# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW
Refer to the file [fsw/platform_inc/generic_star_tracker_platform_cfg.h](fsw/platform_inc/generic_star_tracker_platform_cfg.h) for the default
configuration settings, as well as a summary on overriding parameters in mission-specific repositories.  In addition, to build the flight software, the component must be listed in the ```APPLICATION_LIST``` setting of the cFS ```targets.cmake``` file (typically in a ```*_defs``` directory).  Also, to run the Star Tracker app as part of the flight software, the component must be listed in the cFE startup script (```*.scr```) file (also typically found in a ```*_defs``` directory).  A typical line to add the FSS app looks like:

```
CFE_APP, generic_st,                ST_AppMain,               ST,               71, 8192,  0x0, 0;
```

For more information on the format, please refer to the cFE startup script (```*.scr```) file.


## Simulation
Refer to the file [sim/cfg/nos3-StarTracker-simulator.xml](sim/cfg/nos3-StarTracker-simulator.xml) for the default configuration settings.  Note that the block of XML within this file should be placed within the `<simulators></simulators>` section of the main ```nos3-simulator.xml``` configuration file which is read by the ```sim_common code``` and used by the simulator.  For more information, refer to the example configuration file in ```nos3/sims/cfg/nos3-simulator.xml```.

## 42

The ```GENERIC_STAR_TRACKER_42_PROVIDER``` simulation data provider depends on receiving data for a star tracker over a TCP/IP socket (specified in the simulator configuration) from the 42 spacecraft simulator.  To provide this data, 42 must be configured with a star tracker block in the 42 spacecraft configuration file (typically named to begin with ```SC_``` but whose name is ultimately specified in the Spacecraft section of the 42 ```Inp_Sim.txt``` file).  Refer to the file [sim/cfg/SC_StarTracker_NOS3.txt](sim/cfg/SC_StarTracker_NOS3.txt) for what this configuration block should look like.  This block should be placed within the `************************** Star Tracker *********************************` section of the ```SC_*.xml``` configuration file which is read by 42 for spacecraft information.  For more information, refer to the example 42 spacecraft configuration file in ```nos3/sims/cfg/InOut/SC_NOS3.txt```.

In addition, the 42 ```Inp_IPC.txt``` configuration file must specify information about the TCP/IP socket connection to the simulation data provider.  Refer to the file [sim/cfg/Inp_StarTracker_IPC.txt](sim/cfg/Inp_StarTracker_IPC.txt) for what this configuration block should look like.  This block should be placed in the ```Inp_IPC.txt``` configuration file which is read by 42 for IPC information.  For more information, refer to the example 42 spacecraft configuration file in ```nos3/sims/cfg/InOut/Inp_IPC.txt```.

Important for communication with the simulation provider:
- The ```IPC Mode``` must be ```TX```
- The ```Socket Role``` must be ```SERVER```
- The ```Server Host Name, Port``` must be specified and match the setting in the simulator XML configuration file
- ```Allow Blocking``` must be ```FALSE```
- ```Prefix```(es) must be set based on the data to send from 42 and based on what is expected to be parsed by the simulation data provider

To get a better understanding of the data that can be provided by 42, try running 42 with ```Echo to stdout``` set to ```TRUE```, 1 ```TX``` prefix, and a prefix of ```SC```.  If you then run 42, the main terminal window will echo the data being sent out from 42 to the TCP/IP socket.  You can then choose the prefix(es) to filter out and determine how to parse the 42 data in the simulation data provider (or the data point that provides the interface between the simulation data provider and the simulation hardware model).


# Documentation
If this generic_star_tracker application had an ICD and/or test procedure, they would be linked here.

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
* v1.0.0 - 10/10/2023
  - Initial release of the generic star tracker component.
