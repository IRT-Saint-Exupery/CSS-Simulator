import logging

import cfdp
from cfdp.transport.zmq import ZmqTransport
from cfdp.filestore import NativeFileStore


logging.basicConfig(level=logging.DEBUG)

zmq_transport = ZmqTransport()
zmq_transport.bind("127.0.0.1", 5222)

cfdp_entity = cfdp.CfdpEntity(
    entity_id=2, filestore=NativeFileStore("../files/remote"), transport=zmq_transport
)

input("Running. Press <Enter> to stop...\n")

cfdp_entity.shutdown()
zmq_transport.unbind()
