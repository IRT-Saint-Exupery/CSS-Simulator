class Transport:
    def __init__(self):
        self.config = None

    def is_ready(self):
        return True

    def connect(self, *args, **kwargs):
        pass

    def disconnect(self, *args, **kwargs):
        pass

    def bind(self, *args, **kwargs):
        pass

    def unbind(self, *args, **kwargs):
        pass

    # send data
    def request(self, data):
        raise NotImplementedError

    # indication that data was received
    def indication(self, data):
        pass

    def shutdown(self):
        pass
