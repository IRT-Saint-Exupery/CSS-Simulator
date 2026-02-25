/* Copyright (C) 2009 - 2018 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
any warranty that the software will be error free.

In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
documentation or services provided hereunder

ITC Team
NASA IV&V
ivv-itc@lists.nasa.gov
*/

#ifndef _lib_i2c_h_
#define _lib_i2c_h_

#include "hwlib.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __linux__
  #include <fcntl.h>
  #include <sys/time.h>
  #include <linux/i2c.h>
  #include <linux/i2c-dev.h>
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

#ifdef __rtems__
  #include <errno.h>
  #include <string.h>
  #include <rtems/libi2c.h>
  #include <rtems/libio.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <grlib/i2cmst.h>
  #include "rtos_i2c_rdwr.h"
#endif


/*
 * I2C device modes
*/
#define I2C_OPEN                0
#define I2C_CLOSED              1

/* Number of I2C busses */
#define NUM_I2C                 2

/* Max size of I2C transaction */
#define I2C_MAX_BYTES           128

/* Defines */
#define I2C_SUCCESS            OS_SUCCESS
#define I2C_ERROR              OS_ERROR
#define I2C_FD_OPEN_ERR        OS_ERR_FILE

/*
** I2C bus info struct
*/
typedef struct
{
    int32_t  handle;   /* handle to the fd */
    int32_t  addr;     /* slave address */
    uint8_t  isOpen;   /* port status */
    uint32_t speed;    
} i2c_bus_info_t;

/* 
 * Initialize I2C handle as Master with bus speed
 *
 * @param device Which I2C bus (if more than one exists)
 * @param speed Bus speed in kbps
 * @return Returns error code: I2C_SUCCESS, or I2C_ERROR
*/
int32_t i2c_master_init(i2c_bus_info_t* device);

/**
 * Execute an I2C master write and slave read in one transaction
 *
 * @param device Which I2C bus (if more than one exists)
 * @param addr I2C address, not bit-shifted
 * @param txbuf pointer to tx data
 * @param txlen length of tx data
 * @param rxbuf pointer to rx data
 * @param rxlen length of rx data
 * @param timeout Number of ticks to wait for a frame
 * @return Returns error code: I2C_SUCCESS if a frame is received, or I2C_ERROR if timed out or if handle is not a valid device
 */
int32_t i2c_master_transaction(i2c_bus_info_t* device, uint8_t addr, void * txbuf, uint8_t txlen, void * rxbuf, uint8_t rxlen, uint16_t timeout);

/**
 * Execute an I2C slave read
 *
 * @param device Which I2C bus (if more than one exists)
 * @param addr I2C address, not bit-shifted
 * @param rxbuf pointer to rx data
 * @param rxlen length of rx data
 * @param timeout Number of ticks to wait for a frame
 * @return Returns error code: I2C_SUCCESS if a frame is received, or I2C_ERROR if timed out or if handle is not a valid device
 */
int32_t i2c_read_transaction(i2c_bus_info_t* device, uint8_t addr, void * rxbuf, uint8_t rxlen, uint8_t timeout);

/**
 * Execute an I2C master write
 *
 * @param device Which I2C bus (if more than one exists)
 * @param addr I2C address, not bit-shifted
 * @param txbuf pointer to tx data
 * @param txlen length of tx data
 * @param timeout Number of ticks to wait for a frame
 * @return Returns error code: I2C_SUCCESS if a frame is received, or I2C_ERROR if timed out or if handle is not a valid device
 */
int32_t i2c_write_transaction(i2c_bus_info_t* device, uint8_t addr, void * txbuf, uint8_t txlen, uint8_t timeout);

/**
 * Execute multiple I2C transactions without stops
 *
 * @param device Which I2C bus (if more than one exists)
 * @param addr I2C address, not bit-shifted
 * @param i2c_rdwr_ioctl_data structure containing multiple messages
 * @param timeout Number of ticks to wait for a frame
 * @return Returns error code: I2C_SUCCESS if a frame is received, or I2C_ERROR if timed out or if handle is not a valid device
 */
int32_t i2c_multiple_transaction(i2c_bus_info_t* device, uint8_t addr, struct i2c_rdwr_ioctl_data* rdwr_data, uint16_t timeout);

/**
 * Close an I2C bus
 *
 * @param device Which I2C bus (if more than one exists)
 * @return Returns error code: I2C_SUCCESS if close was succes, or I2C_ERROR if close fails
 */
int32_t i2c_master_close(i2c_bus_info_t* device);

#endif
