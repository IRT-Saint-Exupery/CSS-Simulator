# Generic Thruster - NOS3 Component
This repository contains the NOS3 Generic Thruster Component.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Overview
The generic thruster is a UART device that can be used to command the thruster percentage on.  The available flight software is for use in the core Flight System (cFS) while the ground software supports COSMOS.  A NOS3 simulation is available which uses a 42 data provider.

## Mechanical
### Reference System
The +x axis is in the direction of the thrust.  The +y and +z axes are perpendicular to the thruster, and together with the +z axis make an orthogonal, right-handed coordinate system.

# Device Communications
The protocol, commands, and responses of the component are captured below.

## Protocol
The protocol in use is UART 115200 8N1.
The device is speak when spoken too.
All communications with the device require / contain a header of 0xDEAD and a trailer of 0xBEEF.

## Commands
All commands received by the device are echoed back to the sender to confirm receipt.
Device commands are all formatted in the same manner and are fixed in size:
* uint16, 0xDEAD
* uint8, thruster number
* uint8, percentage of full thrust, 0-100
* uint16, 0xBEEF

# Running the Application
1. Run NOS3
1. Send the command to enable the thrusters
1. Send command(s) to set thruster (0 to 3) to a percentage of full thrust
1. Observe the spacecraft reacts and rotates and/or translates.

# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW
Refer to the file [fsw/platform_inc/generic_thruster_platform_cfg.h](fsw/platform_inc/generic_thruster_platform_cfg.h) for the default
configuration settings, as well as a summary on overriding parameters in mission-specific repositories.

## Simulation
## 42
The 42 data provider can be configured in the `nos3-simulator.xml`:
```
            <data-provider>
                <type>GENERIC_THRUSTER_42_PROVIDER</type>
                <hostname>fortytwo</hostname>
                <command-port>4280</command-port>
                <max-connection-attempts>0</max-connection-attempts>
                <retry-wait-seconds>1</retry-wait-seconds>
            </data-provider>
```

42 is initially configured with 4 thrusters, attached to 4 nodes of body 0 in the spacecraft SC_* file.

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
* v1.0.0 
  - Initial release with version tagging
