import os
import time

import spacepacket

import cfdp
from cfdp.transport.spacepacket import SpacePacketTransport
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore

from utils import RemoteEntitySpacePacket


def test_spacepacket():
    remote_entity = RemoteEntitySpacePacket()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    spacepacket_transport = SpacePacketTransport(
        apid=222,
        transport=udp_transport,
        packet_type=spacepacket.PacketType.TELECOMMAND,
    )

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=2,
        filestore=NativeFileStore("./files/local"),
        transport=spacepacket_transport,
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    )

    while not cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/medium.txt")
    os.remove("./files/remote/medium.txt")


if __name__ == "__main__":
    import logging

    logging.basicConfig(level=logging.DEBUG)

    print("Test spacepacket Transport " + 50 * "=")
    test_spacepacket()
