import socket

kill_command = [0x18, 0x06, 0xC0, 0x00, 0x00, 0x015, 0x32, 0x05, 0x4B, 0x49, 0x54, 0x5f, 0x43, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]

# This command send a stop and delete command for the KIT_CI application on OSK (command Ingest Module)

# Without the Command Ingest application, the sattelite can not recieve or process any commands/telemetry

# Without a way to process commands, the CI app will never restart, thus leaving the sat permanently INOP
# Without a physical reset being made


def transmit(command):
    byte_message = bytes(command)
    opened_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    opened_socket.sendto(byte_message, ("127.0.0.1", 1234))


transmit(kill_command)