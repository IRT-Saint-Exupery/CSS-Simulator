from cfdp.constants import DirectiveCode, PduTypeCode
from .header import PduHeader


class NakPdu:
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
        # nak pdu parameters
        start_of_scope=None,
        end_of_scope=None,
        segment_requests=None,
    ):
        self.directive_code = DirectiveCode.NAK
        self.start_of_scope = start_of_scope
        self.end_of_scope = end_of_scope
        self.segment_requests = segment_requests

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
                    (self.start_of_scope >> 24) & 0xFF,
                    (self.start_of_scope >> 16) & 0xFF,
                    (self.start_of_scope >> 8) & 0xFF,
                    (self.start_of_scope) & 0xFF,
                ]
            )

        else:
            databytes += bytes(
                [
                    (self.start_of_scope >> 56) & 0xFF,
                    (self.start_of_scope >> 48) & 0xFF,
                    (self.start_of_scope >> 40) & 0xFF,
                    (self.start_of_scope >> 32) & 0xFF,
                    (self.start_of_scope >> 24) & 0xFF,
                    (self.start_of_scope >> 16) & 0xFF,
                    (self.start_of_scope >> 8) & 0xFF,
                    (self.start_of_scope) & 0xFF,
                ]
            )

        if not self.pdu_header.large_file_flag:
            databytes += bytes(
                [
                    (self.end_of_scope >> 24) & 0xFF,
                    (self.end_of_scope >> 16) & 0xFF,
                    (self.end_of_scope >> 8) & 0xFF,
                    (self.end_of_scope) & 0xFF,
                ]
            )

        else:
            databytes += bytes(
                [
                    (self.end_of_scope >> 56) & 0xFF,
                    (self.end_of_scope >> 48) & 0xFF,
                    (self.end_of_scope >> 40) & 0xFF,
                    (self.end_of_scope >> 32) & 0xFF,
                    (self.end_of_scope >> 24) & 0xFF,
                    (self.end_of_scope >> 16) & 0xFF,
                    (self.end_of_scope >> 8) & 0xFF,
                    (self.end_of_scope) & 0xFF,
                ]
            )

        if self.segment_requests:
            for start_offset, end_offset in self.segment_requests:
                if not self.pdu_header.large_file_flag:
                    databytes += bytes(
                        [
                            (start_offset >> 24) & 0xFF,
                            (start_offset >> 16) & 0xFF,
                            (start_offset >> 8) & 0xFF,
                            (start_offset) & 0xFF,
                        ]
                    )

                else:
                    databytes += bytes(
                        [
                            (start_offset >> 56) & 0xFF,
                            (start_offset >> 48) & 0xFF,
                            (start_offset >> 40) & 0xFF,
                            (start_offset >> 32) & 0xFF,
                            (start_offset >> 24) & 0xFF,
                            (start_offset >> 16) & 0xFF,
                            (start_offset >> 8) & 0xFF,
                            (start_offset) & 0xFF,
                        ]
                    )

                if not self.pdu_header.large_file_flag:
                    databytes += bytes(
                        [
                            (end_offset >> 24) & 0xFF,
                            (end_offset >> 16) & 0xFF,
                            (end_offset >> 8) & 0xFF,
                            (end_offset) & 0xFF,
                        ]
                    )

                else:
                    databytes += bytes(
                        [
                            (end_offset >> 56) & 0xFF,
                            (end_offset >> 48) & 0xFF,
                            (end_offset >> 40) & 0xFF,
                            (end_offset >> 32) & 0xFF,
                            (end_offset >> 24) & 0xFF,
                            (end_offset >> 16) & 0xFF,
                            (end_offset >> 8) & 0xFF,
                            (end_offset) & 0xFF,
                        ]
                    )

        self.pdu_header.pdu_data_field_length = len(databytes)
        return self.pdu_header.encode() + databytes

    @classmethod
    def decode(cls, pdu):
        """ref: CCSDS 720.2-G-3 Page 2-7"""

        pdu_header = PduHeader.decode(pdu)
        pdu_data = pdu[len(pdu_header) :]
        pdu_data_field_length = len(pdu_data)

        offset = 8
        if pdu_header.large_file_flag:
            start_of_scope = int.from_bytes(
                pdu_data[offset // 8 : (offset + 64) // 8], "big"
            )
            offset += 64
            end_of_scope = int.from_bytes(
                pdu_data[offset // 8 : (offset + 64) // 8], "big"
            )
            offset += 64
        else:
            start_of_scope = int.from_bytes(
                pdu_data[offset // 8 : (offset + 32) // 8], "big"
            )
            offset += 32
            end_of_scope = int.from_bytes(
                pdu_data[offset // 8 : (offset + 32) // 8], "big"
            )
            offset += 32

        segment_requests = []
        while offset < len(pdu_data) * 8:
            if pdu_header.large_file_flag:
                start_offset = int.from_bytes(
                    pdu_data[offset // 8 : (offset + 64) // 8], "big"
                )
                offset += 64
                end_offset = int.from_bytes(
                    pdu_data[offset // 8 : (offset + 64) // 8], "big"
                )
                offset += 64
            else:
                start_offset = int.from_bytes(
                    pdu_data[offset // 8 : (offset + 32) // 8], "big"
                )
                offset += 32
                end_offset = int.from_bytes(
                    pdu_data[offset // 8 : (offset + 32) // 8], "big"
                )
                offset += 32
            segment_requests.append((start_offset, end_offset))

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
            # nak pdu parameters
            start_of_scope=start_of_scope,
            end_of_scope=end_of_scope,
            segment_requests=segment_requests,
        )
