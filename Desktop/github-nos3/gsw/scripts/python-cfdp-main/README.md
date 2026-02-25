# Python CFDP, Version 0.2

The CCSDS File Delivery Protocol (CFDP) was developed by the
[Consultative Committee for Space Data Systems (CCSDS)](https://public.ccsds.org).
The CFDP protocol provides reliable transfer of files from one endpoint to
another and has been designed to work well over space links that suffer
from outtakes and long delays. The basic operation of CFDP is to transfer a
file from a sender to a receiver (both referred to as CFDP entities). The
sender and receiver must be configured and running at the same time to perform
a file transfer. It can be used to perform  space to ground, ground to space,
space to space, and ground to ground file transfers. For example it can be used
to transfer science data from satellite over mission control to the science
center in an automated fashion with minimal to no human intervention.

This Python module is an implementation of the CCSDS File Delivery Protocol.
It supports all features as outlined in the latest version of the [CFDP Blue Book](docs/727x0b5.pdf). These are:

- Class 1 (unacknowledged) file transfer
- Class 2 (acknowledged) file transfer
- Filestore requests
- Proxy operations
- Directory listing request
- Native filestore implementation
- UDP as default transport layer (optional ZMQ)

## Installation

Clone the repository and then install via pip:

```
pip install cfdp
```

> Depending on the transport layer to use with CFDP, one needs to install additional dependencies.

## Documentation

Find the detailed documentation [here](docs/README.md).

## Tests

The protocol is tested for cross-support as outlined in CCSDS CFDP Yellow Book.
See [tests/README.md](tests/README.md) for details.

## Contribute

To learn more on how to successfully contribute please read the contributing
information in the [LibreCube documentation](https://librecube.gitlab.io/).

## Support

If you are having issues, please let us know. Reach us at
[Matrix](https://app.element.io/#/room/#librecube.org:matrix.org)
or via [Email](mailto:info@librecube.org).

## License

The project is licensed under the MIT license. See the [LICENSE](./LICENSE.txt) file for details.
