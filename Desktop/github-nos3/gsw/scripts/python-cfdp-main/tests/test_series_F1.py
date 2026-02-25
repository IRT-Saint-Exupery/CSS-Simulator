import os
import types
import time

import cfdp
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore
from cfdp.event import EventType

from utils import RemoteEntityUdp

"""
===============================================================================
                        Test Series F1
===============================================================================
"""

"""
===============================================================================
Test Seq 1 - Establish one-way connectivity
===============================================================================
Purpose: Check the one-way connection of the UNACKNOWLEDGED mode

- Establish a one-way connection from client to server
- Send the small.txt file to server

Expected Behavior:
- establishing of one-way connection
- sending of small.txt

Check: The test is passed, when the file is in the server files.

"""


def test_seq_1():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/small.txt",
        destination_filename="/small.txt",
        transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/small.txt")
    os.remove("./files/remote/small.txt")


"""
===============================================================================
Test Seq 2 - Exercise multiple File Data PDUs
===============================================================================
Purpose: Check the possibility to send multiple File Data PDUs
         in UNACKNOWLEDGED mode

- Establish a one-way connection from client to server
- Send the medium.txt file to server

Expected Behavior:
- establishing of one-way connection
- sending of medium.txt

Check: The test is passed, when the medium file is in the server files.

"""


def test_seq_2():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/medium.txt")
    os.remove("./files/remote/medium.txt")


"""
===============================================================================
Test Seq 3 - Establish two-way connectivity and establish performance baseline
===============================================================================
Purpose: Check the two-way connection of the ACKNOWLEDGED mode and proofe
         the basic functionality

- Establish a two-way connection between client and server
- Send the medium.txt file to server

Expected Behavior:
- establishing of two-way connection
- sending of medium.txt

Check: The test is passed, when the medium file is in the server files.

"""


def test_seq_3():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/medium.txt")
    os.remove("./files/remote/medium.txt")


"""
===============================================================================
Test Seq 4 - Check recovery of dropped data
===============================================================================
Purpose: Check the Recovery of dropped data in the ACKNOWLEDGED mode

- Establish a two-way connection between client and server
- Send the medium.txt file to server
- dropping of ~1% of the File Data PDUs in the sending process

Expected Behavior:
- dropping of FiledataPdus
- reodering with NakPdu

Check: The test is passed, when the medium file in the server files
       is complete.

"""


