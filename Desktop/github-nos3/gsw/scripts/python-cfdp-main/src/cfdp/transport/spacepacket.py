from spacepacket import SpacePacketProtocolEntity, SpacePacket, SequenceFlags

from .base import Transport


class SpacePacketTransport(Transport):
    """Space Packet Protocol Transport for CFDP PDUs.

    This transport protocol needs another transport protocol below it.
    """

    def __init__(self, apid, transport, packet_type):
        super().__init__()
        self.apid = apid
        self.transport = transport
        self.packet_type = packet_type
        self.spp_entity = SpacePacketProtocolEntity(apid=apid, transport=transport)
        self.spp_entity.indication = self._incoming_pdu_handler
        self._packet_sequence_count = 0

    def _get_next_packet_sequence_count(self):
        self._packet_sequence_count += 1
        return self._packet_sequence_count

    def request(self, pdu):
        space_packet = SpacePacket(
            packet_type=self.packet_type,
            packet_sec_hdr_flag=False,
            apid=self.apid,
            sequence_flags=SequenceFlags.UNSEGMENTED,
            packet_sequence_count=self._get_next_packet_sequence_count(),
            packet_data_field=pdu,
        )
        self.spp_entity.request(space_packet)

    def _incoming_pdu_handler(self, space_packet):
        data = space_packet.packet_data_field
        self.indication(data)
