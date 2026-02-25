import queue
import threading

from . import logger
from .constants import (
    TransmissionMode,
    PduTypeCode,
    DirectiveCode,
    Direction,
    MachineState,
    ConditionCode,
    DirectiveSubTypeCode,
    TransactionStatus,
    ChecksumType,
    DEFAULT_FAULT_HANDLERS,
    DEFAULT_INACTIVITY_TIMEOUT,
    DEFAULT_ACK_TIMER_LIMIT,
    DEFAULT_ACK_TIMER_EXPIRATION_LIMIT,
    DEFAULT_NAK_TIMER_INTERVAL,
    DEFAULT_NAK_TIMER_EXPIRATION_LIMIT,
)
from .event import Event, EventType
from .transaction import Transaction
from .machines import Sender1, Receiver1, Sender2, Receiver2
from .pdu import (
    PduHeader,
    MetadataPdu,
    FiledataPdu,
    EofPdu,
    FinishedPdu,
    NakPdu,
    AckPdu,
    KeepAlivePdu,
    PromptPdu,
)
from .meta import process_user_message as meta_process_user_message


class CfdpEntity:

    """
    The CfdpUser keeps a dict of active state machines in the form:
        self.machines = {(a1, b1): c1, (a2,b2): c2}
    where the key is the transaction id, which is a tuple with
    a = source_entity_id, b = seq_number, and value c = transaction instance.
    """

    def __init__(
        self,
        entity_id,
        filestore,
        transport,
        eof_sent_indication_required=False,
        eof_received_indication_required=False,
        filesegment_received_indication_required=False,
        transaction_finished_indication_required=False,
        suspended_indication_required=False,
        resumed_indication_required=False,
        default_fault_handlers=DEFAULT_FAULT_HANDLERS,
        transaction_inactivity_limit=DEFAULT_INACTIVITY_TIMEOUT,
        positive_ack_timer_interval=DEFAULT_ACK_TIMER_LIMIT,
        positive_ack_timer_expiration_limit=DEFAULT_ACK_TIMER_EXPIRATION_LIMIT,
        nak_timer_interval=DEFAULT_NAK_TIMER_INTERVAL,
        nak_timer_expiration_limit=DEFAULT_NAK_TIMER_EXPIRATION_LIMIT,
    ):
        self.entity_id = entity_id
        self.filestore = filestore
        self.transport = transport
        self.eof_sent_indication_required = eof_sent_indication_required
        self.eof_received_indication_required = eof_received_indication_required
        self.filesegment_received_indication_required = (
            filesegment_received_indication_required
        )
        self.transaction_finished_indication_required = (
            transaction_finished_indication_required
        )
        self.suspended_indication_required = suspended_indication_required
        self.resumed_indication_required = resumed_indication_required
        self.default_fault_handlers = default_fault_handlers
        self.transaction_inactivity_limit = transaction_inactivity_limit
        self.positive_ack_timer_interval = positive_ack_timer_interval
        self.positive_ack_timer_expiration_limit = positive_ack_timer_expiration_limit
        self.nak_timer_interval = nak_timer_interval
        self.nak_timer_expiration_limit = nak_timer_expiration_limit

        self.transport.indication = lambda x: self.pdu_received(x)

        self._transaction_seq_counter = 0
        self.machines = {}

        self._event_queue = queue.Queue()
        self._event_thread = threading.Thread(target=self._event_handler)
        self._event_thread.kill = False
        self._event_thread.start()

        self._messages_to_user_queue = queue.Queue()
        self._message_thread = threading.Thread(target=self._message_handler)
        self._message_thread.kill = False
        self._message_thread.start()

        self._file_data_queue = queue.Queue()

    def _increment_transaction_seq_counter(self):
        self._transaction_seq_counter += 1
        return self._transaction_seq_counter

    def trigger_event(self, event, pdu=None):
        self._event_queue.put([event, pdu])

    def is_complete(self, transaction_id):
        machine = self.machines.get(transaction_id)
        if machine is None:
            return True
        else:
            return machine.state == MachineState.COMPLETED

    def _event_handler(self):
        thread = threading.current_thread()
        while not thread.kill:
            try:
                event, pdu = self._event_queue.get(timeout=1000)
                if thread.kill:
                    break
                machine = self.machines.get(event.transaction.id)
                if machine:
                    machine.update_state(event, pdu)
            except queue.Empty:
                pass

    def _message_handler(self):
        thread = threading.current_thread()
        while not thread.kill:
            try:
                message = self._messages_to_user_queue.get(timeout=1000)
                if thread.kill:
                    break
                logger.info("Processing user message %s" % str(message))
                self.process_user_message(message)
            except queue.Empty:
                pass

    def shutdown(self):
        self._event_thread.kill = True
        self._message_thread.kill = True
        self._event_queue.put([0, 0])  # needed to break the loop
        self._messages_to_user_queue.put(0)  # needed to break the loop

    """ CFDP service requests """

    def put(
        self,
        destination_id,
        source_filename=None,
        destination_filename=None,
        segmentation_control=None,  # not implemented
        fault_handler_overrides=None,
        flow_label=None,  # not implemented
        transmission_mode=None,
        closure_requested=False,  # not implemented
        messages_to_user=None,
        filestore_requests=None,
        checksum_type=ChecksumType.MODULAR,
    ):
        seq_number = self._increment_transaction_seq_counter()

        transaction = Transaction(
            kernel=self,
            source_entity_id=self.entity_id,
            seq_number=seq_number,
            destination_entity_id=destination_id,
            transmission_mode=transmission_mode,
            source_filename=source_filename,
            destination_filename=destination_filename,
            fault_handler_overrides=fault_handler_overrides,
            messages_to_user=messages_to_user,
            filestore_requests=filestore_requests,
            checksum_type=checksum_type,
        )

        if transmission_mode == TransmissionMode.UNACKNOWLEDGED:
            machine = Sender1(self, transaction)
        else:
            machine = Sender2(self, transaction)
        self.machines[transaction.id] = machine
        self.trigger_event(Event(transaction, EventType.E0_ENTERED_STATE))
        self.trigger_event(Event(transaction, EventType.E30_RECEIVED_PUT_REQUEST))
        return transaction.id

    def cancel(self, transaction_id):
        machine = self.machines.get(transaction_id)
        if machine:
            self.trigger_event(
                Event(machine.transaction, EventType.E33_RECEIVED_CANCEL_REQUEST)
            )

    def suspend(self, transaction_id):
        machine = self.machines.get(transaction_id)
        if machine:
            self.trigger_event(
                Event(machine.transaction, EventType.E31_RECEIVED_SUSPEND_REQUEST)
            )

    def resume(self, transaction_id):
        machine = self.machines.get(transaction_id)
        if machine:
            self.trigger_event(
                Event(machine.transaction, EventType.E32_RECEIVED_RESUME_REQUEST)
            )

    def report(self, transaction_id):
        machine = self.machines.get(transaction_id)
        if machine:
            self.trigger_event(
                Event(machine.transaction, EventType.E34_RECEIVED_REPORT_REQUEST)
            )

    """ CFDP service indications """

    def transaction_indication(self, transaction_id):
        """
        The Transaction.indication primitive shall be used to indicate the
        Transaction identifier to the source CFDP user.
        """
        logger.info("[%s] Transaction indication" % str(transaction_id))

    def eof_sent_indication(self, transaction_id):
        """
        The EOF-Sent.indication primitive shall be used to notify the source
        CFDP user of the initial transmission of a transaction’s EOF PDU.
        """
        logger.info("[%s] EOF sent indication" % str(transaction_id))

    def transaction_finished_indication(
        self,
        transaction_id,
        condition_code,
        file_status,
        delivery_code,
        filestore_responses=None,
        status_report=None,
    ):
        """
        The Transaction-Finished.indication primitive shall be used to indicate
        to the source or destination CFDP user that the transaction is complete
        and that the source CFDP user is authorized to modify or delete any
        retransmission buffer (file) that it was sharing with the protocol
        entity to conserve persistent storage space.
        """
        logger.info("[%s] Transaction finished indication" % str(transaction_id))

    def metadata_received_indication(
        self,
        transaction_id,
        source_entity_id,
        file_size=None,
        source_filename=None,
        destination_filename=None,
        messages_to_user=None,
    ):
        """
        The Metadata-Recv.indication primitive shall be used to indicate to the
        destination CFDP user that the metadata associated with a transaction
        has been received.
        """
        logger.info(
            "[%s] Metadata received indication (%s bytes)"
            % (str(transaction_id), str(file_size))
        )

    def filesegment_received_indication(
        self,
        transaction_id,
        offset,
        length,
        record_continuation_state=None,
        length_of_segment_metadata=None,
        segment_metadata=None,
    ):
        """
        The File-Segment-Recv.indication primitive shall be used to indicate
        the receipt of individual file data (non-metadata) segments to the
        destination CFDP user.
        """
        logger.info(
            "[%s] Filesegment received indication (offset %s, lenght %s)"
            % (transaction_id, offset, length)
        )

    def suspended_indication(self, transaction_id, condition_code):
        logger.info("[%s] Suspended indication" % str(transaction_id))

    def resumed_indication(self, transaction_id, progress):
        logger.info("[%s] Resumed indication" % str(transaction_id))

    def report_indication(self, transaction_id, status_report):
        """
        The Report.indication primitive shall be used to report to the CFDP
        user on the status of a transaction.
        """
        logger.info("[%s] Report indication" % str(transaction_id))

    def fault_indication(self, transaction_id, condition_code, progress):
        """
        The Fault.indication primitive shall be used to indicate to the
        CFDP user the occurrence of a fault condition for which the designated
        fault handler was ‘Ignore’.
        """
        logger.info("[%s] Fault indication" % str(transaction_id))

    def abandoned_indication(self, transaction_id, condition_code, progress):
        """
        The Abandoned.indication primitive shall be used to indicate to the
        CFDP user the occurrence of a fault condition for which the designated
        fault handler was ‘Abandon’.
        """
        logger.info("[%s] Abandoned indication" % str(transaction_id))

    def eof_received_indication(self, transaction_id):
        """
        The EOF-Recv.indication primitive shall be used to indicate to the
        destination CFDP user that the EOF PDU associated with a transaction
        has been received.
        """
        logger.info("[%s] EOF received indication" % str(transaction_id))

    """ Link state procedures """

    def freeze(self, receiving_entity_id):
        # freeze transmission for all transactions for which it is the
        # sending entity and the specified remote entity is the receiving one
        for transaction_id, machine in self.machines.items():
            if transaction_id[0] == self.entity_id:
                if machine.transaction.destination_entity_id == receiving_entity_id:
                    self.trigger_event(
                        Event(machine.transaction, EventType.E40_RECEIVED_FREEZE)
                    )

    def thaw(self, receiving_entity_id):
        for transaction_id, machine in self.machines.items():
            if transaction_id[0] == self.entity_id:
                if machine.transaction.destination_entity_id == receiving_entity_id:
                    self.trigger_event(
                        Event(machine.transaction, EventType.E41_RECEIVED_THAW)
                    )

    def pdu_received(self, data):
        """See CCSDS 720.2-G-3, chapter 5.4"""

        pdu = self._get_pdu_object(data)

        mode = pdu.pdu_header.transmission_mode
        direction = pdu.pdu_header.direction
        transaction_id = (
            pdu.pdu_header.source_entity_id,
            pdu.pdu_header.transaction_seq_number,
        )

        if transaction_id in self.machines:
            machine = self.machines[transaction_id]
        else:
            machine = None

        if machine and machine.transmission_mode == mode:
            if machine.state is not MachineState.COMPLETED:
                # deliver pdu to that state machine
                self._send_pdu_to_state_machine(machine, pdu)

            else:
                del self.machines[transaction_id]  # remove old state machine
                self._run_action_for_incoming_pdu(mode, direction, transaction_id, pdu)

        else:
            self._run_action_for_incoming_pdu(mode, direction, transaction_id, pdu)

    def _get_pdu_object(self, raw_pdu):
        "Take the raw input and return proper PDU object"

        pdu_header = PduHeader.decode(raw_pdu)

        # determine type of PDU
        if pdu_header.pdu_type == PduTypeCode.FILE_DATA:
            pdu = FiledataPdu.decode(raw_pdu)

        elif pdu_header.pdu_type == PduTypeCode.FILE_DIRECTIVE:
            code = raw_pdu[len(pdu_header)]

            if code == DirectiveCode.EOF:
                pdu = EofPdu.decode(raw_pdu)

            elif code == DirectiveCode.FINISHED:
                pdu = FinishedPdu.decode(raw_pdu)

            elif code == DirectiveCode.ACK:
                pdu = AckPdu.decode(raw_pdu)

            elif code == DirectiveCode.METADATA:
                pdu = MetadataPdu.decode(raw_pdu)

            elif code == DirectiveCode.NAK:
                pdu = NakPdu.decode(raw_pdu)

            elif code == DirectiveCode.PROMPT:
                pdu = PromptPdu.decode(raw_pdu)

            elif code == DirectiveCode.KEEP_ALIVE:
                pdu = KeepAlivePdu.decode(raw_pdu)

            else:
                raise RuntimeError

        return pdu

    def _send_pdu_to_state_machine(self, machine, pdu):
        if isinstance(pdu, MetadataPdu):
            self.trigger_event(
                Event(machine.transaction, EventType.E10_RECEIVED_METADATA), pdu
            )

        elif isinstance(pdu, FiledataPdu):
            self.trigger_event(
                Event(machine.transaction, EventType.E11_RECEIVED_FILEDATA), pdu
            )

        elif isinstance(pdu, EofPdu):
            if pdu.condition_code == ConditionCode.NO_ERROR:
                self.trigger_event(
                    Event(machine.transaction, EventType.E12_RECEIVED_EOF_NO_ERROR), pdu
                )
            else:
                self.trigger_event(
                    Event(machine.transaction, EventType.E13_RECEIVED_EOF_CANCEL), pdu
                )

        elif isinstance(pdu, AckPdu):
            if pdu.directive_code_ack == DirectiveCode.EOF:
                self.trigger_event(
                    Event(machine.transaction, EventType.E14_RECEIVED_ACK_EOF), pdu
                )
            elif pdu.directive_code_ack == DirectiveCode.FINISHED:
                self.trigger_event(
                    Event(machine.transaction, EventType.E18_RECEIVED_ACK_FINISHED), pdu
                )
            else:
                raise NotImplementedError

        elif isinstance(pdu, FinishedPdu):
            if pdu.condition_code == ConditionCode.NO_ERROR:
                self.trigger_event(
                    Event(
                        machine.transaction, EventType.E16_RECEIVED_FINISHED_NO_ERROR
                    ),
                    pdu,
                )
            else:
                self.trigger_event(
                    Event(machine.transaction, EventType.E17_RECEIVED_FINISHED_CANCEL),
                    pdu,
                )

        elif isinstance(pdu, NakPdu):
            self.trigger_event(
                Event(machine.transaction, EventType.E15_RECEIVED_NAK), pdu
            )

        else:
            raise RuntimeError

    def _run_action_for_incoming_pdu(self, mode, direction, transaction_id, pdu):
        """See CCSDS 720.2-G-3, Chapter 5.4, Table 5-5"""

        if (
            direction == Direction.TOWARD_SENDER
            and mode == TransmissionMode.UNACKNOWLEDGED
        ):
            # ignore
            pass

        elif (
            direction == Direction.TOWARD_SENDER
            and mode == TransmissionMode.ACKNOWLEDGED
        ):
            if isinstance(pdu, FinishedPdu):
                logger.debug("[{}] Send Ack Finished".format(transaction_id))
                ack_pdu = AckPdu(
                    direction=Direction.TOWARD_RECEIVER,
                    transmission_mode=pdu.pdu_header.transmission_mode,
                    source_entity_id=pdu.pdu_header.source_entity_id,
                    transaction_seq_number=pdu.pdu_header.transaction_seq_number,
                    destination_entity_id=pdu.pdu_header.destination_entity_id,
                    #
                    directive_code_ack=DirectiveCode.FINISHED,
                    directive_subtype_code=DirectiveSubTypeCode.ACK_FINISHED,
                    condition_code=ConditionCode.NO_ERROR,
                    transaction_status=TransactionStatus.UNDEFINED,
                )
                self.transport.request(ack_pdu.encode())

        elif direction == Direction.TOWARD_RECEIVER:
            # start a new state machine
            if (
                isinstance(pdu, MetadataPdu)
                or isinstance(pdu, FiledataPdu)
                or isinstance(pdu, EofPdu)
            ):
                if isinstance(pdu, MetadataPdu):
                    transaction = Transaction(
                        self,
                        pdu.pdu_header.source_entity_id,
                        pdu.pdu_header.transaction_seq_number,
                        pdu.pdu_header.destination_entity_id,
                        pdu.pdu_header.transmission_mode,
                        pdu.source_filename,
                        pdu.destination_filename,
                    )
                else:
                    transaction = Transaction(
                        self,
                        pdu.pdu_header.source_entity_id,
                        pdu.pdu_header.transaction_seq_number,
                        pdu.pdu_header.destination_entity_id,
                        pdu.pdu_header.transmission_mode,
                    )

                machine = self._new_state_machine(transaction, pdu)
                self.machines[transaction_id] = machine
                self.trigger_event(Event(transaction, EventType.E0_ENTERED_STATE))
                self._send_pdu_to_state_machine(machine, pdu)

    def _new_state_machine(self, transaction, pdu):
        if pdu.pdu_header.transmission_mode == TransmissionMode.UNACKNOWLEDGED:
            machine = Receiver1(self, transaction)
        else:
            machine = Receiver2(self, transaction)
        return machine

    def process_user_message(self, message):
        meta_process_user_message(self, message)
