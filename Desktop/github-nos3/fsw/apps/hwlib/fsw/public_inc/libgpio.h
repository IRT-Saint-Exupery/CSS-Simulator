/* Copyright (C) 2009 - 2019 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#ifndef _lib_gpio_h_
#define _lib_gpio_h_

/* Includes */
#include "hwlib.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#if defined __linux__ 
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/ioctl.h>
#endif

/* Defines */
#define GPIO_SUCCESS            OS_SUCCESS
#define GPIO_ERROR              OS_ERROR
#define GPIO_FD_OPEN_ERR        OS_ERR_FILE
#define GPIO_WRITE_ERR          -3
#define GPIO_READ_ERR           -4

#define GPIO_INPUT              0
#define GPIO_OUTPUT             1

#define GPIO_OPEN               1
#define GPIO_CLOSED             0

#ifdef __cplusplus
extern "C" {
#endif

/* Structures */
typedef struct
{
    uint32_t pin;                /* pin number to initialize */
    int32_t  handle;             /* handle to device */ 
    uint8_t  direction;          /* in or out */
    uint8_t  isOpen;             
} gpio_info_t;

/* Prototypes */
void gpio_dummy(void);

/*
 * Initialize GPIO pin
 * @param device - GPIO device information
 * @return Returns GPIO_SUCCESS or an error code defined above
 */
int32_t gpio_init(gpio_info_t* device);

/*
 * Read a specified GPIO pin value
 * @param device - GPIO device information
 * @param value - uint8_t pointer to store read value in
 * @return Returns GPIO_SUCCESS or an error code defined above
 */
int32_t gpio_read(gpio_info_t* device, uint8_t* value);

/*
 * Write `value` to a specified GPIO pin
 * @param device - GPIO device information
 * @param value - value to write to the pin (1 or 0)
 * @return Returns GPIO_SUCCESS or an error code defined above
 */
int32_t gpio_write(gpio_info_t* device, uint8_t value);

/*
 * Close GPIO handle
 * @param device - GPIO device information
 * @return Returns GPIO_SUCCESS or an error code defined above
 */
int32_t gpio_close(gpio_info_t* device);

#ifdef __cplusplus
}
#endif

#endif
