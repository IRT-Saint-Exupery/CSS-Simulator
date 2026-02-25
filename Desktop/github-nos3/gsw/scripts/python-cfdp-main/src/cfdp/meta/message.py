import math

from cfdp.constants import TypeFieldCode, MessageType
from .datafield import TypeLengthValue
from .filestore import extract_filestore_request


def get_required_octets(number):
    return max(math.ceil(math.log(number + 1, 2) / 8), 1)


class MessageToUser(TypeLengthValue):
    def __init__(self):
        self.type = TypeFieldCode.MESSAGE_TO_USER


class ReservedMessageToUser(MessageToUser):
    def __init__(self):
        super().__init__()
        self.message_identifier = "cfdp"


class OriginatingTransactionId(ReservedMessageToUser):
    def __init__(self, source_entity_id, transaction_seq_number):
        super().__init__()
        self.message_type = MessageType.ORIGINATING_TRANSACTION_ID

        self.source_entity_id = source_entity_id
        self.transaction_seq_number = transaction_seq_number

    def encode(self):
        data = self.message_identifier.encode("utf-8")
        data += bytes([self.message_type])

        length_of_entity_id = get_required_octets(self.source_entity_id) - 1
        length_of_seq_number = get_required_octets(self.transaction_seq_number) - 1
        data += bytes(
            [(0 << 7) + (length_of_entity_id << 4) + (0 << 3) + length_of_seq_number]
        )

        x = []

        for i in range(length_of_entity_id + 1):
            x.append((self.source_entity_id >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)

        x = []

        for i in range(length_of_seq_number + 1):
            x.append((self.transaction_seq_number >> (8 * i)) & 0xFF)

        x.reverse()
        data += bytes(x)

        self.value = data
        return super().encode()


class ProxyPutRequest(ReservedMessageToUser):
    def __init__(
        self, destination_entity_id, source_filename=None, destination_filename=None
    ):
        super().__init__()
        self.message_type = MessageType.PROXY_PUT_REQUEST

        self.destination_entity_id = destination_entity_id
        self.source_filename = source_filename
        self.destination_filename = destination_filename

    def encode(self):
        data = self.message_identifier.encode("utf-8")
        data += bytes([self.message_type])

        length_of_entity_id = get_required_octets(self.destination_entity_id)
        data += bytes([length_of_entity_id])
        x = []
        for i in range(length_of_entity_id):
            x.append((self.destination_entity_id >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)

        value = self.source_filename.encode("utf-8")
        length = len(value)
        data += bytes([length])
        data += value

        value = self.destination_filename.encode("utf-8")
        length = len(value)
        data += bytes([length])
        data += value

        self.value = data
        return super().encode()


class ProxyPutCancel(ReservedMessageToUser):
    def __init__(self, entity_id, seq_number):
        super().__init__()
        self.message_type = MessageType.PROXY_PUT_CANCEL

        self.entity_id = entity_id
        self.seq_number = seq_number

    def encode(self):
        data = self.message_identifier.encode("utf-8")
        data += bytes([self.message_type])
        self.value = data

        length_of_entity_id = get_required_octets(self.entity_id)

        data += bytes([length_of_entity_id])

        x = []

        for i in range(length_of_entity_id):
            x.append((self.entity_id >> (8 * i)) & 0xFF)

        x.reverse()
        data += bytes(x)

        length_of_seq_number = get_required_octets(self.seq_number)
        data += bytes([length_of_seq_number])

        x = []

        for i in range(length_of_seq_number):
            x.append((self.seq_number >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)

        self.value = data
        return super().encode()


class DirectoryListingRequest(ReservedMessageToUser):
    def __init__(self, remote_directory, local_file):
        super().__init__()
        self.message_type = MessageType.DIRECTORY_LISTING_REQUEST

        self.directory_name = remote_directory
        self.directory_file_name = local_file

    def encode(self):
        data = self.message_identifier.encode("utf-8")
        data += bytes([self.message_type])

        value = self.directory_name.encode("utf-8")
        length = len(value)
        data += bytes([length])
        data += value

        value = self.directory_file_name.encode("utf-8")
        length = len(value)
        data += bytes([length])
        data += value

        self.value = data
        return super().encode()


class DirectoryListingResponse(ReservedMessageToUser):
    def __init__(self, remote_directory, local_file, listing_response_code=0x00):
        super().__init__()
        self.message_type = MessageType.DIRECTORY_LISTING_RESPONSE

        self.listing_response_code = listing_response_code
        self.directory_name = remote_directory
        self.directory_file_name = local_file

    def encode(self):
        data = self.message_identifier.encode("utf-8")
        data += bytes([self.message_type])

        data += bytes([self.listing_response_code])

        value = self.directory_name.encode("utf-8")
        length = len(value)
        data += bytes([length])
        data += value

        value = self.directory_file_name.encode("utf-8")
        length = len(value)
        data += bytes([length])
        data += value

        self.value = data
        return super().encode()


class RemoteSuspendRequest(ReservedMessageToUser):
    def __init__(self, source_entity_id, transaction_seq_number):
        super().__init__()
        self.message_type = MessageType.REMOTE_SUSPEND_REQUEST

        self.source_entity_id = source_entity_id
        self.transaction_seq_number = transaction_seq_number

    def encode(self):
        data = self.message_identifier.encode("utf-8")
        data += bytes([self.message_type])

        length_of_entity_id = get_required_octets(self.source_entity_id) - 1
        length_of_seq_number = get_required_octets(self.transaction_seq_number) - 1

        data += bytes(
            [(0 << 7) + (length_of_entity_id << 4) + (0 << 3) + length_of_seq_number]
        )

        x = []

        for i in range(length_of_entity_id + 1):
            x.append((self.source_entity_id >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)

        x = []

        for i in range(length_of_seq_number + 1):
            x.append((self.transaction_seq_number >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)

        self.value = data
        return super().encode()


class RemoteSuspendResponse(ReservedMessageToUser):
    def __init__(
        self,
        source_entity_id,
        transaction_seq_number,
        suspension_indicator=1,
        transaction_status=0,
    ):
        super().__init__()
        self.message_type = MessageType.REMOTE_SUSPEND_RESPONSE

        self.suspension_indicator = suspension_indicator
        self.transaction_status = transaction_status
        self.source_entity_id = source_entity_id
        self.transaction_seq_number = transaction_seq_number

    def encode(self):
        data = self.message_identifier.encode("utf-8")
        data += bytes([self.message_type])

        data += bytes(
            [(self.suspension_indicator << 7) + (self.transaction_status << 5)]
        )

        length_of_entity_id = get_required_octets(self.source_entity_id) - 1
        length_of_seq_number = get_required_octets(self.transaction_seq_number) - 1

        data += bytes(
            [(0 << 7) + (length_of_entity_id << 4) + (0 << 3) + length_of_seq_number]
        )

        x = []

        for i in range(length_of_entity_id + 1):
            x.append((self.source_entity_id >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)

        x = []

        for i in range(length_of_seq_number + 1):
            x.append((self.transaction_seq_number >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)

        self.value = data
        return super().encode()


class RemoteResumeRequest(ReservedMessageToUser):
    def __init__(self, source_entity_id, transaction_seq_number):
        super().__init__()
        self.message_type = MessageType.REMOTE_RESUME_REQUEST

        self.source_entity_id = source_entity_id
        self.transaction_seq_number = transaction_seq_number

    def encode(self):
        data = self.message_identifier.encode("utf-8")
        data += bytes([self.message_type])

        length_of_entity_id = get_required_octets(self.source_entity_id) - 1

        length_of_seq_number = get_required_octets(self.transaction_seq_number) - 1
        data += bytes(
            [(0 << 7) + (length_of_entity_id << 4) + (0 << 3) + length_of_seq_number]
        )

        x = []

        for i in range(length_of_entity_id + 1):
            x.append((self.source_entity_id >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)
        x = []
        for i in range(length_of_seq_number + 1):
            x.append((self.transaction_seq_number >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)

        self.value = data
        return super().encode()


class RemoteResumeResponse(ReservedMessageToUser):
    def __init__(
        self,
        source_entity_id,
        transaction_seq_number,
        suspension_indicator=1,
        transaction_status=0,
    ):
        super().__init__()
        self.message_type = MessageType.REMOTE_RESUME_RESPONSE

        self.suspension_indicator = suspension_indicator
        self.transaction_status = transaction_status
        self.source_entity_id = source_entity_id
        self.transaction_seq_number = transaction_seq_number

    def encode(self):
        data = self.message_identifier.encode("utf-8")
        data += bytes([self.message_type])

        data += bytes(
            [(self.suspension_indicator << 7) + (self.transaction_status << 5)]
        )

        length_of_entity_id = get_required_octets(self.source_entity_id) - 1
        length_of_seq_number = get_required_octets(self.transaction_seq_number) - 1

        data += bytes(
            [(0 << 7) + (length_of_entity_id << 4) + (0 << 3) + length_of_seq_number]
        )

        x = []

        for i in range(length_of_entity_id + 1):
            x.append((self.source_entity_id >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)

        x = []

        for i in range(length_of_seq_number + 1):
            x.append((self.transaction_seq_number >> (8 * i)) & 0xFF)
        x.reverse()
        data += bytes(x)

        self.value = data
        return super().encode()


def extract_metadata_options(databytes):
    filestore_requests = []
    messages_to_user = []
    fault_handler_overrides = []
    flow_label = []

    while databytes:
        type = databytes[0]
        length = databytes[1]
        data = databytes[2 : 2 + length]
        databytes = databytes[2 + length :]

        if type == TypeFieldCode.FILESTORE_REQUEST:
            filestore_request = extract_filestore_request(data)
            filestore_requests.append(filestore_request)

        if type == TypeFieldCode.MESSAGE_TO_USER:
            message_to_user = extract_message_to_user(data)
            messages_to_user.append(message_to_user)

    return filestore_requests, messages_to_user, fault_handler_overrides, flow_label


def extract_message_to_user(data):
    if not data.startswith(b"cfdp"):
        # non-standard user messages
        raise NotImplementedError

    message_type = data[4]
    data = data[5:]

    if message_type == MessageType.ORIGINATING_TRANSACTION_ID:
        length_of_entity_id = (data[0] >> 4) + 1
        length_of_seq_number = (data[0] & 0x0F) + 1
        source_entity_id = int.from_bytes(
            data[2 : 2 + length_of_entity_id], byteorder="big", signed=False
        )
        transaction_seq_number = int.from_bytes(
            data[2 + length_of_entity_id :], byteorder="big", signed=False
        )
        return OriginatingTransactionId(source_entity_id, transaction_seq_number)

    elif message_type == MessageType.PROXY_PUT_REQUEST:
        len_destination_entity_id = data[0]
        offset = 1
        destination_entity_id = int.from_bytes(
            data[offset : offset + len_destination_entity_id],
            byteorder="big",
            signed=False,
        )
        offset += len_destination_entity_id
        len_source_filename = data[offset]
        offset += 1
        source_filename = data[offset : offset + len_source_filename]
        offset += len_source_filename
        len_destination_filename = data[offset]
        offset += 1
        destination_filename = data[offset : offset + len_destination_filename]
        return ProxyPutRequest(
            destination_entity_id, source_filename, destination_filename
        )

    elif message_type == MessageType.PROXY_PUT_CANCEL:
        len_entity_id = data[0]
        offset = 1
        entity_id = int.from_bytes(
            data[offset : offset + len_entity_id], byteorder="big", signed=False
        )
        offset += len_entity_id
        len_seq_number = data[offset]
        offset += 1
        seq_number = int.from_bytes(
            data[offset : offset + len_seq_number], byteorder="big", signed=False
        )
        return ProxyPutCancel(entity_id, seq_number)

    elif message_type == MessageType.DIRECTORY_LISTING_REQUEST:
        length_of_first_filename = data[0]
        directory_name = data[1 : 1 + length_of_first_filename]
        directory_file_name = data[2 + length_of_first_filename :]
        return DirectoryListingRequest(directory_name, directory_file_name)

    elif message_type == MessageType.DIRECTORY_LISTING_RESPONSE:
        listing_response_code = data[0]
        length_of_directory_name = data[1]
        directory_name = data[2 : 2 + length_of_directory_name]
        directory_file_name = data[3 + length_of_directory_name :]
        return DirectoryListingResponse(
            directory_name, directory_file_name, listing_response_code
        )

    elif message_type == MessageType.REMOTE_SUSPEND_REQUEST:
        length_of_entity_id = (data[0] >> 4) + 1
        length_of_seq_number = (data[0] & 0x0F) + 1
        source_entity_id = int.from_bytes(
            data[1 : 1 + length_of_entity_id], byteorder="big", signed=False
        )
        transaction_seq_number = int.from_bytes(
            data[1 + length_of_entity_id :], byteorder="big", signed=False
        )
        return RemoteSuspendRequest(source_entity_id, transaction_seq_number)

    elif message_type == MessageType.REMOTE_SUSPEND_RESPONSE:
        suspension_indicator = data[0] >> 7
        transaction_status = (data[0] >> 7) & 0x03
        length_of_entity_id = (data[1] >> 4) + 1
        length_of_seq_number = (data[1] & 0x0F) + 1
        source_entity_id = int.from_bytes(
            data[2 : 2 + length_of_entity_id], byteorder="big", signed=False
        )
        transaction_seq_number = int.from_bytes(
            data[2 + length_of_seq_number :], byteorder="big", signed=False
        )
        return RemoteSuspendResponse(
            source_entity_id,
            transaction_seq_number,
            suspension_indicator,
            transaction_status,
        )

    elif message_type == MessageType.REMOTE_RESUME_REQUEST:
        length_of_entity_id = (data[0] >> 4) + 1
        length_of_seq_number = (data[0] & 0x0F) + 1
        source_entity_id = int.from_bytes(
            data[1 : 1 + length_of_entity_id], byteorder="big", signed=False
        )
        transaction_seq_number = int.from_bytes(
            data[1 + length_of_entity_id :], byteorder="big", signed=False
        )
        return RemoteResumeRequest(source_entity_id, transaction_seq_number)

    elif message_type == MessageType.REMOTE_RESUME_RESPONSE:
        suspension_indicator = data[0] >> 7
        transaction_status = (data[0] >> 7) & 0x03
        length_of_entity_id = (data[1] >> 4) + 1
        length_of_seq_number = (data[1] & 0x0F) + 1
        source_entity_id = int.from_bytes(
            data[2 : 2 + length_of_entity_id], byteorder="big", signed=False
        )
        transaction_seq_number = int.from_bytes(
            data[2 + length_of_seq_number :], byteorder="big", signed=False
        )
        return RemoteResumeResponse(
            source_entity_id,
            transaction_seq_number,
            suspension_indicator,
            transaction_status,
        )

    else:
        raise NotImplementedError


def process_user_message(kernel, message):
    if isinstance(message, OriginatingTransactionId):
        pass

    elif isinstance(message, ProxyPutRequest):
        destination_entity_id = message.destination_entity_id
        source_filename = message.source_filename.decode()
        destination_filename = message.destination_filename.decode()

        transmission_mode = message.originating_transaction.transmission_mode
        source_entity_id = message.originating_transaction.source_entity_id
        transaction_seq_number = message.originating_transaction.seq_number
        kernel.put(
            destination_id=destination_entity_id,
            source_filename=source_filename,
            destination_filename=destination_filename,
            transmission_mode=transmission_mode,
            messages_to_user=[
                OriginatingTransactionId(
                    source_entity_id=source_entity_id,
                    transaction_seq_number=transaction_seq_number,
                )
            ],
        )

    elif isinstance(message, ProxyPutCancel):
        transaction_id = (message.entity_id, message.seq_number)
        kernel.cancel(transaction_id)

    elif isinstance(message, DirectoryListingRequest):
        dirname = message.directory_name.decode()
        listing = kernel.filestore.list_directory(dirname)

        filename = kernel.filestore.join_path(dirname, ".listing")
        fh = kernel.filestore.open(filename, "wt")
        fh.write(listing)
        fh.close()

        # send file and directory response
        destination_id = message.originating_transaction.source_entity_id
        transmission_mode = message.originating_transaction.transmission_mode

        destination_filename = message.directory_file_name.decode()
        directory_name = message.directory_name.decode()
        source_entity_id = message.originating_transaction.source_entity_id
        transaction_seq_number = message.originating_transaction.seq_number
        kernel.put(
            destination_id=destination_id,
            source_filename=filename,
            destination_filename=destination_filename,
            transmission_mode=transmission_mode,
            messages_to_user=[
                DirectoryListingResponse(directory_name, destination_filename),
                OriginatingTransactionId(
                    source_entity_id=source_entity_id,
                    transaction_seq_number=transaction_seq_number,
                ),
            ],
        )

    elif isinstance(message, DirectoryListingResponse):
        # TODO: let user add a callback to be triggered with directory listing
        pass

    elif isinstance(message, RemoteSuspendRequest):
        transaction_id = (message.source_entity_id, message.transaction_seq_number)
        kernel.suspend(transaction_id)
        transmission_mode = message.originating_transaction.transmission_mode
        source_entity_id = message.originating_transaction.source_entity_id
        transaction_seq_number = message.originating_transaction.seq_number
        kernel.put(
            destination_id=source_entity_id,
            transmission_mode=transmission_mode,
            messages_to_user=[
                RemoteSuspendResponse(
                    source_entity_id=message.source_entity_id,
                    transaction_seq_number=message.transaction_seq_number,
                ),
                OriginatingTransactionId(
                    source_entity_id=source_entity_id,
                    transaction_seq_number=transaction_seq_number,
                ),
            ],
        )

    elif isinstance(message, RemoteSuspendResponse):
        kernel.suspended_indication(
            (message.source_entity_id, message.transaction_seq_number),
            message.transaction_status,
        )

    elif isinstance(message, RemoteResumeRequest):
        transaction_id = (message.source_entity_id, message.transaction_seq_number)
        kernel.resume(transaction_id)
        transmission_mode = message.originating_transaction.transmission_mode
        source_entity_id = message.originating_transaction.source_entity_id
        transaction_seq_number = message.originating_transaction.seq_number
        kernel.put(
            destination_id=source_entity_id,
            transmission_mode=transmission_mode,
            messages_to_user=[
                RemoteResumeResponse(
                    source_entity_id=message.source_entity_id,
                    transaction_seq_number=message.transaction_seq_number,
                ),
                OriginatingTransactionId(
                    source_entity_id=source_entity_id,
                    transaction_seq_number=transaction_seq_number,
                ),
            ],
        )

    elif isinstance(message, RemoteResumeResponse):
        kernel.resumed_indication(
            (message.source_entity_id, message.transaction_seq_number),
            message.transaction_status,
        )

    else:
        raise NotImplementedError
