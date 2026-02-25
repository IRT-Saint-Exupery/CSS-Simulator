from cfdp.constants import DirectiveCode, PduTypeCode
from cfdp.meta import extract_metadata_options
from .header import PduHeader


class MetadataPdu:
    def __init__(
        self,
        # pdu header parameters
        direction=None,
        transmission_mode=None,
        crc_flag=0,
        large_file_flag=0,
        pdu_data_field_length=None,
        segmentation_control=0,
        length_of_entity_ids=0,
        segmentation_metadata_flag=0,
        length_of_seq_number=0,
        source_entity_id=None,
        transaction_seq_number=None,
        destination_entity_id=None,
        # metadata pdu parameters
        closure_requested=0,
        checksum_type=0,
        file_size=None,
        source_filename=None,
        destination_filename=None,
        filestore_requests=None,
        messages_to_user=None,
        fault_handler_overrides=None,
        flow_label=None,
    ):
        self.directive_code = DirectiveCode.METADATA
        self.closure_requested = closure_requested
        self.checksum_type = checksum_type
        self.file_size = file_size
        self.source_filename = source_filename
        self.destination_filename = destination_filename
        self.filestore_requests = filestore_requests
        self.messages_to_user = messages_to_user
        self.fault_handler_overrides = fault_handler_overrides
        self.flow_label = flow_label

        self.pdu_header = PduHeader(
            pdu_type=PduTypeCode.FILE_DIRECTIVE,
            direction=direction,
            transmission_mode=transmission_mode,
            crc_flag=crc_flag,
            large_file_flag=large_file_flag,
            pdu_data_field_length=pdu_data_field_length,
            segmentation_control=segmentation_control,
            length_of_entity_ids=length_of_entity_ids,
            segmentation_metadata_flag=segmentation_metadata_flag,
            length_of_seq_number=length_of_seq_number,
            source_entity_id=source_entity_id,
            transaction_seq_number=transaction_seq_number,
            destination_entity_id=destination_entity_id,
        )

    def encode(self):
        databytes = bytes(
            [
                self.directive_code,
                (0 << 7)
                + (self.closure_requested << 6)
                + (0 << 4)
                + self.checksum_type,
            ]
        )

        if not self.pdu_header.large_file_flag:
            databytes += bytes(
                [
                    (self.file_size >> 24) & 0xFF,
                    (self.file_size >> 16) & 0xFF,
                    (self.file_size >> 8) & 0xFF,
                    (self.file_size) & 0xFF,
                ]
            )

        else:
            databytes += bytes(
                [
                    (self.file_size >> 56) & 0xFF,
                    (self.file_size >> 48) & 0xFF,
                    (self.file_size >> 40) & 0xFF,
                    (self.file_size >> 32) & 0xFF,
                    (self.file_size >> 24) & 0xFF,
                    (self.file_size >> 16) & 0xFF,
                    (self.file_size >> 8) & 0xFF,
                    (self.file_size) & 0xFF,
                ]
            )

        if self.source_filename:
            value = self.source_filename.encode("utf-8")
            length = len(value)
            databytes += bytes([length])
            databytes += value
        else:
            databytes += bytes([0])
        if self.destination_filename:
            value = self.destination_filename.encode("utf-8")
            length = len(value)
            databytes += bytes([length])
            databytes += value
        else:
            databytes += bytes([0])

        if self.filestore_requests:
            for filestore_request in self.filestore_requests:
                databytes += filestore_request.encode()

        if self.messages_to_user:
            for message_to_user in self.messages_to_user:
                databytes += message_to_user.encode()

        if self.fault_handler_overrides:
            raise NotImplementedError

        if self.flow_label:
            raise NotImplementedError

        self.pdu_header.pdu_data_field_length = len(databytes)
        return self.pdu_header.encode() + databytes

    @classmethod
    def decode(cls, pdu):
        """ref: CCSDS 720.2-G-3 Page 2-6"""

        pdu_header = PduHeader.decode(pdu)
        pdu_data = pdu[len(pdu_header) :]
        pdu_data_field_length = len(pdu_data)

        offset = 9

        closure_requested = pdu_data[1] >> 6

        offset += 3
        checksum_type = pdu_data[offset // 8] & 0x0F

        offset += 4
        if pdu_header.large_file_flag:
            file_size = int.from_bytes(
                pdu_data[offset // 8 : (offset + 64) // 8], "big"
            )
            offset += 64
        else:
            file_size = int.from_bytes(
                pdu_data[offset // 8 : (offset + 32) // 8], "big"
            )
            offset += 32

        length_of_source_filename = int.from_bytes(
            pdu_data[offset // 8 : (offset + 8) // 8], "big"
        )

        offset += 8
        source_filenname = pdu_data[
            offset // 8 : (offset + 8 * length_of_source_filename) // 8
        ]

        offset += 8 * length_of_source_filename
        length_of_destination_filename = int.from_bytes(
            pdu_data[offset // 8 : (offset + 8) // 8], "big"
        )

        offset += 8
        destination_filename = pdu_data[
            offset // 8 : (offset + 8 * length_of_destination_filename) // 8
        ]

        offset += 8 * length_of_destination_filename

        (
            filestore_requests,
            messages_to_user,
            fault_handler_overrides,
            flow_label,
        ) = extract_metadata_options(pdu_data[offset // 8 :])

        return cls(
            # pdu header parameters
            direction=pdu_header.direction,
            transmission_mode=pdu_header.transmission_mode,
            crc_flag=pdu_header.crc_flag,
            large_file_flag=pdu_header.large_file_flag,
            pdu_data_field_length=pdu_data_field_length,
            segmentation_control=pdu_header.segmentation_control,
            length_of_entity_ids=pdu_header.length_of_entity_ids,
            segmentation_metadata_flag=pdu_header.segmentation_metadata_flag,
            length_of_seq_number=pdu_header.length_of_seq_number,
            source_entity_id=pdu_header.source_entity_id,
            transaction_seq_number=pdu_header.transaction_seq_number,
            destination_entity_id=pdu_header.destination_entity_id,
            # metadata pdu parameters
            closure_requested=closure_requested,
            checksum_type=checksum_type,
            file_size=file_size,
            source_filename=source_filenname.decode(),
            destination_filename=destination_filename.decode(),
            filestore_requests=filestore_requests,
            messages_to_user=messages_to_user,
            fault_handler_overrides=fault_handler_overrides,
            flow_label=flow_label,
        )
