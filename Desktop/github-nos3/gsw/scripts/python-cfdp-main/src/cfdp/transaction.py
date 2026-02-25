from .constants import (
    ChecksumType,
    DEFAULT_MAX_FILE_SEGMENT_LEN,
    DEFAULT_FAULT_HANDLERS,
)


class Transaction:
    """
    A transaction is the end-to-end transmission of a single File Data Unit
    from a single source CFDP entity to a single destination CFDP entity.

    A transaction may transfer a file from the local filestore, and/or one or
    more user messages, and/or one or more filestore requests to the
    destination entity.

    """

    def __init__(
        self,
        kernel,
        source_entity_id,
        seq_number,
        destination_entity_id,
        transmission_mode,
        source_filename=None,
        destination_filename=None,
        fault_handler_overrides=None,
        messages_to_user=None,
        filestore_requests=None,
        checksum_type=ChecksumType.MODULAR,
        maximum_file_segment_length=DEFAULT_MAX_FILE_SEGMENT_LEN,
    ):
        self.kernel = kernel
        self.source_entity_id = source_entity_id
        self.seq_number = seq_number
        self.destination_entity_id = destination_entity_id
        self.transmission_mode = transmission_mode
        self.source_filename = source_filename
        self.destination_filename = destination_filename
        self.fault_handler_overrides = fault_handler_overrides
        self.messages_to_user = messages_to_user
        self.filestore_requests = filestore_requests
        self.checksum_type = checksum_type

        self.id = (source_entity_id, seq_number)
        self.file_handle = None

        self._file_checksum = None
        self._file_size = None
        self._file_pos = 0

        self.file_segment_size = maximum_file_segment_length

    def get_file_size(self):
        if self._file_size is None:
            try:
                self._file_size = (
                    self.kernel.filestore.get_size(self.source_filename)
                    if self.source_filename
                    else 0
                )
            except FileNotFoundError:
                self._file_size = 0
        return self._file_size

    def get_file_checksum(self):
        if self._file_checksum is None:
            if self.source_filename:
                fh = self.kernel.filestore.open(self.source_filename)
                self._file_checksum = fh.calculate_checksum(self.checksum_type)
                fh.close()
            else:
                self._file_checksum = 0
        return self._file_checksum

    def get_file_segment(self, offset=None, length=None):
        if self.file_handle is None:
            raise ValueError("File handle not defined")

        if offset is None:
            offset = self._file_pos
        if offset > self._file_size:
            raise ValueError("Exceeded file end")

        if length is None:
            length = self.file_segment_size
            self._file_pos += self.file_segment_size

        self.file_handle.seek(offset)
        data = self.file_handle.read(length)
        return offset, data

    def is_file_send_complete(self):
        return self._file_pos >= self._file_size

    def get_progress(self):
        # TODO...
        return None

    def get_fault_handler(self, fault):
        if self.fault_handler_overrides:
            if fault in self.fault_handler_overrides:
                return self.fault_handler_overrides[fault]
        return DEFAULT_FAULT_HANDLERS[fault]
