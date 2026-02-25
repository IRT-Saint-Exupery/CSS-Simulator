import os
import types
import time

import cfdp
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore
from cfdp.pdu import PduHeader
from cfdp.constants import PduTypeCode, DirectiveCode, DirectiveSubTypeCode

from utils import RemoteEntityUdp

"""
===============================================================================
                        Test Series F2
===============================================================================
"""


"""
===============================================================================
Test Seq 1 - Check recovery from loss of Metadata PDU
===============================================================================
Purpose: Check recovery from loss of first MetaData PDU

- Establish a two-way connection between client and server
- Sending of the medium.txt file to server
- Prevent sending of first MetaData PDU

Expected Behavior:
- first MetaData is dropped
- reordered with NAK after sending the FiledataPdus

Check: The test is passed, when the medium file exists in server files.

"""


def test_seq_1():
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

        if outgoing_pdu_count == 1:  # drop metadata PDU
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
Test Seq 2 - Check recovery from loss of EOF PDU
===============================================================================
Purpose: Check recovery from loss of first EOF PDU

- Establish a two-way connection between client and server
- Sending of the medium.txt file to server
- Prevent sending of first EOF PDU

Expected Behavior:
- first EofPdu is dropped
- start of the ACK timer (Sender)
- resending after timerexpiration

Check: The test is passed, when the medium file exists in server files.

"""


def test_seq_2():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    # modify transport request to drop PDUs
    eofpdu_count = 0

    def modified_request(self, pdu):
        nonlocal eofpdu_count

        pdu_header = PduHeader.decode(pdu)

        # determine type of PDU
        if pdu_header.pdu_type == PduTypeCode.FILE_DIRECTIVE:
            code = pdu[len(pdu_header)]
            if code == DirectiveCode.EOF:
                eofpdu_count += 1
                if eofpdu_count == 1:  # drop first EOF PDU
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
Test Seq 3 - Check recovery from loss of Finished PDU
===============================================================================
Purpose: Check recovery from loss of first finished PDU

- Establish a two-way connection between client and server
- Sending of the medium.txt file to server
- Prevend sending of first finished PDU

Expected Behavior:
- first FinishedPdu is dropped
- start of ACK timer (Receiver)
- resending after Timerexpiration

Check: The test is passed, when the medium file exists in server files.

"""


