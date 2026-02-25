# NovAtel OEM 615 - cFS Application

This repository contains the NovAtel OEM 615 cFS application.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Documentation
Currently support exists for the following versions:
* [OEM615 Dual-Frequency GNSS Receiver](https://www.novatel.com/products/gnss-receivers/oem-receiver-boards/oem6-receivers/oem615/)

## Overview
This novatel_oem615 component is a USART device that accepts multiple commands including requests for telemetry. 
The available FSW is for use in the core Flight System (cFS) while the GSw supports COSMOS.
A NOS3 simulation is available which includes a 42 data provider.

# Device Communications
The protocol, commands, and responses of the component are captured below.

## Protocol
The protocol in use is Universal Synchronous/Asynchronous Receiver/Transmitter (USART). The novatel_oem615 operates on the usart_1 bus on the spacecraft under handle/node-port 1.

## Commands
The commands for the novatel_oem615 include LOG, LOGALL, UNLOG, and SERIALCONFIG, as well as generic commands expected of all NOS3 components (NOOP, Request HK, Request Data, RESET_COUNTERS, ENABLE, DISABLE). Note that RESET_COUNTERS, ENABLE, and DISABLE do not communicate with the sim.

## Response
Response formats are as follows:
* Generic Commands (Hex)
  - uint16, 0xDEAD
  - uint8, command code
  - uint32, payload
  - uint16, 0xBEEF
* Novatel Commands (ASCII)
  - LOG COM [log type] ONTIME [period options]
    * [log types] = [BESTXYZA, GPGGAA, RANGECMPA, BESTXYZB, RANGECMPB]
    * [period options] = [ONCE, 0.05, 0.1, 0.2, 0.25, 0.5]
  - UNLOG COM [log type]
    * [log types] = [BESTXYZA, GPGGAA, RANGECMPA, BESTXYZB, RANGECMPB]
  - UNLOGALL
  - SERIALCONFIG

# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW
Refer to the file [fsw/platform_inc/novatel_oem615_platform_cfg.h](fsw/platform_inc/novatel_oem615_platform_cfg.h) for the default
configuration settings, as well as a summary on overriding parameters in mission-specific repositories.

## Simulation
```
<simulator>
    <name>gps</name>
    <active>true</active>
    <library>libgps_sim.so</library>
    <hardware-model>
        <type>OEM615</type>
        <connections>
            <!-- <connection><type>command</type><bus-name>command</bus-name><node-name>gps-command</node-name></connection> -->
            <connection><type>usart</type>
                <bus-name>usart_1</bus-name>
                <node-port>1</node-port>
            </connection>
        </connections>
        <data-provider>               
            <type>GPS42SOCKET</type>
            <hostname>localhost</hostname>
            <port>4245</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
            <GPS>0</GPS>
            <leap-seconds>37</leap-seconds>
        </data-provider>               
        <!-- <data-provider>               
            <type>GPSFILE</type>
            <filename>gps_data.42</filename>
        </data-provider> -->
    </hardware-model>
</simulator>
```

## 42
Optionally the 42 data provider can be configured in the `nos3-simulator.xml`:
```
        <data-provider>               
            <type>GPS42SOCKET</type>
            <hostname>localhost</hostname>
            <port>4245</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
            <GPS>0</GPS>
            <leap-seconds>37</leap-seconds>
        </data-provider>      
```

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
* v1.0.0 - X/Y/Z 
  - Updated to be a component repository including FSW, GSW, Sim, and Standalone checkout
* v0.1.0 - 10/9/2021 
  - Initial release with version tagging

## Legal 
If this project interests you or if you have any questions, please feel free to contact any developer.

### License
This project is licensed under the NOSA (NASA Open Source Agreement) License. 

### Acknowledgments
* Special thanks to all the developers involved!
