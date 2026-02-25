from cfdp.constants import DirectiveCode, PduTypeCode
from .header import PduHeader


class FinishedPdu:
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
        # finished pdu parameters
        condition_code=None,
        delivery_code=None,
        file_status=None,
        filestore_responses=None,
        fault_location=None,
    ):
        self.directive_code = DirectiveCode.FINISHED
        self.condition_code = condition_code
        self.delivery_code = delivery_code
        self.file_status = file_status
        self.filestore_responses = filestore_responses

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
                (self.directive_code),
                (self.condition_code << 4)
                + (1 << 3)
                + (self.delivery_code << 2)  # end system status must be 1
                + (self.file_status),
            ]
        )

        if self.filestore_responses:
            for filestore_responses in self.filestore_responses:
                databytes += filestore_responses.encode()
            raise NotImplementedError

        self.pdu_header.pdu_data_field_length = len(databytes)
        return self.pdu_header.encode() + databytes

    @classmethod
    def decode(cls, pdu):
        """ref: CCSDS 720.2-G-3 Page 2-9"""

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
            # finished pdu parameters
            condition_code=pdu_data[1] >> 4,
            delivery_code=(pdu_data[1] >> 2) & 0x01,
            file_status=pdu_data[1] & 0x03,
            filestore_responses=0,  # TODO
            fault_location=None,
        )