def test_seq_3():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    # modify transport request to drop PDUs
    finishedpdu_count = 0

    def modified_request(self, pdu):
        nonlocal finishedpdu_count

        pdu_header = PduHeader.decode(pdu)

        # determine type of PDU
        if pdu_header.pdu_type == PduTypeCode.FILE_DIRECTIVE:
            code = pdu[len(pdu_header)]
            if code == DirectiveCode.FINISHED:
                finishedpdu_count += 1
                if finishedpdu_count == 1:  # drop first finished PDU
                    return

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)
                self._socket.sendto(pdu, (host, port))

    remote_entity.cfdp_entity.transport.request = types.MethodType(
        modified_request, remote_entity.cfdp_entity.transport
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
Test Seq 4 - Check recovery from loss of ACK(EOF) PDU
===============================================================================
Purpose: Check recovery from loss of first ACK(EOF) PDU

- Establish a two-way connection between client and server
- Sending of the medium.txt file to server
- Prevend sending of first ACK(EOF) PDU

Expected Behavior:
- start of ACK timer (Sender)
- receiving EOF from Sender
- first ACK(EOF) is dropped
- resending EOFPdu because of ACK Timerexpiration on Sender
- reordering of ACK(EOF) from Receiver

Check: The test is passed, when the medium file exists in server files.

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
    ack_eofpdu_count = 0

    def modified_request(self, pdu):
        nonlocal ack_eofpdu_count
        pdu_header = PduHeader.decode(pdu)

        # determine type of PDU
        if pdu_header.pdu_type == PduTypeCode.FILE_DIRECTIVE:
            code = pdu[len(pdu_header)]
            if code == DirectiveCode.ACK:  # 06
                pdu_data = pdu[len(pdu_header) :]
                directive_subtype_code = pdu_data[1] & 0x0F
                if directive_subtype_code == DirectiveSubTypeCode.ACK_OTHERS:
                    ack_eofpdu_count += 1
                    if ack_eofpdu_count == 1:  # drop first ACK(EOF) PDU
                        return

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)
                self._socket.sendto(pdu, (host, port))

    remote_entity.cfdp_entity.transport.request = types.MethodType(
        modified_request, remote_entity.cfdp_entity.transport
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
Test Seq 5 - Check recovery from loss of ACK(Finished) PDU
===============================================================================
Purpose: Check recovery from loss of first ACK(Finished) PDU

- Establish a two-way connection between client and server
- Sending of the medium.txt file to server
- Prevend sending of first ACK(Finished) PDU

Expected Behavior:
- start of ACK timer (Receiver)
- receiving FinishedPdu from Receiver
- first ACK(Finished) is dropped
- resending FinishedPdu because of ACK Timerexpiration on Receiver
- reordering of ACK(Finished) from Sender


Check: The test is passed, when the medium file exists in server files.

"""


def test_seq_5():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    # modify transport request to drop PDUs
    ack_finishedpdu_count = 0

    def modified_request(self, pdu):
        nonlocal ack_finishedpdu_count
        pdu_header = PduHeader.decode(pdu)

        # determine type of PDU
        if pdu_header.pdu_type == PduTypeCode.FILE_DIRECTIVE:
            code = pdu[len(pdu_header)]
            if code == DirectiveCode.ACK:
                pdu_data = pdu[len(pdu_header) :]
                directive_subtype_code = pdu_data[1] & 0x0F
                if directive_subtype_code == DirectiveSubTypeCode.ACK_FINISHED:
                    ack_finishedpdu_count += 1
                    if ack_finishedpdu_count == 1:  # drop first ACK(Finished)
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
Test Seq 6 - Check operation in extremely noisy environment
===============================================================================
Purpose: Check operation with dropping every PDU except EOF in each direction

- Establish a two-way connection between client and server
- Sending of the medium.txt file to server
- Prevend sending of every first PDU except of EOF

Expected Behavior:
- first MetaData is dropped (Sender)
- Receive EOF (Receiver)
- first ACK(EOF) is dropped (Receiver) -> Start of ACKTimer (Sender)
- reodering after Timerexpiration and resending of EOFPdu
- reordering of MetaData with NAK after sending the FiledataPdus (Receiver)
- first NAKPdu is dropped -> Start of NAKTimer (Receiver)
- NAK Timerexpiration
  -> resending of NAKPDU with reordering of MetadataPDU (Receiver)
- first FinishedPdu is dropped -> start of ACK timer (Receiver)
- resending of FinishedPdu after Timerexpiration (Receiver)
- first ACK(Finished) is dropped (Sender)
- resending FinishedPdu because of ACK Timerexpiration (Receiver)
- reordering of ACK(Finished) from Sender


Check: The test is passed, when the medium file exists in server files.

"""


def test_seq_6():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    # modify transport request to drop PDUs
    # [MetaData, Nak, ACK(EOF), Finished, ACK(Fin)]
    outgoing_pdu_count = [0, 0, 0, 0, 0]

    def modified_request(self, pdu):
        nonlocal outgoing_pdu_count
        outgoing_pdu_count[0] += 1

        if outgoing_pdu_count[0] == 1:  # drop metadata PDU
            return

        pdu_header = PduHeader.decode(pdu)

        # determine type of PDU
        if pdu_header.pdu_type == PduTypeCode.FILE_DIRECTIVE:
            code = pdu[len(pdu_header)]
            if code == DirectiveCode.FINISHED:  # 05
                outgoing_pdu_count[3] += 1
                if outgoing_pdu_count[3] == 1:
                    return
            elif code == DirectiveCode.ACK:  # 06
                pdu_data = pdu[len(pdu_header) :]
                directive_subtype_code = pdu_data[1] & 0x0F
                if directive_subtype_code == DirectiveSubTypeCode.ACK_OTHERS:
                    outgoing_pdu_count[2] += 1
                    if outgoing_pdu_count[2] == 1:  # drop first ACK(EOF) PDU
                        return
                elif directive_subtype_code == DirectiveSubTypeCode.ACK_FINISHED:
                    outgoing_pdu_count[4] += 1
                    if outgoing_pdu_count[4] == 1:  # drop first ACK(Finished)
                        return
            elif code == DirectiveCode.NAK:  # 08
                outgoing_pdu_count[1] += 1
                if outgoing_pdu_count[1] == 1:
                    return

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)
                self._socket.sendto(pdu, (host, port))

    cfdp_entity.transport.request = types.MethodType(
        modified_request, cfdp_entity.transport
    )

    remote_entity.cfdp_entity.transport.request = types.MethodType(
        modified_request, remote_entity.cfdp_entity.transport
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
Test Seq 7 - Check response to ACK Limit Reached at sender
===============================================================================
Purpose: Drop all ACK and Finished PDUs to cause fault at sender

- Establish a two-way connection between client and server
- Sending of the medium.txt file to server
- Prevend sending of every ACKPdu and FinishedPdu

Expected Behavior:
- all ACK and FinishedPdus are dropped -> Start of ACKTimer (sender)
- reodering of ACKPdu while Timerexpiration
- Timerexpirationlimit is reached
- fault handler at sender -> CANCEL REQUEST

Check: The test is passed, when the medium file exists in server files.

"""


def test_seq_7():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    # modify transport request to drop PDUs
    def modified_request(self, pdu):
        pdu_header = PduHeader.decode(pdu)

        # determine type of PDU
        if pdu_header.pdu_type == PduTypeCode.FILE_DIRECTIVE:
            code = pdu[len(pdu_header)]
            if code == DirectiveCode.ACK:  # 06
                return
            elif code == DirectiveCode.FINISHED:  # 05
                return

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)
                self._socket.sendto(pdu, (host, port))

    remote_entity.cfdp_entity.transport.request = types.MethodType(
        modified_request, remote_entity.cfdp_entity.transport
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
Test Seq 8 - Check response to NAK Limit Reached at receiver
===============================================================================
Purpose: Drop all NAK PDUs to cause fault at receiver
        Receiving entity needs to set Inactivity Timer appropriately.

- Establish a two-way connection between client and server
- Sending of the medium.txt file to server
- Prevend sending of every NAKPdu

Expected Behavior:
- drop one FiledataPdu
- all NAKPdu are dropped -> Start of NAKTimer (Receiver)
- reodering of NAKPdu while Timerexpiration
- Timerexpirationlimit is reached
- fault handler at Receiver -> CANCEL REQUEST with deletion of data


Check: The test is passed, when the medium file doesn't exists in server files.

"""


def test_seq_8():
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
            if (outgoing_filedata_count == 1) or (outgoing_filedata_count == 4):
                return
        elif pdu_header.pdu_type == PduTypeCode.FILE_DIRECTIVE:
            code = pdu[len(pdu_header)]
            if code == DirectiveCode.NAK:  # 08
                return

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)
                self._socket.sendto(pdu, (host, port))

    cfdp_entity.transport.request = types.MethodType(
        modified_request, cfdp_entity.transport
    )

    remote_entity.cfdp_entity.transport.request = types.MethodType(
        modified_request, remote_entity.cfdp_entity.transport
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
    assert not os.path.isfile("./files/remote/medium.txt")


"""
===============================================================================
Test Seq 9 - Check Inactivity Timer at sender
===============================================================================
Purpose: Drop all Finished PDUs from receiver

- Establish a two-way connection between client and server
- Sending of the medium.txt file to server
- Prevend sending of every FinishedPdu

Expected Behavior:
- start of Inactivity Timer at Sender
- all FinishedPdus are dropped
- Timerexpiration and Inactivity fault handler -> CANCEL Request

Check: The test is passed, when the medium file exists in server files.

"""


def test_seq_9():
    remote_entity = RemoteEntityUdp()
    remote_entity.up()

    udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5222)]})
    udp_transport.bind("127.0.0.1", 5111)

    cfdp_entity = cfdp.CfdpEntity(
        entity_id=1, filestore=NativeFileStore("./files/local"), transport=udp_transport
    )

    # modify transport request to drop PDUs
    def modified_request(self, pdu):
        pdu_header = PduHeader.decode(pdu)

        # determine type of PDU
        if pdu_header.pdu_type == PduTypeCode.FILE_DIRECTIVE:
            code = pdu[len(pdu_header)]
            if code == DirectiveCode.FINISHED:  # 05
                return

        if self.routing and "*" in self.routing:
            for address in self.routing["*"]:
                host, port = address
                port = int(port)
                self._socket.sendto(pdu, (host, port))

    remote_entity.cfdp_entity.transport.request = types.MethodType(
        modified_request, remote_entity.cfdp_entity.transport
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
Test Seq 10 - Check Inactivity Timer at receiver
===============================================================================
Purpose: After file copy procedure starts, block all transmissions
            from sender

- Establish a two-way connection between client and server
- Sending of the medium.txt file to server
- Prevend sending of every FinishedPdu

Expected Behavior:
- start of Inactivity Timer at Sender
- all FinishedPdus are dropped
- Timerexpiration and Inactivity fault handler -> CANCEL Request

Check: The test is passed, when the medium file does not exists in remote.

"""


def test_seq_10():
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

        if outgoing_pdu_count >= 3:
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
    assert not os.path.isfile("./files/remote/medium.txt")


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

    print("Test Seq 7 " + 50 * "=")
    test_seq_7()

    print("Test Seq 8 " + 50 * "=")
    test_seq_8()

    print("Test Seq 9 " + 50 * "=")
    test_seq_9()

    print("Test Seq 10 " + 50 * "=")
    test_seq_10()
