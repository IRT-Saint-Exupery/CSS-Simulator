from cfdp import logger
from cfdp.constants import MachineState, TransmissionMode, DeliveryCode, ConditionCode
from cfdp.event import EventType
from cfdp.meta import execute_filestore_requests
from .timer import InactivityTimer
from .base import Machine


class Receiver1(Machine):

    """Implementation of Class 1 (unacknowledged transfer) receiver."""

    def __init__(self, kernel, transaction):
        super().__init__(kernel, transaction)
        self.transmission_mode = TransmissionMode.UNACKNOWLEDGED
        self.state = MachineState.WAIT_FOR_MD

        # inactivity timer
        timeout = kernel.transaction_inactivity_limit
        self.inactivity_timer = InactivityTimer(
            self, timeout, EventType.E27_INACTIVITY_TIMEOUT
        )

    def update_state(self, event, pdu=None):
        """See state table given in CCSDS 720.2-G-3, Table 5-2"""
        logger.debug(
            "[{}] Event: {}, State: {}".format(
                self.transaction.id, event.type, self.state
            )
        )

        if pdu:
            self.restart_inactivity_timer()  # as per CCSDS 720.2-G-3, 5.3.7

        if self.state == MachineState.WAIT_FOR_MD:
            if event.type == EventType.E0_ENTERED_STATE:
                self.initialize()
                self.restart_inactivity_timer()

            elif event.type == EventType.E2_ABANDON_TRANSACTION:
                self.issue_abandoned_indication()
                self.shutdown()

            elif event.type == EventType.E3_NOTICE_OF_CANCELLATION:
                self.issue_transaction_finished_indication()
                self.shutdown()

            elif event.type == EventType.E10_RECEIVED_METADATA:
                self.issue_metadata_received_indication(pdu.file_size)
                if self.is_file_transfer():
                    self.open_tempfile()
                self.process_metadata_options(pdu)
                self.state = MachineState.WAIT_FOR_EOF

            elif event.type == EventType.E12_RECEIVED_EOF_NO_ERROR:
                logger.info(
                    "[{}] Finished without Metadata received".format(
                        self.transaction.id
                    )
                )
                self.issue_transaction_finished_indication()
                self.shutdown()

            elif event.type == EventType.E13_RECEIVED_EOF_CANCEL:
                self.condition_code = pdu.condition_code
                self.issue_transaction_finished_indication()
                self.shutdown()

            elif event.type == EventType.E27_INACTIVITY_TIMEOUT:
                self.restart_inactivity_timer()
                self.fault_inactivity()

            elif event.type == EventType.E33_RECEIVED_CANCEL_REQUEST:
                self.condition_code = ConditionCode.CANCEL_REQUEST_RECEIVED
                self.trigger_event(EventType.E3_NOTICE_OF_CANCELLATION)

            elif event.type == EventType.E34_RECEIVED_REPORT_REQUEST:
                self.issue_report_indication()

            else:
                logger.debug(
                    "[{}] Event: {} not applicable for this state: {}".format(
                        self.transaction.id, event.type, self.state
                    )
                )

        elif self.state == MachineState.WAIT_FOR_EOF:
            if event.type == EventType.E2_ABANDON_TRANSACTION:
                self.issue_abandoned_indication()
                self.shutdown()

            elif event.type == EventType.E3_NOTICE_OF_CANCELLATION:
                self.issue_transaction_finished_indication()
                self.shutdown()

            elif event.type == EventType.E11_RECEIVED_FILEDATA:
                self.issue_filesegment_received_indication(
                    pdu.segment_offset, len(pdu.file_data)
                )
                if self.is_file_transfer():
                    self.store_file_data(pdu)
                    self.update_received_file_size(pdu)

            elif event.type == EventType.E12_RECEIVED_EOF_NO_ERROR:
                self.issue_eof_received_indication()

                if self.is_file_transfer():
                    if self.is_file_size_error(pdu.file_size):
                        self.fault_file_size()
                    if self.is_file_checksum_failure(pdu.file_checksum):
                        self.fault_file_checksum()

                    self.delivery_code = DeliveryCode.DATA_COMPLETE
                    self.copy_tempfile_to_destfile()
                    self.close_tempfile()

                if self.transaction.filestore_requests:
                    execute_filestore_requests(
                        self.kernel, self.transaction.filestore_requests
                    )

                self.issue_transaction_finished_indication()
                self.shutdown()

            elif event.type == EventType.E13_RECEIVED_EOF_CANCEL:
                self.condition_code = pdu.condition_code
                self.issue_transaction_finished_indication()
                self.shutdown()

            elif event.type == EventType.E27_INACTIVITY_TIMEOUT:
                self.restart_inactivity_timer()
                self.fault_inactivity()

            elif event.type == EventType.E33_RECEIVED_CANCEL_REQUEST:
                self.condition_code = ConditionCode.CANCEL_REQUEST_RECEIVED
                self.trigger_event(EventType.E3_NOTICE_OF_CANCELLATION)

            elif event.type == EventType.E34_RECEIVED_REPORT_REQUEST:
                self.issue_report_indication()

            else:
                pass  # ignore an event not applicable for this state

    def shutdown(self):
        super().shutdown()
        self.inactivity_timer.shutdown()
