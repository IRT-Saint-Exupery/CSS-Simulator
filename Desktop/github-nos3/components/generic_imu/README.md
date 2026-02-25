# Generic_imu - NOS3 Component
This repository contains the NOS3 Generic_imu Component.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Overview
This generic_imu component is a CAN device that accepts multiple commands, including requests for telemetry and data.
The available FSW is for use in the core Flight System (cFS) while the GSW supports COSMOS.
A NOS3 simulation is available which includes both generic_imu and 42 data providers.

An inertial measurement unit, or IMU, is an electrical unit which uses a combination of accelerometers and gyroscopes to
report a vehicle's attitude, heading, and rotation. 
Upon a command, it provides information about the attitude, heading, and rotation of the vehicle.
The available flight software is for use in the core Flight System (cFS) while the ground software supports COSMOS.
A NOS3 simulation is available which uses a 42 data provider.


# Device Communications
The protocol, commands, and responses of the component are captured below.

## Protocol
The protocol in use is CAN.
The device is speak when spoken too.

## Commands
All commands received by the device are echoed back to the sender to confirm receipt.
Should commmands involve a reply, the device immediately sends the reply after the command echo.
Device commands are all formatted in the same manner and are fixed in size:
* uint8, command header
  - 0x80
* uint8, command identifier
  - (0) Get Housekeeping
  - (1) Get Generic_imu

## Response
Response formats are as follows:
* Housekeeping
  - uint32, Command Counter
    * Increments for each command received
  - uint32, Configuration
    * Internal configuration number in use by the device
  - uint32, Status
    * Self reported status of the component where zero is completely healthy and each bit represents different errors
    * No means to clear / set status except for a power cycle to the device
* Generic_imu
  - uint32, Command Counter
    * Increments for each command received
  - uint16, Data X
    * X component of generic_imu data
  - uint16, Data Y
    * X component of generic_imu data
  - uint16, Data Z
    * X component of generic_imu data

# Device Communications (CAN)

## Communications
The NOS3 generic inertial measurement unit communicates using the CAN protocol.

### Data Format
The data structure for communications is explained below:
- Character transmission (unsigned char):  (Byte_0(LSB)) --> to communications channel
- Floating data transmission (float):  Float codification according to IEEE 754-1985 standard for single-precision floating 32 bits:  (sign|exponent|fraction) = (Byte_3(MSB)|Byte_2|Byte_1|Byte_0(LSB)) --> to communications channel
    - Floating value = s x 2^e x m, where:
        - Sign(s): 1 bit (0 = positive, 1 = negative)
        - Exponent: 8 bits (e = Exponent - 127)
        - Fraction: 23 bits (m = 1.Fraction)

### Frame Format
This protocol uses two message types:
- Request message: from master to slave
- Response message: from slave to master

The master is the onboard computer of the satellite, or the master of the communication bus, and the slave is the generic inertial
measurement unit.


### CAN Protocol Format
The CAN Protocol is the message-passing format which will be used in the case of the generic inertial measurement unit. A CAN frame
consists of a 16-bit header, a 4-bit data length variable, and then between 0 and 8 bytes of data. There is then a trailing unit of
28 bits. The following chart indicates the parts of the message:

Field Name     | Length (bits) | Purpose | Value (in this case)
|:-------------|:-------------:|:-------:|--------------------:|
Start of Frame | 1   | Denotes the start of frame transmission | 1
Identifier     | 11  | An identifier (message priority)        | 
Stuff Bit      | 1   | An opposing bit to maintain polarity    | 
Remote Transmission Request | 1 | 0 for data frames; 1 for remote request frames |
Identifier Extension Bit  | 1 | 0 for 11-bit identifier frames | 0
Reserved bit   | 1   | Reserved bit; must be dominant (0)      | 0
Data length    | 4   | Defines the length of the following     |
Data field     | <64 | Data to be transmitted (0-8 bytes)      |
CRC            | 15  | Cyclic Redundancy Check                 |
CRC delimeter  | 1   | Must be recessive (1)                   | 1
ACK slot       | 1   | Must be recessive (1)                   | 1
ACK delimeter  | 1   | Must be recessive (1)                   | 1
End-of-Frame   | 7   | Must be recessive (1)                   | 0000001
Inter-frame spacing | 3 | Must be recessive (1)                | 001

