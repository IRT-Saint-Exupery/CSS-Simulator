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

transaction_id = cfdp_entity.put(
    destination_id=2,
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    filestore_requests=[
        # directory actions
        cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_DIRECTORY, "/A"),
        cfdp.FilestoreRequest(cfdp.ActionCode.REMOVE_DIRECTORY, "/A"),
        cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_DIRECTORY, "/B"),
        cfdp.FilestoreRequest(cfdp.ActionCode.DENY_DIRECTORY, "/B"),
        # file actions
        cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_FILE, "a.txt"),
        cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_FILE, "b.txt"),
        cfdp.FilestoreRequest(cfdp.ActionCode.APPEND_FILE, "a.txt", "b.txt"),
        cfdp.FilestoreRequest(cfdp.ActionCode.REPLACE_FILE, "a.txt", "b.txt"),
        cfdp.FilestoreRequest(cfdp.ActionCode.DELETE_FILE, "a.txt"),
        cfdp.FilestoreRequest(cfdp.ActionCode.DENY_FILE, "b.txt"),
    ],
)

input("Press <Enter> to finish.\n")

cfdp_entity.shutdown()
udp_transport.unbind()
