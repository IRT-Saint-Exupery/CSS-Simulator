import threading

from cfdp import logger
from cfdp.event import Event


class Timer:
    def __init__(self, machine, timeout, raise_event, expiration_limit=None):
        self.machine = machine
        self.timeout = timeout
        self.raise_event = raise_event
        self._timer = None
        self._expiration_limit = expiration_limit
        self._expiration_counter = 0
        self.suspended = False

    def restart(self):
        if self._timer:
            self._timer.cancel()
        if self.timeout:
            self._timer = threading.Timer(self.timeout, self._expired)
            self._timer.start()

    def cancel(self):
        if self._timer:
            self._timer.cancel()

    def _expired(self):
        logger.debug("Timer expired {}".format(type(self)))
        self._expiration_counter += 1
        self.machine.kernel.trigger_event(
            Event(self.machine.transaction, self.raise_event)
        )

    def is_limit_reached(self):
        if self._expiration_limit is None:
            return False
        else:
            return self._expiration_counter >= self._expiration_limit

    def shutdown(self):
        self.cancel()


class InactivityTimer(Timer):
    pass


class AckTimer(Timer):
    pass


class NakTimer(Timer):
    pass
