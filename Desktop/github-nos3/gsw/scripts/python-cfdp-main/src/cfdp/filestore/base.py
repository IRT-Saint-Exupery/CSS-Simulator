class VirtualFileStore:

    """
    The Virtual File Store (VFS) is an abstraction of file system. An
    implementation must provide the methods listed below.

    An implementation can map the VFS to a platform dependent native file
    system or a packet store for example.

    The VFS starts at root '/' and uses slashes for subfolders. This is how
    pathes are to be passed to CFDP methods (eg. Put Request, Filestore
    Requests, and Directory Listings.)

    Example: '/files/a_file.txt'

    """

    def __init__(self):
        self.config = None

    def create_file(self, filepath):
        raise NotImplementedError

    def delete_file(self, filepath):
        raise NotImplementedError

    def rename_file(self, filepath1, filepath2):
        raise NotImplementedError

    def append_file(self, filepath1, filepath2):
        raise NotImplementedError

    def replace_file(self, filepath1, filepath2):
        raise NotImplementedError

    def create_directory(self, dirpath):
        raise NotImplementedError

    def remove_directory(self, dirpath):
        raise NotImplementedError

    def list_directory(self, dirpath):
        raise NotImplementedError
