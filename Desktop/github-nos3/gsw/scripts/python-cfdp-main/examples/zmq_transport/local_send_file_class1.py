import logging
import time

import cfdp
from cfdp.transport.zmq import ZmqTransport
from cfdp.filestore import NativeFileStore


logging.basicConfig(level=logging.DEBUG)

zmq_transport = ZmqTransport()
zmq_transport.connect("127.0.0.1", 5222)

cfdp_entity = cfdp.CfdpEntity(
    entity_id=1, filestore=NativeFileStore("../files/local"), transport=zmq_transport
)

transaction_id = cfdp_entity.put(
    destination_id=2,
    source_filename="/medium.txt",
    destination_filename="/medium.txt",
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
)

# uncomment the sections below to test suspend and freeze

# time.sleep(0.1)
# cfdp_entity.freeze(receiving_entity_id=2)
# input("Transaction frozen. Press enter to unfreeze")
# cfdp_entity.thaw(receiving_entity_id=2)

# time.sleep(0.1)
# cfdp_entity.suspend(transaction_id)
# input("Transaction suspended. Press enter to resume")
# cfdp_entity.resume(transaction_id)

# time.sleep(0.1)
# cfdp_entity.suspend(transaction_id)
# input("Transaction suspended. Press enter to cancel")
# cfdp_entity.cancel(transaction_id)

while not cfdp_entity.is_complete(transaction_id):
    time.sleep(0.1)

input("Press <Enter> to finish.\n")

cfdp_entity.shutdown()
zmq_transport.disconnect()
