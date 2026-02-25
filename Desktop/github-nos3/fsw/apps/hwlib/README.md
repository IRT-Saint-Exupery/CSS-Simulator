# Hardware Library
The hardware library provides an abstraction layer for hardware interfaces across platforms such as:
* linux
* nos-linux

Note that the hardware library does not include `cfe.h` or `osapi.h`.
When using with cFS these includes must be in the `cpuX_device_cfg.h` file.

# Protocol Specifics

## CAN
Note that the currently maximum number of allocated devices is 30.

## I2C
Note that the currently maximum number of allocated devices is 30.

## SPI
Note that currently the maximum number of allocated buses is 3, with each bus supporting 10 devices.
Due to the use of GPIO pins as chip selects, it is expected that the following order is used when leveraging a device:
* `spi_select_chip` - this grabs the bus specific mutex
* Select GPIO pin if necessary
* `spi_transaction`
  - multiple if necessary
* Unselect GPIO pin if necessary
* `spi_unselect_chip`

## UART
Note that the currently maximum number of allocated devices is 30.
