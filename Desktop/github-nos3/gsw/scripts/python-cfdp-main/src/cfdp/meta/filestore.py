from cfdp import logger
from cfdp.constants import TypeFieldCode, ActionCode
from .datafield import TypeLengthValue


class FilestoreRequest(TypeLengthValue):
    def __init__(self, action_code, first_filename, second_filename=None):
        self.type = TypeFieldCode.FILESTORE_REQUEST

        self.action_code = action_code
        self.first_filename = first_filename
        self.second_filename = second_filename

    def encode(self):
        data = bytes([(self.action_code << 4)])

        value = self.first_filename.encode("utf-8")
        length = len(value)
        data += bytes([length])
        data += value

        if self.second_filename:
            value = self.second_filename.encode("utf-8")
            length = len(value)
            data += bytes([length])
            data += value

        self.value = data
        return super().encode()


def execute_filestore_requests(kernel, filestore_requests):
    for filestore_request in filestore_requests:
        if filestore_request.action_code == ActionCode.CREATE_FILE:
            filename = filestore_request.first_filename.decode()
            logger.info("Create file: <{}>".format(filename))
            try:
                kernel.filestore.create_file(filename)
            except Exception:
                logger.error("Failed to create file <{}>".format(filename))

        elif filestore_request.action_code == ActionCode.DELETE_FILE:
            filename = filestore_request.first_filename.decode()
            logger.info("Delete file: <{}>".format(filename))
            try:
                kernel.filestore.delete_file(filename)
            except Exception:
                logger.error("Failed to delete file: <{}>".format(filename))

        elif filestore_request.action_code == ActionCode.RENAME_FILE:
            filename1 = filestore_request.first_filename.decode()
            filename2 = filestore_request.second_filename.decode()
            logger.info("Rename file: <{}> to <{}>".format(filename1, filename2))
            try:
                kernel.filestore.rename_file(filename1, filename2)
            except Exception:
                logger.error(
                    "Failed to rename file <{}> to <{}>".format(filename1, filename2)
                )

        elif filestore_request.action_code == ActionCode.APPEND_FILE:
            filename1 = filestore_request.first_filename.decode()
            filename2 = filestore_request.second_filename.decode()
            logger.info("Append file <{}> with <{}>".format(filename1, filename2))
            try:
                kernel.filestore.append_file(filename1, filename2)
            except Exception:
                logger.error(
                    "Failed to append file <{}> with <{}>".format(filename1, filename2)
                )

        elif filestore_request.action_code == ActionCode.REPLACE_FILE:
            filename1 = filestore_request.first_filename.decode()
            filename2 = filestore_request.second_filename.decode()
            logger.info(
                "Replace content of file <{}> by <{}>".format(filename1, filename2)
            )
            try:
                kernel.filestore.replace_file(filename1, filename2)
            except Exception:
                logger.error(
                    "Failed to replace content of <{}> by <{}>".format(
                        filename1, filename2
                    )
                )

        elif filestore_request.action_code == ActionCode.CREATE_DIRECTORY:
            dirname = filestore_request.first_filename.decode()
            logger.info("Create directory: <{}>".format(dirname))
            try:
                kernel.filestore.create_directory(dirname)
            except Exception:
                logger.error("Failed to create directory: <{}>".format(dirname))

        elif filestore_request.action_code == ActionCode.REMOVE_DIRECTORY:
            dirname = filestore_request.first_filename.decode()
            logger.info("Remove directory: <{}>".format(dirname))
            try:
                kernel.filestore.remove_directory(dirname)
            except Exception:
                logger.error("Failed to delete directory: <{}>".format(dirname))

        elif filestore_request.action_code == ActionCode.DENY_FILE:
            filename = filestore_request.first_filename.decode()
            logger.info("Deny file: <{}>".format(filename))
            try:
                kernel.filestore.delete_file(filename)
            except Exception:
                pass

        elif filestore_request.action_code == ActionCode.DENY_DIRECTORY:
            dirname = filestore_request.first_filename.decode()
            logger.info("Deny directory: <{}>".format(dirname))
            try:
                kernel.filestore.remove_directory(dirname)
            except Exception:
                pass


def extract_filestore_request(data):
    action_code = data[0] >> 4
    length_of_first_filename = data[1]
    first_filename = data[2 : 2 + length_of_first_filename]

    if action_code in [
        ActionCode.RENAME_FILE,
        ActionCode.APPEND_FILE,
        ActionCode.REPLACE_FILE,
    ]:
        second_filename = data[3 + length_of_first_filename :]
    else:
        second_filename = None

    return FilestoreRequest(action_code, first_filename, second_filename)
