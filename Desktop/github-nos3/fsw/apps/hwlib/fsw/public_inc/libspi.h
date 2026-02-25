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

#ifndef _lib_spi_h_
#define _lib_spi_h_

/*
** Includes
*/ 
#include "hwlib.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__    
  #include <fcntl.h>      /* for posix open()  */
  #include <unistd.h>     /* for posix close() */
  #include <sys/ioctl.h>
  #include <linux/types.h>
  #include <linux/spi/spidev.h>
#endif


#ifdef __rtems__
    #include <rtems.h>
    #include <bsp.h>

    #include <errno.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>

    #include <rtems/libi2c.h>
    #include <rtems/libio.h>
    #include <grlib/spictrl.h>
    #include <rtems/bspIo.h>
#endif


/*
** Definitions
*/
#define SPI_DEVICE_CLOSED    0
#define SPI_DEVICE_OPEN      1

#define SPI_SUCCESS        		  OS_SUCCESS
#define SPI_ERROR          		  OS_ERROR
#define SPI_ERR_FILE_OPEN  		 -2 
#define SPI_ERR_FILE_CLOSE 		 -3
#define SPI_ERR_FILE_HANDLE		 -4
#define SPI_ERR_INVAL_MD   		 -5
#define SPI_ERR_WR_MODE    		 -6
#define SPI_ERR_RD_MODE    		 -7
#define SPI_ERR_WR_BPW     		 -8
#define SPI_ERR_RD_BPW     		 -9
#define SPI_ERR_WR_SD_HZ   		-10
#define SPI_ERR_RD_SD_HZ   		-11
#define SPI_ERR_IOC_MSG	   		-12
#define SPI_ERR_MUTEX_CREATE 	-13

#define MAX_SPI_BUSES        		3

/* 
** Structures
*/
typedef struct 
{
	uint32_t spi_mutex;
	uint8_t  users;
} spi_mutex_t;

typedef struct {
	const char*     deviceString;  /* uart string descriptor of the port  */
	int32_t   handle;	       /* handle to the hardware device */
	uint8_t   bus;           /* bus number for mutex use */
	uint8_t   cs;			       /* chip Select */
	uint32_t  baudrate;		   /* baudrate for the SPI */
	uint8_t   spi_mode;		   /* which of the four SPI-modes to use when transmitting */
	uint8_t   isOpen;        /* device status */
	uint8_t   bits_per_word; /* number of bits per word */
} spi_info_t;

/*
 * Initialize SPI device
 * @param device spi_info_t struct with all spi params
 * @return Returns SPI_SUCCESS or an error code 
 */
int32_t spi_init_dev(spi_info_t* device);

/*
 * Set SPI device mode
 * @param device spi_info_t struct with all spi params
 * @return Returns SPI_SUCCESS or an error code 
 */
int32_t spi_set_mode(spi_info_t* device);

/*
 * Get SPI device mode
 * @param device spi_info_t struct 
 * @return Returns SPI_SUCCESS or an error code 
 */
int32_t spi_get_mode(spi_info_t* device);

/*
 * Write a number of bytes to a given spi port
 * 
 * @param spi_info_t struct with all spi params
 * @param data array of the data to write
 * @param numBytes number of bytes to write the port
 * @return Returns error code: OS_SUCCESS, or OS_ERROR
*/
int32_t spi_write(spi_info_t* device, uint8_t data[], const uint32_t numBytes);

/*
 * Read a number of bytes off of a given spi port
 * 
 * @param spi_info_t struct with all spi params
 * @param data array to store the read data
 * @param numBytes number of bytes to read off the port
 * @return Returns error code: OS_SUCCESS, or OS_ERROR
*/
int32_t spi_read(spi_info_t* device, uint8_t data[], const uint32_t numBytes);

/*
 * Perform a full duplex SPI transaction
 * 
 * @param spi_info_t struct with all spi params
 * @param pointer to transmit data buffer or null, if null zeros are shifted out
 * @param pointer to receive data buffer or null
 * @param length of tx and rx buffers in bytes
 * @param if nonzero, how long to delay after the last bit transfer
 * @param number of bits per word 
 * @param (1) to leave SS active after transmit, (0) to leave SS non-active after transmit
 * @return Returns SPI_SUCCESS or an error code 
*/
int32_t spi_transaction(spi_info_t* device, uint8_t *txBuff, uint8_t * rxBuffer, uint32_t length, uint16_t delay, uint8_t bits, uint8_t deselect);

/*
 * For manual control of the CS line where needed
 * 
 * @param chipSelect the number of the CS line
 * @return Returns error code: OS_SUCCESS, or OS_ERROR
*/
int32_t spi_select_chip(spi_info_t* device);

int32_t spi_unselect_chip(spi_info_t* device);

/*
 * Close the SPI device 
 * 
 * @param spi_info_t struct with all spi params
 * @return Returns SPI_SUCCESS or an error code 
*/
int32_t spi_close_device(spi_info_t* device);
#endif /*_lib_spi_h_*/
