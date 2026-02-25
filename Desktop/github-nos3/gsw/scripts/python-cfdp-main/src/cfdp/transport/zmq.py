import threading
import zmq

from .base import Transport


class ZmqTransport(Transport):
    """ZMQ Transport for CFDP PDUs.

    This transport is mostly used for testing and for prototypes.
    A connection is established by making one entity the host, which does `bind` to a port,
    and the other entity the client, which does `connect` to the host.
    """

    def __init__(self):
        super().__init__()
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.PAIR)

        self._thread = threading.Thread(target=self._incoming_pdu_handler)
        self._thread.kill = False

    def request(self, pdu):
        self.socket.send(pdu)

    def connect(self, host, port):
        self.socket.connect(f"tcp://{host}:{port}")
        self._thread.kill = False
        self._thread.start()

    def disconnect(self):
        self._thread.kill = True
        self._thread.join()
        self.socket.close()

    def bind(self, host, port):
        self.socket.bind(f"tcp://{host}:{port}")
        self._thread.kill = False
        self._thread.start()

    def unbind(self):
        self._thread.kill = True
        self._thread.join()
        self.context.destroy()

    def _incoming_pdu_handler(self):
        thread = threading.current_thread()
        while not thread.kill:
            if self.socket.poll(100):
                if thread.kill:
                    break
                pdu = self.socket.recv()
                self.indication(pdu)
