import os
import time

import cfdp
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore

from utils import RemoteEntityUdp

"""
===============================================================================
                        Test Series F3
===============================================================================
"""

"""
===============================================================================
Test Seq 1 - Check two party Remote Put (acts as a ‘Get’)
===============================================================================
Purpose: Check Proxy Operation: ProxyPutRequest

- Establish a two-way connection between client and server
- Send the ProxyPutRequest to the server and request the medium.txt
- Sending of the medium.txt from server to client

Expected Behavior:
- Remote Get from Client
- Sending file from Server to client

Similar to test_series_F1.py: test_sequence 7_1

Check: The test is passed, when the medium file exists in client files.

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
    # time.sleep(1)
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
Test Seq 2 - Check Create File directive
===============================================================================
Purpose: Check filestore requests

- Establish a two-way connection between client and server
- Filestore_request: Create File

Expected Behavior:
- Put of filestore request: Create File "remote_file.txt"
- Creation of file

Check: The test is passed, when the file exists in the filestore.

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
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        filestore_requests=[
            # file actions
            cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_FILE, "remote_file.txt")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/remote_file.txt")
    os.remove("./files/remote/remote_file.txt")


"""
===============================================================================
Test Seq 3 - Check Delete File directive
===============================================================================
Purpose: Check filestore requests

- Establish a two-way connection between client and server
- Filestore_request: Create File
- Filestore_request: Delete File

Expected Behavior:
- Put of filestore request: Create File "remote_file.txt"
- Creation of file
- Put of filestore request: Delete File "remote_file.txt"
- Deletion of file

