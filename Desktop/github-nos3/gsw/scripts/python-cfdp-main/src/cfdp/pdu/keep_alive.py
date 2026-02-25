from cfdp.constants import DirectiveCode, PduTypeCode
from .header import PduHeader


class KeepAlivePdu:
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
        # keep alive pdu parameters
        progress=None,
    ):
        self.directive_code = DirectiveCode.KEEP_ALIVE
        self.progress = progress

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
        databytes = bytes([self.directive_code])

        if not self.pdu_header.large_file_flag:
            databytes += bytes(
                [
                    (self.progress >> 24) & 0xFF,
                    (self.progress >> 16) & 0xFF,
                    (self.progress >> 8) & 0xFF,
                    (self.progress) & 0xFF,
                ]
            )

        else:
            databytes += bytes(
                [
                    (self.progress >> 56) & 0xFF,
                    (self.progress >> 48) & 0xFF,
                    (self.progress >> 40) & 0xFF,
                    (self.progress >> 32) & 0xFF,
                    (self.progress >> 24) & 0xFF,
                    (self.progress >> 16) & 0xFF,
                    (self.progress >> 8) & 0xFF,
                    (self.progress) & 0xFF,
                ]
            )

        self.pdu_header.pdu_data_field_length = len(databytes)
        return self.pdu_header.encode() + databytes

    @classmethod
    def decode(cls, pdu):
        """ref: CCSDS 720.2-G-3 Page 2-12"""

        pdu_header = PduHeader.decode(pdu)
        pdu_data = pdu[len(pdu_header) :]
        pdu_data_field_length = len(pdu_data)

        progress = (
            int.from_bytes(pdu_data[1:5], "big")
            if not pdu_header.large_file_flag
            else int.from_bytes(pdu_data[1:9], "big")
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
            # keep alive pdu parameters
            progress=progress,
        )
