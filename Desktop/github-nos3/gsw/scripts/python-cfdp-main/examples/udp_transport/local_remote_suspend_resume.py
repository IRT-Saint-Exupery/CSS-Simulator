import logging
import time

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
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    messages_to_user=[
        cfdp.ProxyPutRequest(
            destination_entity_id=1,
            source_filename="/medium.txt",
            destination_filename="/medium.txt",
        )
    ],
)

transaction_id = None
while transaction_id is None:
    transaction_ids = list(cfdp_entity.machines.keys())
    for n in transaction_ids:
        if n[0] == 1:
            transaction_id = n
            break  # transfer from remote has started

cfdp_entity.put(
    destination_id=2,
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    messages_to_user=[cfdp.RemoteSuspendRequest(*transaction_id)],
)

print("Transaction suspended.")
time.sleep(4)

cfdp_entity.put(
    destination_id=2,
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    messages_to_user=[cfdp.RemoteResumeRequest(*transaction_id)],
)

print("Transaction resumed.")
time.sleep(0.1)

cfdp_entity.put(
    destination_id=2,
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    messages_to_user=[cfdp.RemoteSuspendRequest(*transaction_id)],
)

print("Transaction suspended.")
time.sleep(4)

cfdp_entity.put(
    destination_id=2,
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    messages_to_user=[cfdp.ProxyPutCancel(*transaction_id)],
)

print("Transaction cancelled.")
time.sleep(1)

cfdp_entity.shutdown()
udp_transport.unbind()
