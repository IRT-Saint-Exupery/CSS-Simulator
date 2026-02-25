import logging

import spacepacket

import cfdp
from cfdp.transport.spacepacket import SpacePacketTransport
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore

import sys

#sourceSat = sys.argv[1]
portcfdp = int(sys.argv[1])


logging.basicConfig(level=logging.DEBUG)

#udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5111)]})
#udp_transport.bind("127.0.0.1", 5222)

udp_transport = UdpTransport(routing={"*": [("127.0.0.1", portcfdp)]})
udp_transport.bind("127.0.0.1", portcfdp)

spacepacket_transport = SpacePacketTransport(
    apid=0xB6, transport=udp_transport, packet_type=spacepacket.PacketType.TELEMETRY
)

cfdp_entity = cfdp.CfdpEntity(
    entity_id=1,
    #filestore=NativeFileStore("../python-cfdp-main/examples/files/remote"),
    filestore=NativeFileStore("/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf"),
    transport=spacepacket_transport,
)

input("Running. Press <Enter> to stop...\n")

cfdp_entity.shutdown()
udp_transport.unbind()
