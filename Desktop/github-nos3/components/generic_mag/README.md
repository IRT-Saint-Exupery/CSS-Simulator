# Generic_mag - NOS3 Component
This repository contains the NOS3 Generic_mag Component.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Overview
This generic_mag component is an SPI device that accepts multiple commands including requests for telemetry. 
The available FSW is for use in the core Flight System (cFS) while the GSw supports COSMOS.
A NOS3 simulation is available which includes a 42 data provider.

# Device Communications
The protocol, commands, and responses of the component are captured below.

## Protocol
The protocol in use is Serial Peripheral Interface (SPI). The generic_mag is slave to the SPI master bus on the spacecraft and operates on chip select 2.

## Commands
There are no commands for the generic_mag beyond the basic app commands available to all NOS3 components.

## Response
Response formats are as follows:
* Generic_mag
  - uint16, 0xDEAD
  - uint16, 0xBEEF
  - int32, Data X
    * X component of generic_mag data
  - int32, Data Y
    * Y component of generic_mag data
  - int32, Data Z
    * Z component of generic_mag data


# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW
Refer to the file [fsw/platform_inc/generic_mag_platform_cfg.h](fsw/platform_inc/generic_mag_platform_cfg.h) for the default
configuration settings, as well as a summary on overriding parameters in mission-specific repositories.

## Simulation
```
<simulator>
    <name>generic_mag_sim</name>
    <active>true</active>
    <library>libgeneric_mag_sim.so</library>
    <hardware-model>
        <type>GENERIC_MAG</type>
        <connections>
            <connection><type>command</type>
                <bus-name>command</bus-name>
                <node-name>mag-command</node-name>
            </connection>
            <connection><type>spi</type>
                <bus-name>spi_2</bus-name>
                <chip-select>2</chip-select>
            </connection>
        </connections>
        <data-provider>
            <type>GENERIC_MAG_42_PROVIDER</type>
            <hostname>localhost</hostname>
            <port>4234</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
        </data-provider>
    </hardware-model>
</simulator>
```

## 42
Optionally the 42 data provider can be configured in the `nos3-simulator.xml`:
```
        <data-provider>
            <type>GENERIC_MAG_42_PROVIDER</type>
            <hostname>localhost</hostname>
            <port>4234</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
        </data-provider>
```


# Documentation
If this generic_mag application had an ICD and/or test procedure, they would be linked here.

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
* v1.0.0 - X/Y/Z 
  - Updated to be a component repository including FSW, GSW, Sim, and Standalone checkout
* v0.1.0 - 10/9/2021 
  - Initial release with version tagging
