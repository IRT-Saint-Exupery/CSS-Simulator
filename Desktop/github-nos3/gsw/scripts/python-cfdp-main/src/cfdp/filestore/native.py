import os
import shutil
import tempfile

from .base import VirtualFileStore


class NativeFileStore(VirtualFileStore):

    """
    The Native File Store implements the Virtual File Store as a platform
    dependent file system, using the Python 'os.path' library.

    A rootpath is provided as class instantiation. The VFS paths are then
    relative to this root path. For example, if root path is '/home/demo'
    and a Put Request places a file to '/files/small.txt', then this will
    land in '/home/demo/files/small.txt'.

    """

    def __init__(self, rootpath):
        super().__init__()
        self.rootpath = os.path.abspath(rootpath)

    def get_native_path(self, path):
        # convert from virtual filestore path to absolute native path
        path_items = path.split("/")
        path_items = [x for x in path_items if x != ""]
        if path_items:
            path = os.path.join(*path_items)
        else:
            path = ""
        return os.path.join(self.rootpath, path)

    def get_virtual_path(self, path):
        # convert from native relative path to virtual path
        path = "/".join(os.path.normpath(path).split(os.path.sep))
        if not path.startswith("/"):
            path = "/" + path
        return path

    def create_file(self, filepath):
        NativeFileHandle(self.get_native_path(filepath), "wb").close()

    def delete_file(self, filepath):
        os.remove(self.get_native_path(filepath))

    def rename_file(self, filepath1, filepath2):
        os.rename(self.get_native_path(filepath1), self.get_native_path(filepath2))

    def append_file(self, filepath1, filepath2):
        fh1 = open(self.get_native_path(filepath1), "ab")
        fh2 = open(self.get_native_path(filepath2), "rb")
        fh1.write(fh2.read())
        fh1.close()
        fh2.close()

    def replace_file(self, filepath1, filepath2):
        fh1 = open(self.get_native_path(filepath1), "wb")
        fh2 = open(self.get_native_path(filepath2), "rb")
        fh1.write(fh2.read())
        fh1.close()
        fh2.close()

    def create_directory(self, dirpath):
        os.mkdir(self.get_native_path(dirpath))

    def remove_directory(self, dirpath):
        shutil.rmtree(self.get_native_path(dirpath))

    def list_directory(self, dirpath):
        # create directory listing
        dirpath = self.get_native_path(dirpath)
        # get the full path w.r.t to host file system
        dirpath, dirnames, filenames = next(os.walk(dirpath))

        listing = "type,path,size,timestamp\n"
        for dirname in dirnames:
            fullpath = os.path.join(dirpath, dirname)
            relpath = os.path.relpath(fullpath, self.rootpath)
            line = "{},{},{},{}\n".format(
                "d",
                self.get_virtual_path(relpath),
                os.path.getsize(fullpath),
                os.path.getmtime(fullpath),
            )
            listing += line
        for filename in filenames:
            fullpath = os.path.join(dirpath, filename)
            relpath = os.path.relpath(fullpath, self.rootpath)
            line = "{},{},{},{}\n".format(
                "f",
                self.get_virtual_path(relpath),
                os.path.getsize(fullpath),
                os.path.getmtime(fullpath),
            )
            listing += line
        return listing

    def open(self, filepath, mode="rb"):
        return NativeFileHandle(self.get_native_path(filepath), mode)

    def open_tempfile(self):
        return NativeTemporaryFileHandle()

    def is_file(self, filepath):
        return os.path.isfile(self.get_native_path(filepath))

    def get_size(self, filepath):
        return os.path.getsize(self.get_native_path(filepath))

    def join_path(self, *args):
        return "/".join(args)


class NativeFileHandle:

    """
    The NativeFileHandle provides a generic interface to Python's 'open'
    function. It also implements method for calculating the file checksum.

    """

    def __init__(self, filepath, mode="rb"):
        self.handle = open(filepath, mode)

    def get_file_size(self):
        pass

    def seek(self, *args, **kwargs):
        return self.handle.seek(*args, **kwargs)

    def tell(self, *args, **kwargs):
        return self.handle.tell(*args, **kwargs)

    def read(self, *args, **kwargs):
        return self.handle.read(*args, **kwargs)

    def write(self, *args, **kwargs):
        return self.handle.write(*args, **kwargs)

    def close(self, *args, **kwargs):
        return self.handle.close(*args, **kwargs)

    def calculate_checksum(self, type):
        if type == 0:  # Modular, see Annex F of CCSC 727.0-B-5
            file_size = self.handle.seek(0, 2)
            checksum = 0
            a = b""
            x = 0
            while x < file_size:
                self.handle.seek(x)
                if x > file_size - 4:
                    a = self.handle.read(file_size % 4)
                    int_a = int(a.hex(), 16) * 2 ** ((4 - file_size % 4) * 8)
                else:
                    a = self.handle.read(4)
                    int_a = int(a.hex(), 16)
                checksum = checksum + int_a
                x = x + 4
            checksum = checksum % 2 ** 32
            return checksum
        elif type == 15:  # Null checksum
            return 0
        else:
            # TODO: implemented other checksum algorithms
            raise NotImplementedError


class NativeTemporaryFileHandle(NativeFileHandle):
    def __init__(self):
        self.handle = tempfile.TemporaryFile()
