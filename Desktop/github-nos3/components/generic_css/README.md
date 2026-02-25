# Generic_css - NOS3 Component
This repository contains the NOS3 Generic_css Component.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Overview
This generic_css component is an I2C device that accepts multiple commands including requests for telemetry. 
The available FSW is for use in the core Flight System (cFS) while the GSw supports COSMOS.
A NOS3 simulation is available which includes both generic_css and 42 data providers.

# Device Communications
The protocol, commands, and responses of the component are captured below.

## Protocol
The protocol in use is Inter-Integrated Circuit (I2C).
I2C is a synchronous serial communication protocol that supports slaves being controlled by multiple masters, as well as masters controlling multiple slaves.
This is a two wire communication protocol with one wire used for data and the other used for a clock signal.
For our purposes we will treat the generic_css as a single slave controlled by a master software bus.
I2C transfers data using messages, and these message are broken up into frames.
Data sent in data frames are always MSB first.

The generic_css device is simple in function.
It is addressed by the master software bus, and then it responds with the latest data value for each of the 6 ADCs contained within.

## Commands
The command formats are as follows:
* uint8, Address
  * CSS Address is 0b01000000 or 0x40

## Response
Telemetry is returned in big endian format.
Response formats are as follows:
* Telemetry
  * uint16, Voltage [0-5]
* Conversion table
  * Voltages = (uint16 value * 0.001 V)


# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW

## Simulation

## 42


# Documentation
If this generic_css application had an ICD and/or test procedure, they would be linked here.

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