Check: The test is passed, when the file doesn't exist in the filestore.

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
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        filestore_requests=[
            # file actions
            cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_FILE, "remote_file.txt")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/remote_file.txt")

    transaction_id = cfdp_entity.put(
        destination_id=2,
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        filestore_requests=[
            # file actions
            cfdp.FilestoreRequest(cfdp.ActionCode.DELETE_FILE, "remote_file.txt")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert not os.path.isfile("./files/remote/remote_file.txt")


"""
===============================================================================
Test Seq 4 - Check Rename File directive
===============================================================================
Purpose: Check filestore requests

- Establish a two-way connection between client and server
- Filestore_request: Create File
- Filestore_request: Rename File

Expected Behavior:
- Put of filestore request: Create File "remote_file.txt"
- Creation of file
- Put of filestore request: Rename File "remote_file.txt" to "new_file.txt"
- Renaming of file

Check: The test is passed, when the file "new_file.txt" exist in the filestore.

"""


def test_seq_4():
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
        filestore_requests=[
            # file actions
            cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_FILE, "remote_file.txt"),
            cfdp.FilestoreRequest(
                cfdp.ActionCode.RENAME_FILE, "remote_file.txt", "new_file.txt"
            ),
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/new_file.txt")
    os.remove("./files/remote/new_file.txt")


"""
===============================================================================
Test Seq 5 - Check Append File directive
===============================================================================
Purpose: Check filestore requests

- Establish a two-way connection between client and server
- Filestore_request: Create File "remote_file.txt"
- Put Request: "medium.txt"
- Filestore_request: Append File "medium.txt" to "remote_file.txt"
- Filestore_request: Delete File "medium.txt"

Expected Behavior:
- Put of filestore request: Create File "remote_file.txt"
- Creation of empty file
- Sending of medium-file to Server
- Put of filestore request: Append File "medium.txt" to "remote_file.txt"
- Appending of file
- Put of filestore request: Delete File "medium.txt"
- Deletion of file

Check: The test is passed, when the medium-file exist in the filestore
       as "remote_file.txt".

"""


def test_seq_5():
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
        filestore_requests=[
            # file actions
            cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_FILE, "remote_file.txt")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    time.sleep(0.1)

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        filestore_requests=[
            # file actions
            cfdp.FilestoreRequest(
                cfdp.ActionCode.APPEND_FILE, "remote_file.txt", "medium.txt"
            ),
            cfdp.FilestoreRequest(cfdp.ActionCode.DELETE_FILE, "medium.txt"),
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/remote_file.txt")
    os.remove("./files/remote/remote_file.txt")


"""
===============================================================================
Test Seq 6 - Check Replace File directive
===============================================================================
Purpose: Check filestore requests

- Establish a two-way connection between client and server
- Filestore_request: Create File "remote_file.txt"
- Put Request: "medium.txt"
- Filestore_request: Replace File "medium.txt" as "remote_file.txt"
- Filestore_request: Delete File "medium.txt"

Expected Behavior:
- Put of filestore request: Create File "remote_file.txt"
- Creation of empty file
- Sending of medium-file to Server
- Put of filestore request: Replace File "medium.txt" as "remote_file.txt"
- Replace of file
- Put of filestore request: Delete File "medium.txt"
- Deletion of file

Check: The test is passed, when the medium-file exist in the filestore
       as "remote_file.txt".

"""


def test_seq_6():
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
        filestore_requests=[
            # file actions
            cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_FILE, "remote_file.txt")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    time.sleep(0.1)

    transaction_id = cfdp_entity.put(
        destination_id=2,
        source_filename="/medium.txt",
        destination_filename="/medium.txt",
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        filestore_requests=[
            # file actions
            cfdp.FilestoreRequest(
                cfdp.ActionCode.REPLACE_FILE, "remote_file.txt", "medium.txt"
            ),
            cfdp.FilestoreRequest(cfdp.ActionCode.DELETE_FILE, "medium.txt"),
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/remote_file.txt")
    os.remove("./files/remote/remote_file.txt")


"""
===============================================================================
Test Seq 7 - Check Create Directory directive
===============================================================================
Purpose: Check filestore requests

- Establish a two-way connection between client and server
- Filestore_request: Create Directory "/test_directory"

Expected Behavior:
- Put of filestore request: Create Directory "/test_directory"
- Creation of directory

Check: The test is passed, when the directory exists.

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
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        filestore_requests=[
            # directory actions
            cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_DIRECTORY, "/test_directory")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)

    time.sleep(0.1)
    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isdir("./files/remote/test_directory")
    os.rmdir("./files/remote/test_directory")


"""
===============================================================================
Test Seq 8 - Check Remove Directory directive
===============================================================================
Purpose: Check filestore requests

- Establish a two-way connection between client and server
- Filestore_request: Create Directory "/test_directory"
- Filestore_request: Remove Directory "/test_directory"

Expected Behavior:
- Put of filestore request: Create Directory "/test_directory"
- Creation of directory
- Put of filestore request: Remove Directory "/test_directory"
- Deletion of directory

Check: The test is passed, when the directory doesn´t exists.

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
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        filestore_requests=[
            # directory actions
            cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_DIRECTORY, "/test_directory")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    time.sleep(0.1)
    assert os.path.isdir("./files/remote/test_directory")

    transaction_id = cfdp_entity.put(
        destination_id=2,
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        filestore_requests=[
            # directory actions
            cfdp.FilestoreRequest(cfdp.ActionCode.REMOVE_DIRECTORY, "/test_directory")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    time.sleep(0.1)

    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert not os.path.isdir("./files/remote/test_directory")


"""
===============================================================================
Test Seq 9 - Check Deny File Directive
===============================================================================
Purpose: Check filestore requests

- Establish a two-way connection between client and server
- Filestore_request: Create Directory "/test_directory"
- Filestore_request: Deny Directory "/test_directory" if present

Expected Behavior:
- Put of filestore request: Create Directory "/test_directory"
- Creation of directory
- Put of filestore request: Deny Directory "/test_directory"
- Deletion of directory

Check: The test is passed, when the directory doesn't exists.

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
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        filestore_requests=[
            # directory actions
            cfdp.FilestoreRequest(cfdp.ActionCode.CREATE_DIRECTORY, "/test_directory")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    time.sleep(0.1)

    assert os.path.isdir("./files/remote/test_directory")

    transaction_id = cfdp_entity.put(
        destination_id=2,
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        filestore_requests=[
            # directory actions
            cfdp.FilestoreRequest(cfdp.ActionCode.DENY_DIRECTORY, "/test_directory")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    time.sleep(0.1)

    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert not os.path.isdir("./files/remote/test_directory")


"""
===============================================================================
Test Seq 10 - Check Directory Listing Request
===============================================================================
Purpose: Check Directory listing requests

- Establish a two-way connection between client and server
- Message to User: Directory listing "/.listing"

Expected Behavior:
- Put of message to user: Directory listing "/.listing"
- Creation of listing

Check: The test is passed, when the .listing exists.

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
        transmission_mode=cfdp.TransmissionMode.ACKNOWLEDGED,
        messages_to_user=[
            cfdp.DirectoryListingRequest(remote_directory="/", local_file="/.listing")
        ],
    )

    while not cfdp_entity.is_complete(
        transaction_id
    ) or not remote_entity.cfdp_entity.is_complete(transaction_id):
        time.sleep(0.1)
    time.sleep(0.1)

    cfdp_entity.shutdown()
    remote_entity.down()
    udp_transport.unbind()

    time.sleep(0.1)
    assert os.path.isfile("./files/remote/.listing")
    os.remove("./files/remote/.listing")
    os.remove("./files/local/.listing")


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