The above chart is primarily reproduced from [Wikipedia](https://en.wikipedia.org/wiki/CAN_bus).


### Commands (CMD)
There will be one command required:

#### Command 0x01:  Data request
This command will direct the IMU to return the data on both linear and rotational acceleration in all three axes.

Field Name     | Length (bits) | Purpose | Value (in this case)
|:-------------|:-------------:|:-------:|--------------------:|
Start of Frame | 1   | Denotes the start of frame transmission | 1
Identifier     | 11  | An identifier (message priority)        | 00000000001
Stuff Bit      | 1   | An opposing bit to maintain polarity    | 0
Remote Transmission Request | 1 | 0 for data frames; 1 for remote request frames | 1
Identifier Extension Bit  | 1 | 0 for 11-bit identifier frames | 0
Reserved bit   | 1   | Reserved bit; must be dominant (0)      | 0
Data length    | 4   | Defines the length of the following     | 0000
Data field     | 0   | Data to be transmitted                  | --
CRC            | 15  | Cyclic Redundancy Check                 | 000000000000000
CRC delimeter  | 1   | Must be recessive (1)                   | 1
ACK slot       | 1   | Must be recessive (1)                   | 1
ACK delimeter  | 1   | Must be recessive (1)                   | 1
End-of-Frame   | 7   | Must be recessive (1)                   | 0000001
Inter-frame spacing | 3 | Must be recessive (1)                | 001


#### Telemetry (TLM)
Because of size constraints on the message itself, there will be three telemetry messages. All three will be sent upon receipt
of the aforementioned command message.

#### Telemetry 0x02:  X Telemetry
This command will contain the telemetry from the IMU, including data on linear and rotational acceleration in all three axes.

Field Name     | Length (bits) | Purpose | Value (in this case)
|:-------------|:-------------:|:-------:|--------------------:|
Start of Frame | 1   | Denotes the start of frame transmission | 1
Identifier     | 11  | An identifier (message priority)        | 00000000010
Stuff Bit      | 1   | An opposing bit to maintain polarity    | 1
Remote Transmission Request | 1 | 0 for data frames; 1 for remote request frames | 0
Identifier Extension Bit  | 1 | 0 for 11-bit identifier frames | 0
Reserved bit   | 1   | Reserved bit; must be dominant (0)      | 0
Data length    | 4   | Defines the length of the following     | 1000
Data field     | 64 | Data to be transmitted                   | 32 bit float denoting linear X acceleration, 32 bit float denoting angular X acceleration
CRC            | 15  | Cyclic Redundancy Check                 | 
CRC delimeter  | 1   | Must be recessive (1)                   | 1
ACK slot       | 1   | Must be recessive (1)                   | 1
ACK delimeter  | 1   | Must be recessive (1)                   | 1
End-of-Frame   | 7   | Must be recessive (1)                   | 0000001
Inter-frame spacing | 3 | Must be recessive (1)                | 001

#### Telemetry 0x03:  Y Telemetry
This command will be identical to the X telemetry message, except that it will had an identifier of 00000000011, a stuff bit of 0,
and the data field will denote linear Y and angular Y accelerations, respectively.

#### Telemetry 0x04:  Z Telemetry
This command will be identical to the X telemetry message, except that it will had an identifier of 00000000100, a stuff bit of 1,
and the data field will denote linear Z and angular Z accelerations, respectively.


# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW
Refer to the file [fsw/platform_inc/generic_imu_platform_cfg.h](fsw/platform_inc/generic_imu_platform_cfg.h) for the default
configuration settings, as well as a summary on overriding parameters in mission-specific repositories.

## Simulation
The default configuration returns data that is X * 0.001, Y * 0.002, and Z * 0.003 the request count after conversions:
```
<simulator>
    <name>generic_imu_sim</name>
    <active>true</active>
    <library>libgeneric_imu_sim.so</library>
    <hardware-model>
        <type>GENERIC_IMU</type>
        <connections>
            <connection><type>command</type>
                <bus-name>command</bus-name>
                <node-name>generic_imu-sim-command-node</node-name>
            </connection>
            <connection><type>usart</type>
                <bus-name>usart_29</bus-name>
                <node-port>29</node-port>
            </connection>
        </connections>
        <data-provider>
            <type>GENERIC_IMU_PROVIDER</type>
        </data-provider>
    </hardware-model>
</simulator>
```

## 42
Optionally the 42 data provider can be configured in the `nos3-simulator.xml`:
```
        <data-provider>
            <type>GENERIC_IMU_42_PROVIDER</type>
            <hostname>localhost</hostname>
            <port>4242</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
        </data-provider>
```


# Documentation
If this generic_imu application had an ICD and/or test procedure, they would be linked here.

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
* v1.0.0 - X/Y/Z 
  - Updated to be a component repository including FSW, GSW, Sim, and Standalone checkout
* v0.1.0 - 10/9/2021 
  - Initial release with version tagging
