# Test Series for CFDP Transfer

This folder contains scripts for testing the CFDP operation. It follows closely
the tests provided in the CCSDS Yellow Book 720.4-Y-1.

The tests can be run automatically using pytests:

```bash
$ pytest  # to run all tests
$ pytest test_series_F1.py  # to run single file
```

Alternative, to show debug messages, the scripts can be run on their own:

```bash
$ python test_series_F1.py  # to run single file
```

## Test Series F1

This first Test Series is simple in order to expedite testing and to establish
a confidence baseline for Series F2 tests, which initiate thorough checking of
protocol procedures. Demonstrations are made of Unacknowledged and Acknowledged
modes, of canceling an ongoing transaction, and of user messages.

## Test Series F2

The second Test Series initiates thorough checking of protocol procedures of
the Acknowledged mode, including automatic recovery from dropping of the
metadata PDU, of each of the positively acknowledged PDUs (EOF and Finished),
of the positive acknowledgements to those PDUs, as well as simulation of an
extremely noisy link in which every PDU in each direction is dropped once,
and simulation of a fault.

## Test Series F3

The third Test Series checks the functioning of the two party Remote Put (Proxy)
functions and checks the operation of the Filestore Procedures and Directory
Listing Request.

## Test Series F4

The fourth Test Series checks the operation of each of the options not checked
in a previous Test Series. (Options between an Entity and its User are not tested
as they are not interoperability issues.)

## Test Series F5

The fifth Test Series checks the functioning of the three party Remote Put
(Proxy) functions.
