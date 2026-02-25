import os
import types
import time

import cfdp
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore
from cfdp.pdu import PduHeader
from cfdp.constants import PduTypeCode

from utils import RemoteEntityUdp

"""
===============================================================================
                        Test Series F4
===============================================================================
"""

"""
===============================================================================
Test Seq 1 - Check Deferred NAK mode
===============================================================================
Purpose: Check Defered NAK mode

- Establish a two-way connection between client and server
- Sending of medium.txt to server
- dropping of ~5% of the Data

Expected Behavior:
- reordering with NakPdu after EOF (Deferred)

Check: The test is passed, when the medium file exists in client files.

"""


def test_seq_1():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()
    # modify the nak expiration limit for the local entity
    remote_entity.cfdp_entity.nak_timer_expiration_limit = 10

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )
    # modify transport request to drop PDUs
    outgoing_filedata_count = 0

    def modified_request(self, pdu):
        nonlocal outgoing_filedata_count
        pdu_header = PduHeader.decode(pdu)

        # determine type of PDU
        if pdu_header.pdu_type == PduTypeCode.FILE_DATA:
            outgoing_filedata_count += 1
            if outgoing_filedata_count % 2 == 0:  # drop every n PDU
                return

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)
                self._socket.sendto(pdu, (host, port))

    cfdp_entity.transport.request = types.MethodType(modified_request, cfdp_entity.transport)

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/local_medium.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    )

    while not cfdp_entity.is_complete(transaction_id) or not remote_entity.cfdp_entity.is_complete(
            transaction_id
    ):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/local_medium.txt")
    os.remove("./files/remote/local_medium.txt")


"""
Test Seq 2 - Check Immediate NAK mode
- Not Implemented

Test Seq 3 - Check Prompted NAK mode
- Not Implemented

Test Seq 4 - Check Asynchronous NAK mode
- Not Implemented

Test Seq 5 - Check Segmentation Control (record boundaries observed)
- Not Implemented

Test Seq 6 - Check no Segmentation Control (record boundaries not observed).
- Default Method
- Tested before

"""

"""
===============================================================================
Test Seq 7 - Check Sender initiated Suspend and Resume functioning
===============================================================================
Purpose: Check Sender initiated Suspend and Resume Request

- Establish a two-way connection between client and server
- Sending of medium.txt to server
- Suspend request at mid-file
- Wait 10 seconds
- Resume Request

Expected Behavior:
- Suspend Request -> pausing of current process
- Resume Request -> Continueing of process

Check: The test is passed, when the medium file exists in client files.

"""


def test_seq_7():
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

    time.sleep(0.01)
    cfdp.logger.info("Transaction suspend.")
    cfdp_entity.suspend(transaction_id)
    cfdp_entity.put(
        destination_id=2,
        transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
        messages_to_user=[cfdp.RemoteSuspendRequest(*transaction_id)],
    )

    time.sleep(10)  # wait for some time
    cfdp.logger.info("Transaction resume.")
    cfdp_entity.put(
        destination_id=2,
        transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
        messages_to_user=[cfdp.RemoteResumeRequest(*transaction_id)],
    )
    cfdp_entity.resume(transaction_id)

    while not cfdp_entity.is_complete(transaction_id) or not remote_entity.cfdp_entity.is_complete(
            transaction_id
    ):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/medium.txt")
    os.remove("./files/remote/medium.txt")


"""
===============================================================================
Test Seq 8 - Check Receiver initiated Suspend and Resume functioning
===============================================================================
Purpose: Check Receiver initiated Suspend and Resume Request

- Establish a two-way connection between client and server
- Sending of medium.txt to server
- Suspend request at mid-file
- Wait 10 seconds
- Resume Request

Expected Behavior:
- Suspend Request -> pausing of current process
- Resume Request -> Continueing of process

Check: The test is passed, when the medium file exists in client files.

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
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
    )

    time.sleep(0.01)
    cfdp.logger.info("Transaction suspend.")
    remote_entity.cfdp_entity.suspend(transaction_id)
    remote_entity.cfdp_entity.put(
        destination_id=1,
        transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
        messages_to_user=[cfdp.RemoteSuspendRequest(*transaction_id)],
    )

    time.sleep(10)  # wait for some time
    cfdp.logger.info("Transaction resume.")
    remote_entity.cfdp_entity.put(
        destination_id=1,
        transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
        messages_to_user=[cfdp.RemoteResumeRequest(*transaction_id)],
    )
    remote_entity.cfdp_entity.resume(transaction_id)

    while not cfdp_entity.is_complete(transaction_id) or not remote_entity.cfdp_entity.is_complete(
        transaction_id
    ):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/medium.txt")
    os.remove("./files/remote/medium.txt")


"""
===============================================================================
Test Seq 9 - Check Unbounded file type - Not implemented
===============================================================================
Purpose: Sends a file the length of which is not known when transmission is
         initiated (intended primarily for real-time data)

