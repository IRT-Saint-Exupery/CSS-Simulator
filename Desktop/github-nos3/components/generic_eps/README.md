# Generic EPS - NOS3 Component
This repository contains the NOS3 Generic Electrical Power System (EPS) Component.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Overview
This generic EPS component is an I2C device that accepts commands to change switch state, request telemetry, and reset the spacecraft.
The available FSW is for use in the core Flight System (cFS) while the GSW supports COSMOS.
A NOS3 simulation is available which includes both generic_eps and 42 data providers.


# Device Communications
The protocol, commands, and responses of the component are captured below.
The EPS has 8 switches to turn components on and off.
Information for voltage, current, and status of each are provided. 
Additionally battery voltage, battery temperature and solar array voltage, solar array temperature.

## Protocol
The protocol in use is I2C at 1MHz with 7-bit address 0x2B. 

## Command Address
Device commands are all formatted in the same manner and are fixed in size.
Commands are confirmed only by verifying the expected switch state change in telemetry.
* uint8, Slave Address [7:1] and Read/Write [0]
* uint8, Command
* uint8, Data
* uint8, CRC
  - CRC8 of command and data

Command Table
* 0x00, Switch 0 State
  - Data of 0x00 for OFF, 0xAA for ON for all switches
* 0x01, Switch 1 State
* ...
* 0x07, Switch 7 State
* 0x70, Telemetry Request
  - Data field unused
* 0xAA, Reset
  - Data of 0xAA to trigger power cycle

## Telemetry
Telemetry reported is in big endian format (MSB x, LSB x+1) and is updated at a 1Hz frequency.
* Battery Pack
  - uint16, voltage
  - uint16, temperature
* EPS
  - uint16, 3.3 voltage
  - uint16, 5.0 voltage
  - uint16, 12.0 voltage
  - uint16, temperature
* Solar Array
  - uint16, voltage
  - uint16, temperature 
* Switch [0-7]
  - uint16, voltage
  - uint16, current
  - uint16, status
    * Bits [15:8] - Error flags, 0x00 is healthy
    * Bits [7:0]  - 0x00 is off, 0xAA is on
* uint8, CRC
  - CRC8 of the previous data

Conversion table
* Voltages = (uint16 value * 0.001V)
* Currents = (uint16 value * 0.001A)
* Temperatures = (uint16 value * 0.01C) - 60C


# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW
Refer to the file [fsw/platform_inc/generic_eps_platform_cfg.h](fsw/platform_inc/generic_eps_platform_cfg.h) for the default configuration settings, as well as a summary on overriding parameters in mission-specific repositories.

## Simulation
The default configuration returns data initialized by the values in the simulation configuration settings used in the NOS3 simulator configuration file.
The EPS configuration options for this are captured in [./sim/cfg/nos3-eps-simulator.xml](./sim/cfg/nos3-eps-simulator.xml) for ease of use.

## 42
Optionally the 42 data provider can be configured in the `nos3-simulator.xml`:
```
        <data-provider>
            <type>GENERIC_EPS_42_PROVIDER</type>
            <hostname>localhost</hostname>
            <port>4242</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
        </data-provider>
```


# Documentation
If this generic_eps application had an ICD and/or test procedure, they would be linked here.

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
* v1.0.0 - X/Y/Z 
  - Updated to be a component repository including FSW, GSW, Sim, and Standalone checkout
* v0.1.0 - 10/9/2021 
  - Initial release with version tagging
