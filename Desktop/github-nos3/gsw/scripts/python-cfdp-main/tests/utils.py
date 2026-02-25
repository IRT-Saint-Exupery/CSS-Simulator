import spacepacket

import cfdp
from cfdp.filestore import NativeFileStore


class RemoteEntityUdp:
    """
    Remote entity that the local one connects to.

    """

    def up(self):
        from cfdp.transport.udp import UdpTransport

        self.udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5111)]})
        self.udp_transport.bind("127.0.0.1", 5222)

        self.cfdp_entity = cfdp.CfdpEntity(
            entity_id=2,
            filestore=NativeFileStore("./files/remote"),
            transport=self.udp_transport,
        )

    def down(self):
        self.cfdp_entity.shutdown()
        self.udp_transport.unbind()


class RemoteEntityZmq:
    """
    Remote entity that the local one connects to.

    """

    def up(self):
        from cfdp.transport.zmq import ZmqTransport

        self.zmq_transport = ZmqTransport()
        self.zmq_transport.bind("127.0.0.1", 5222)
        self.cfdp_entity = cfdp.CfdpEntity(
            entity_id=2,
            filestore=NativeFileStore("./files/remote"),
            transport=self.zmq_transport,
        )

    def down(self):
        self.cfdp_entity.shutdown()
        self.zmq_transport.unbind()


class RemoteEntitySpacePacket:
    """
    Remote entity that the local one connects to.

    """

    def up(self):
        from cfdp.transport.spacepacket import SpacePacketTransport
        from cfdp.transport.udp import UdpTransport

        self.udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5111)]})
        self.udp_transport.bind("127.0.0.1", 5222)

        self.spacepacket_transport = SpacePacketTransport(
            apid=111,
            transport=self.udp_transport,
            packet_type=spacepacket.PacketType.TELEMETRY,
        )

        self.cfdp_entity = cfdp.CfdpEntity(
            entity_id=1,
            filestore=NativeFileStore("./files/remote"),
            transport=self.spacepacket_transport,
        )

    def down(self):
        self.cfdp_entity.shutdown()
        self.udp_transport.unbind()
