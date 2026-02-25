from cfdp.meta import get_required_octets


CFDP_PROTOCOL_VERSION = 1


class PduHeader:
    def __init__(
        self,
        pdu_type=None,
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
    ):
        if None in [
            pdu_type,
            direction,
            transmission_mode,
            source_entity_id,
            transaction_seq_number,
            destination_entity_id,
        ]:
            raise Exception("Must provide all required parameters")

        self.version = CFDP_PROTOCOL_VERSION
        self.pdu_type = pdu_type
        self.direction = direction
        self.transmission_mode = transmission_mode
        self.crc_flag = crc_flag
        self.large_file_flag = large_file_flag
        self.pdu_data_field_length = pdu_data_field_length
        self.segmentation_control = segmentation_control
        self.length_of_entity_ids = length_of_entity_ids
        self.segmentation_metadata_flag = segmentation_metadata_flag
        self.length_of_seq_number = length_of_seq_number
        self.source_entity_id = source_entity_id
        self.transaction_seq_number = transaction_seq_number
        self.destination_entity_id = destination_entity_id

    def encode(self):
        if self.pdu_data_field_length is None:
            raise ValueError("PDU data field length not defined")

        self.length_of_entity_ids = max(
            get_required_octets(self.source_entity_id) - 1,
            get_required_octets(self.destination_entity_id) - 1,
        )

        databytes = bytes(
            [
                (self.version << 5)
                + (self.pdu_type << 4)
                + (self.direction << 3)
                + (self.transmission_mode << 2)
                + (self.crc_flag << 1)
                + (self.large_file_flag),
                (self.pdu_data_field_length >> 8),
                (self.pdu_data_field_length & 0xFF),
                (self.segmentation_control << 7)
                + (self.length_of_entity_ids << 4)
                + (self.segmentation_metadata_flag << 3)
                + self.length_of_seq_number,
            ]
        )

        x = []
        for i in range(self.length_of_entity_ids + 1):
            x.append((self.source_entity_id >> (8 * i)) & 0xFF)
        x.reverse()
        databytes += bytes(x)

        x = []
        for i in range(self.length_of_seq_number + 1):
            x.append((self.transaction_seq_number >> (8 * i)) & 0xFF)
        x.reverse()
        databytes += bytes(x)

        x = []
        for i in range(self.length_of_entity_ids + 1):
            x.append((self.destination_entity_id >> (8 * i)) & 0xFF)
        x.reverse()
        databytes += bytes(x)

        return databytes

    def __len__(self):
        entity_id_field_length = 2 * (self.length_of_entity_ids + 1)
        transaction_seq_number_field_length = self.length_of_seq_number + 1
        return 4 + entity_id_field_length + transaction_seq_number_field_length

    @classmethod
    def decode(cls, pdu):
        pdu_data_field_length = int.from_bytes(pdu[1:3], "big")
        length_of_entity_ids = (pdu[3] >> 4) & 0x07
        length_of_seq_number = pdu[3] & 0x07

        fixed_pdu_end = (
            32 + (2 * (8 + 8 * length_of_entity_ids)) + (8 + 8 * length_of_seq_number)
        )
        source_entity_id = int.from_bytes(
            pdu[4 : (32 + (8 + 8 * length_of_entity_ids)) // 8], "big"
        )
        transaction_seq_number = int.from_bytes(
            pdu[
                (32 + (8 + 8 * length_of_entity_ids))
                // 8 : (
                    32 + (8 + 8 * length_of_entity_ids) + (8 + 8 * length_of_seq_number)
                )
                // 8
            ],
            "big",
        )
        destination_entity_id = int.from_bytes(
            pdu[
                (32 + (8 + 8 * length_of_entity_ids) + (8 + 8 * length_of_seq_number))
                // 8 : fixed_pdu_end
                // 8
            ],
            "big",
        )

        crc_flag = (pdu[0] >> 1) & 0x01
        if crc_flag:
            # TODO
            raise ValueError("PDUs with CRC are not yet supported")

        return cls(
            pdu_type=(pdu[0] >> 4) & 0x01,
            direction=(pdu[0] >> 3) & 0x01,
            transmission_mode=(pdu[0] >> 2) & 0x01,
            crc_flag=crc_flag,
            large_file_flag=pdu[0] & 0x01,
            pdu_data_field_length=pdu_data_field_length,
            segmentation_control=(pdu[3] >> 7) & 0x01,
            length_of_entity_ids=length_of_entity_ids,
            segmentation_metadata_flag=(pdu[3] >> 3) & 0x01,
            length_of_seq_number=length_of_seq_number,
            source_entity_id=source_entity_id,
            transaction_seq_number=transaction_seq_number,
            destination_entity_id=destination_entity_id,
        )
