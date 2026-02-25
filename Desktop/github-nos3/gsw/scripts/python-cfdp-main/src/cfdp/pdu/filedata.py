from cfdp.constants import PduTypeCode
from .header import PduHeader


class FiledataPdu:
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
        # filedata pdu parameters
        record_continuation_state=None,
        segment_metadata_length=None,
        segment_metadata=None,
        segment_offset=None,
        file_data=None,
    ):
        if None in [segment_offset, file_data]:
            raise Exception("Must provide all required parameters")

        self.segment_offset = segment_offset
        self.file_data = file_data

        self.pdu_header = PduHeader(
            pdu_type=PduTypeCode.FILE_DATA,
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
        if self.pdu_header.segmentation_metadata_flag:
            raise NotImplementedError
        databytes = bytes(
            [
                (self.segment_offset >> 24) & 0xFF,
                (self.segment_offset >> 16) & 0xFF,
                (self.segment_offset >> 8) & 0xFF,
                (self.segment_offset) & 0xFF,
            ]
        )
        databytes += self.file_data
        self.pdu_header.pdu_data_field_length = len(databytes)
        return self.pdu_header.encode() + databytes

    @classmethod
    def decode(cls, pdu):
        pdu_header = PduHeader.decode(pdu)
        pdu_data = pdu[len(pdu_header) :]
        pdu_data_field_length = len(pdu_data)

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
            # filedata pdu parameters
            segment_offset=int.from_bytes(pdu_data[0:4], "big"),
            file_data=pdu_data[4:],
        )