- Establish a two-way connection between client and server
- Sending of medium.txt to server (in MetaData no length is given)

Expected Behavior:
- Sending of file without knowing the length

Check: The test is passed, when the medium file exists in client files.

"""
"""
Test Seq 10 - Check File Data PDU CRC mode
- Not Implemented

Test Seq 11 - Check Keep Alive function
- Not Implemented

Test Seq 12 - Check Prompt (Keep Alive)
- Not Implemented

"""


"""
===============================================================================
Test Seq 13 - Check Multiple Open Transactions (clean)
===============================================================================
Purpose: Open Transactions in a sequence that causes five Transactions to
         be open at the same time.

- Establish a two-way connection between client and server
- Sending of medium.txt to server (5 times)

Expected Behavior:
- sending of 5 files to client at the same time

Check: The test is passed, when the 5 files exist in client files.

"""


def test_seq_13():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    for i in range(5):
        transaction_id = cfdp_entity.put(
            destination_id=2,
            source_filename="/medium.txt",
            destination_filename="/local_medium_{}.txt".format(i),
            transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        )

    while not cfdp_entity.is_complete(transaction_id) or not remote_entity.cfdp_entity.is_complete(
        transaction_id
    ):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    for i in range(5):
        assert os.path.isfile("./files/remote/local_medium_{}.txt".format(i))
        os.remove("./files/remote/local_medium_{}.txt".format(i))


"""
===============================================================================
Test Seq 14 - Check Multiple Open Transactions (w/ data loss)
===============================================================================
Purpose: Open Transactions in a sequence that causes five Transactions to
         be open at the same time.

- Establish a two-way connection between client and server
- setting InactivityTimerTimeout to 60 for this test
- Sending of medium.txt to server (5 times)
- ~5% of data dropped

Expected Behavior:
- sending of 5 files to client at the same time
- dropping every 10 FiledataPdu

Check: The test is passed, when the 5 files exist in client files.

"""


def test_seq_14():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    # modify transport request to drop PDUs
    outgoing_filedata_count = 0

    def modified_request(self, pdu):
        nonlocal outgoing_filedata_count
        pdu_header = PduHeader.decode(pdu)

        # determine type of PDU
        if pdu_header.pdu_type == PduTypeCode.FILE_DATA:
            outgoing_filedata_count += 1
            if outgoing_filedata_count % 10 == 0:  # drop every n PDU
                return

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)
                self._socket.sendto(pdu, (host, port))

    cfdp_entity.transport.request = types.MethodType(modified_request, cfdp_entity.transport)

    for i in range(5):
        transaction_id = cfdp_entity.put(
            destination_id=2,
            source_filename="/medium.txt",
            destination_filename="/local_medium_{}.txt".format(i),
            transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        )

    while not cfdp_entity.is_complete(transaction_id) or not remote_entity.cfdp_entity.is_complete(
        transaction_id
    ):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    for i in range(5):
        assert os.path.isfile("./files/remote/local_medium_{}.txt".format(i))
        os.remove("./files/remote/local_medium_{}.txt".format(i))


if __name__ == "__main__":
    import logging

    logging.basicConfig(level=logging.DEBUG)

    print("Test Seq 1 " + 50 * "=")
    test_seq_1()

    print("Test Seq 7 " + 50 * "=")
    test_seq_7()

    print("Test Seq 8 " + 50 * "=")
    test_seq_8()

    print("Test Seq 13 " + 50 * "=")
    test_seq_13()

    print("Test Seq 14 " + 50 * "=")
    test_seq_14()