def test_seq_4():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    # modify transport request to drop PDUs
    outgoing_pdu_count = 0

    def modified_request(self, pdu):
        nonlocal outgoing_pdu_count
        outgoing_pdu_count += 1

        if outgoing_pdu_count == 5:  # drop this PDU
            return
        if outgoing_pdu_count == 10:  # drop this PDU
            return

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)
                self._socket.sendto(pdu, (host, port))

    cfdp_entity.transport.request = types.MethodType(
        modified_request, cfdp_entity.transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/medium.txt")
    os.remove("./files/remote/medium.txt")


"""
===============================================================================
Test Seq 5 - Check deletion of duplicate data
===============================================================================
Purpose: Check deletion of duplicated data.

- Establish a two-way connection between client and server
- Send the medium.txt file to server
- Delete ~1% of the File Data PDUs in the sending process

Expected Behavior:
- sending duplicated FiledataPdu
- deletion of excess Pdus

Check: The test is passed, when the medium file in the server files
       is complete.
"""


def test_seq_5():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    # modify transport request to duplicate PDUs
    outgoing_pdu_count = 0

    def modified_request(self, pdu):
        nonlocal outgoing_pdu_count
        outgoing_pdu_count += 1

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)

                if outgoing_pdu_count == 5:  # duplicate this PDU
                    self._socket.sendto(pdu, (host, port))
                if outgoing_pdu_count == 10:  # duplicate this PDU
                    self._socket.sendto(pdu, (host, port))
                self._socket.sendto(pdu, (host, port))

    cfdp_entity.transport.request = types.MethodType(
        modified_request, cfdp_entity.transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/medium.txt")
    os.remove("./files/remote/medium.txt")


"""
===============================================================================
Test Seq 6 - Check reordering of data
===============================================================================
Purpose: Check reording of data sent in the wrong order

- Establish a two-way connection between client and server
- Send the medium.txt file to server
- Sending ~1% of the File Data PDUs out of order

Expected Behavior:
- mixed sending of FiledataPdus
- reodering of FiledataPdus

Check: The test is passed, when the medium file in the server files
       is complete and in the right order.
"""


def test_seq_6():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    # modify transport request to duplicate PDUs
    outgoing_pdu_count = 0
    buffer = []

    def modified_request(self, pdu):
        nonlocal outgoing_pdu_count
        nonlocal buffer
        outgoing_pdu_count += 1

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)

                if outgoing_pdu_count == 5:  # pass one PDU and save it for chaos pdu
                    buffer.append(pdu)
                    return
                if outgoing_pdu_count == 8:  # pass one PDU and save it for chaos pdu
                    time.sleep(1)
                    buffer.append(pdu)
                    return
                elif outgoing_pdu_count == 7:  # send the old_data and the current
                    self._socket.sendto(buffer.pop(), (host, port))
                    self._socket.sendto(pdu, (host, port))
                elif outgoing_pdu_count == 12:  # send the old_data and the current
                    self._socket.sendto(buffer.pop(), (host, port))
                    self._socket.sendto(pdu, (host, port))
                else:
                    self._socket.sendto(pdu, (host, port))

    cfdp_entity.transport.request = types.MethodType(
        modified_request, cfdp_entity.transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/medium.txt")
    os.remove("./files/remote/medium.txt")


"""
===============================================================================
Test Seq 7_1 - Check user (application) messages functioning - ProxyPutRequest
===============================================================================
Purpose: Check the functioning of 2 user (application) messages.
         In the first case check of the ProxyPutRequest

- Establish a two-way connection between client and server
- Send the ProxyPutRequest to the server and request the medium.txt
- Sending of the medium.txt from server to client

Expected Behavior:
- ordering of medium.txt from server
- sending medium.txt from server to client

Check: The test is passed, when the medium file exists in client files.

"""


def test_seq_7_1():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        messages_to_user=[
            cfdp.ProxyPutRequest(
                destination_entity_id=1,
                source_filename="/remote_medium.txt",
                destination_filename="/local_medium.txt",
            )
        ],
    )

    # time.sleep(1)  # wait for proxy operation to start
    # while not cfdp_entity.is_complete(
    #     transaction_id
    # ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
    #     time.sleep(0.1)
    n = 0
    while not os.path.isfile("./files/local/local_medium.txt"):
        time.sleep(1)
        n += 1
        if n > 5:
            break
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/local/local_medium.txt")
    os.remove("./files/local/local_medium.txt")


"""
===============================================================================
Test Seq 7_2 - Check user (application) messages functioning - Cancel
===============================================================================
Purpose: Check the functioning of 2 user (application) messages.
         In the second case check the ProxyPutCancel

- Establish a two-way connection between client and server
- Send the ProxyPutRequest to the server and request the medium.txt
- Sending of the medium.txt from server to client
- Send the ProxyPutCancel to the server

Expected Behavior:
- CancelRequest from Client
- Canceling of the current process

Check: The test is passed, when the medium file doesn´t exist in client
       files, because of canceling.

"""


def test_seq_7_2():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    cfdp_entity.put(
        destination_id=2,
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        messages_to_user=[
            cfdp.ProxyPutRequest(
                destination_entity_id=1,
                source_filename="/remote_medium.txt",
                destination_filename="/local_medium.txt",
            )
        ],
    )

    transaction_id = None
    while transaction_id is None:
        transaction_ids = list(cfdp_entity.machines.keys())
        for n in transaction_ids:
            if n[0] == 2:
                transaction_id = n
                break  # transfer from remote has started

    cfdp_entity.put(
        destination_id=2,
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        messages_to_user=[cfdp.ProxyPutCancel(*transaction_id)],
    )

    print("Transaction cancelled.")
    time.sleep(1)

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert not os.path.isfile("./files/local/local_medium.txt")


"""
===============================================================================
Test Seq 8 - Check cancel functioning (Sender initiated
approximately mid-file)
===============================================================================
Purpose: Check the sender initiated cancel functioning of ACKNOWLEDGED mode.

- Establish a two-way connection between client and server
- Send the medium.txt file to server
- approximately mid-file canceling the sending process from sender/client side

Expected Behavior:
- Sending of CancelRequest from Sender/Client
- Cancelling of process

Check: The test is passed, when the medium file doesn´t exist in Sender
       files, because of canceling.
"""


