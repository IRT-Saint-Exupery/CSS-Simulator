# Generic Radio - NOS3 Component
This repository contains the NOS3 Generic Radio Component.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Overview
This generic radio component is a socket based device that accepts multiple commands, including requests for telemetry and data.
The available FSW is for use in the core Flight System (cFS) while the GSW supports COSMOS.
A NOS3 simulation is available which includes a generic_radio data provider.


# Device Communications
The protocol, commands, and responses of the component are captured below.
This section is essentially the device Interface Control Definition (ICD).

## Protocol
The protocol in use is UDP sockets.
The radio is maintains various sockets for use:
* Communication Interfaces
  - FSW to Radio Primary
    * UDP 5010
  - Radio Primary to FSW
    * UDP 5011
  - FSW to Radio Proximity
    * UDP 7010
  - Radio Proximity to FSW
    * UDP 7011
  - GSW to Radio
    * UDP 6010
  - Radio to GSW
    * UDP 6011
* Radio Component Interface
  - FSW to Radio
    * UDP 5014
  - Radio to FSW
    * UDP 5015

The radio will immediately attempt to forward data send on the communication interface with no storage capability.

## Commands
Should commmands involve a reply, the device immediately sends the reply.
Device commands are all formatted in the same manner and are fixed in size:
* uint16, 0xDEAD
* uint8, command identifier
  - (0) Get Housekeeping
  - (1) Set Configuration
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
    * Internal configuration settings used by the device
  - uint32, Proximity Signal Strength
    * Self reported status of the component where zero is no signal
  - uint16, 0xBEEF


# Radio Flight Software
The FSW enables an abstraction of the device communicatications captured above.
For the radio the FSW accepts the follopwing commands:
* GENERIC_RADIO_CMD_MID
  - (0) NOOP
  - (1) Reset Counters
  - (2) Set Configuration
  - (3) Proximity Forward
* GENERIC_RADIO_REQ_HK_MID
  - (0) Request Housekeeping


# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW
Refer to the file [fsw/platform_inc/generic_radio_platform_cfg.h](fsw/platform_inc/generic_radio_platform_cfg.h) for the default
configuration settings, as well as a summary on overriding parameters in mission-specific repositories.

## Simulation
The default configuration returns data initialized by the values in the simulation configuration settings used in the NOS3 simulator configuration file.
The radio configuration options for this are captured in [./sim/cfg/nos3-radio-simulator.xml](./sim/cfg/nos3-radio-simulator.xml) for ease of use.

# Documentation
If this generic_radio application had an ICD and/or test procedure, they would be linked here.

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
* v1.0.0
  - Initial release
