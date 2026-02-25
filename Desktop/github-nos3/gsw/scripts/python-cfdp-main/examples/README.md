# Examples

This folder contains examples for using the CFDP package. The examples are grouped per transport layer.
The most complete examples are in the UDP transport folder.

For each example first start a a remote entity (acting as server):

```
$ cd <example-folder>
$ python remote.py
```

Then run one of the local entity (acting as client) examples:

```
$ cd <example-folder>
$ python local_send_file_class1.py
```
