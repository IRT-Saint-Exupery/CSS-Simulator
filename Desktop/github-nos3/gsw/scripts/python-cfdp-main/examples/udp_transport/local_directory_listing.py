import logging

import cfdp
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore


logging.basicConfig(level=logging.DEBUG)

udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
udp_transport.bind("127.0.0.1", 5111)

cfdp_entity = cfdp.CfdpEntity(
    entity_id=1, filestore=NativeFileStore("../files/local"), transport=udp_transport
)

cfdp_entity.put(
    destination_id=2,
    transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    messages_to_user=[
        cfdp.DirectoryListingRequest(
            remote_directory="/", local_file="/.listing.remote"
        )
    ],
)

input("Press <Enter> to finish.\n")

cfdp_entity.shutdown()
udp_transport.unbind()
