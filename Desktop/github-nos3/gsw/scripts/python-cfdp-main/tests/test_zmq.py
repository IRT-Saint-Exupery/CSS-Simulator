import os
import time

import cfdp
from cfdp.transport.zmq import ZmqTransport
from cfdp.filestore import NativeFileStore

from utils import RemoteEntityZmq


def test_zmq_unacknowledged():
    remote_entity = RemoteEntityZmq()
    remote_entity.up()

    zmq_transport = ZmqTransport()
    zmq_transport.connect("127.0.0.1", 5222)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=zmq_transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    )

    while not cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    cfdp_entity.shutdown()
    zmq_transport.disconnect()
    remote_entity.down()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/medium.txt")
    os.remove("./files/remote/medium.txt")


def test_zmq_acknowledged():
    remote_entity = RemoteEntityZmq()
    remote_entity.up()

    zmq_transport = ZmqTransport()
    zmq_transport.connect("127.0.0.1", 5222)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=zmq_transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    )

    while not cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    cfdp_entity.shutdown()
    zmq_transport.disconnect()
    remote_entity.down()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/medium.txt")
    os.remove("./files/remote/medium.txt")


if __name__ == "__main__":
    import logging

    logging.basicConfig(level=logging.DEBUG)

    print("Test Zmq Transport " + 50 * "=")
    test_zmq_unacknowledged()
    time.sleep(0.1)
    test_zmq_acknowledged()