def test_seq_8():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/large.txt",
        destination_filename="/local_large.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    )

    # modify the process of sending data
    # cancel function within the 8 File Data PDU
    file_data_count = 0

    def modified_trigger_event(self, event, pdu=None):
        nonlocal file_data_count
        self._event_queue.put([event, pdu])
        if event.type == EventType.E1_SEND_FILE_DATA:
            file_data_count += 1
            if file_data_count == 8:
                cfdp.logger.info("Transaction cancelled.")
                cfdp_entity.cancel(transaction_id)

    cfdp_entity.trigger_event = types.MethodType(modified_trigger_event, cfdp_entity)

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert not os.path.isfile("./files/remote/local_large.txt")


"""
===============================================================================
Test Seq 9 - Check cancel functioning (Receiver initiated
approximately mid-file)
===============================================================================
Purpose: Check the Receiver initiated cancel functioning of ACKNOWLEDGED mode.

- Establish a two-way connection between client and server
- Send the medium.txt file to server
- approximately mid-file canceling the sending process from
  server/Receiver side

Expected Behavior:
- Sending of CancelRequest from Server/Receiver
- Cancelling of process

Check: The test is passed, when the medium file doesn´t exist in Sender
       files, because of canceling.
"""


def test_seq_9():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/large.txt",
        destination_filename="/local_large.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    )

    # modify the process of sending data
    # cancel function within the 8 File Data PDU
    file_data_count = 0

    def modified_trigger_event(self, event, pdu=None):
        nonlocal file_data_count
        self._event_queue.put([event, pdu])
        if event.type == EventType.E11_RECEIVED_FILEDATA:
            file_data_count += 1
            if file_data_count == 8:
                cfdp.logger.info("Transaction cancelled.")
                remote_entity.cfdp_entity.cancel(transaction_id)

    remote_entity.cfdp_entity.trigger_event = types.MethodType(
        modified_trigger_event, remote_entity.cfdp_entity
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert not os.path.isfile("./files/remote/local_large.txt")


"""
===============================================================================
Test Seq 10 - Check cancel functioning (Sender initiated
approximately mid-file)
===============================================================================
Purpose: Check the sender initiated cancel functioning of UNACKNOWLEDGED mode.

- Establish a one-way connection from client to server
- Send the medium.txt file to server
- approximately mid-file canceling the sending process from sender/client side

Expected Behavior:
- Sending of CancelRequest from Sender/Client
- Cancelling of process

Check: The test is passed, when the medium file doesn´t exist in Sender
       files, because of canceling.
"""


def test_seq_10():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/local_medium.txt",
        transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    )

    # modify the process of sending data
    # cancel function within the 8 File Data PDU
    file_data_count = 0

    def modified_trigger_event(self, event, pdu=None):
        nonlocal file_data_count
        self._event_queue.put([event, pdu])
        if event.type == EventType.E1_SEND_FILE_DATA:
            file_data_count += 1
            if file_data_count == 8:
                cfdp.logger.info("Transaction cancelled.")
                cfdp_entity.cancel(transaction_id)

    cfdp_entity.trigger_event = types.MethodType(modified_trigger_event, cfdp_entity)

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert not os.path.isfile("./files/remote/local_medium.txt")


if __name__ == "__main__":
    import logging

    logging.basicConfig(level=logging.DEBUG)

    print("Test Seq 1 " + 50 * "=")
    test_seq_1()

    print("Test Seq 2 " + 50 * "=")
    test_seq_2()

    print("Test Seq 3 " + 50 * "=")
    test_seq_3()

    print("Test Seq 4 " + 50 * "=")
    test_seq_4()

    print("Test Seq 5 " + 50 * "=")
    test_seq_5()

    print("Test Seq 6 " + 50 * "=")
    test_seq_6()

    print("Test Seq 7_1 " + 50 * "=")
    test_seq_7_1()

    print("Test Seq 7_2 " + 50 * "=")
    test_seq_7_2()

    print("Test Seq 8 " + 50 * "=")
    test_seq_8()

    print("Test Seq 9 " + 50 * "=")
    test_seq_9()

    print("Test Seq 10 " + 50 * "=")
    test_seq_10()
