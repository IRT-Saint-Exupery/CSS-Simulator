import logging
import time

import spacepacket

import cfdp
from cfdp.transport.spacepacket import SpacePacketTransport
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore

import sys

sourceFile = sys.argv[1]
destFile = sys.argv[2]
destPort = int(sys.argv[3])
inapid = int(sys.argv[4])


logging.basicConfig(level=logging.DEBUG)

udp_transport = UdpTransport(routing={"*": [("127.0.0.1", destPort)]})
#udp_transport.bind("127.0.0.1", 5111)

spacepacket_transport = SpacePacketTransport(
    apid=inapid, transport=udp_transport, packet_type=spacepacket.PacketType.TELECOMMAND
)


cfdp_entity = cfdp.CfdpEntity(
    entity_id=1,
    #filestore=NativeFileStore("../files/local"),
    filestore=NativeFileStore("/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf"),
    transport=spacepacket_transport,
)

transaction_id = cfdp_entity.put(
    destination_id=2,
    source_filename=sourceFile,
    destination_filename=destFile,
    transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
)

while not cfdp_entity.is_complete(transaction_id):
    time.sleep(0.1)

input("Press <Enter> to finish.\n")

cfdp_entity.shutdown()
#udp_transport.unbind()
