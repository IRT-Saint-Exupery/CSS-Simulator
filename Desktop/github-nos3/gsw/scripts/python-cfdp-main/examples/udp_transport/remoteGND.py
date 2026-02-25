import logging

import cfdp
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore

import sys


sourceSat = sys.argv[1]
portcfdp = int(sys.argv[2])

logging.basicConfig(level=logging.DEBUG)

udp_transport = UdpTransport(routing={"*": [(sourceSat, portcfdp)]})
udp_transport.bind("192.168.100.5", portcfdp)

cfdp_entity = cfdp.CfdpEntity(
    entity_id=1, filestore=NativeFileStore("../python-cfdp-main/examples/files/remote"), transport=udp_transport
)

input("Running. Press <Enter> to stop...\n")

cfdp_entity.shutdown()
udp_transport.unbind()
