# Generic Reaction Wheel - NOS3 Simulator

This repository contains an example Generic Reaction Wheel NOS3 Simulator.

## Default NOS3 Config

    <simulator>
            <name>generic-reactionwheel-sim</name>
            <active>true</active>
            <library>libgeneric_rw_sim.so</library>
            <hardware-model>
                <type>GENERICREACTIONWHEELHARDWARE</type>
                <connections>
                    <connection><type>command</type><bus-name>command</bus-name><node-name>rw-command</node-name></connection>
                    <connection><type>usart</type>
                        <bus-name>usart_2</bus-name>
                        <node-port>2</node-port>
                    </connection>
                </connections>
                <data-provider>               
                    <type>GENERICRWSIMDATA42SOCKETPROVIDER</type>
                    <hostname>localhost</hostname>
                    <port>4277</port>
                    <command-port>4278</command-port>
                    <max-connection-attempts>5</max-connection-attempts>
                    <retry-wait-seconds>5</retry-wait-seconds>
                    <spacecraft>0</spacecraft>
                    <reactionwheel>0</reactionwheel>
                </data-provider>               
            </hardware-model>
    </simulator>

## Default 42 IPC Config
**********************************  IPC 1   *****************************
RX                                      ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)
0                                       ! AC.ID for ACS mode
"State01.42"                            ! File name for WRITE or READ
SERVER                                  ! Socket Role (SERVER,CLIENT,GMSEC_CLIENT)
localhost      4278                     ! Server Host Name, Port 
FALSE                                   ! Allow Blocking (i.e. wait on RX)
FALSE                                   ! Echo to stdout
1                                       ! Number of TX prefixes
"SC"                                    ! Prefix 0
**********************************  IPC 2   *****************************
TX                                      ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)
1                                       ! AC.ID for ACS mode
"State02.42"                            ! File name for WRITE or READ
SERVER                                  ! Socket Role (SERVER,CLIENT,GMSEC_CLIENT)
localhost      4277                     ! Server Host Name, Port 
FALSE                                   ! Allow Blocking (i.e. wait on RX)
FALSE                                   ! Echo to stdout
1                                       ! Number of TX prefixes
"SC"                                    ! Prefix 0


### Versioning
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.

## Legal 
If this project interests you or if you have any questions, please feel free to contact any developer.

### License
This project is licensed under the NOSA (NASA Open Source Agreement) License. 

### Acknowledgments
* Special thanks to all the developers involved!
