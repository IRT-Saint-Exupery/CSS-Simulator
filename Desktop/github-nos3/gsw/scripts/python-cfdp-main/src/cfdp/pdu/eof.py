from cfdp.constants import DirectiveCode, PduTypeCode
from .header import PduHeader


class EofPdu:
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
        # eof pdu parameters
        condition_code=None,
        file_checksum=None,
        file_size=None,
        fault_location=None,
    ):
        self.directive_code = DirectiveCode.EOF
        self.condition_code = condition_code
        self.file_checksum = file_checksum
        self.file_size = file_size

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
                self.condition_code << 4,
                (self.file_checksum >> 24) & 0xFF,
                (self.file_checksum >> 16) & 0xFF,
                (self.file_checksum >> 8) & 0xFF,
                (self.file_checksum) & 0xFF,
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

        self.pdu_header.pdu_data_field_length = len(databytes)
        return self.pdu_header.encode() + databytes

    @classmethod
    def decode(cls, pdu):
        pdu_header = PduHeader.decode(pdu)
        pdu_data = pdu[len(pdu_header) :]
        pdu_data_field_length = len(pdu_data)

        file_size = (
            int.from_bytes(pdu_data[6:10], "big")
            if not pdu_header.large_file_flag
            else int.from_bytes(pdu_data[6:14], "big")
        )

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
            # eof pdu parameters
            condition_code=pdu_data[1] >> 4,
            file_checksum=int.from_bytes(pdu_data[2:6], "big"),
            file_size=file_size,
            fault_location=None,
        )  # not yet implemented
