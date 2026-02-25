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

#ifndef _lib_uart_h_
#define _lib_uart_h_

/*
** Includes
*/
#include "hwlib.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>         /* for posix open()  */
#include <unistd.h>        /* for posix close() */
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>


/*
** Defines
*/
#define PORT_CLOSED  0
#define PORT_OPEN    1

#define UART_SUCCESS            OS_SUCCESS
#define UART_ERROR              OS_ERROR
#define UART_FD_OPEN            OS_ERR_FILE


/*
** Enums and Structs
*/
typedef enum
{
    uart_access_flag_RDONLY = 0,
    uart_access_flag_WRONLY = 1,
    uart_access_flag_RDWR   = 2
} uart_access_flag;

typedef struct
{
    const char  *deviceString; /* uart string descriptor of the port  */
    int32_t  handle;           /* handle to device */
    uint8_t  isOpen;           /* port status */
    uint32_t baud;             /* baud rate */
    uint8_t  canonicalModeOn;  /* turn on canonical mode */
    struct termios options;
    uart_access_flag access_option;
} uart_info_t;


/*
 * Generic uart initialization/ port open
 * 
 * @param device uart_info_t struct with all uart params
 * @return Returns error code: UART_SUCCESS or specific UART_ERROR
*/
int32_t uart_init_port(uart_info_t* device);

/*
 * Get the number of bytes waiting to be read for a given port
 * 
 * @param device uart_info_t struct with all uart params
 * @return Returns number of bytes available
*/
int32_t uart_bytes_available(uart_info_t* device);

/*
 * Flushes data waiting to be read for a given port
 * 
 * @param device uart_info_t struct with all uart params
 * @return Returns error code: UART_SUCCESS or specific UART_ERROR
*/
int32_t uart_flush(uart_info_t* device);

/*
 * Read a number of bytes off of a given uart port
 * 
 * @param device uart_info_t struct with all uart params
 * @param data array to store the read data
 * @param numBytes number of bytes to read off the port
 * @return Returns number of bytes successfully read
*/
int32_t uart_read_port(uart_info_t* device, uint8_t data[], const uint32_t numBytes);

/*
 * Write a number of bytes to of a given uart port
 * 
 * @param device uart_info_t struct with all uart params
 * @param data array of the data to write
 * @param numBytes number of bytes to write to the port
 * @return Returns number of bytes successfully written
*/
int32_t uart_write_port(uart_info_t* device, uint8_t data[], const uint32_t numBytes);

/*
 * Generic uart port close
 * 
 * @param device uart_info_t struct with all uart params
 * @return Returns error code: UART_SUCCESS or specific UART_ERROR
*/
int32_t uart_close_port(uart_info_t* device);

#endif