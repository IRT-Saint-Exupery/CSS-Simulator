/* Copyright (C) 2009 - 2020 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#ifndef _lib_trq_h_
#define _lib_trq_h_

#include "hwlib.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __linux__
    #include <unistd.h>
#endif

#ifdef __rtems__
    #include <bsp.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/ioctl.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <math.h>
    #include <rtems/bspIo.h>
    #include <grlib/grpwm.h>
#endif

#define TRQ_SUCCESS             OS_SUCCESS
#define TRQ_ERROR               OS_ERROR
#define TRQ_INIT_ERR            -1
#define TRQ_SELFTEST_ERR        -2
#define TRQ_CONNECT_ERR         -3
#define TRQ_NUM_ERR             -4
#define TRQ_TIME_HIGH_VAL_ERR   -5

#define TRQ_DIR_POSITIVE        1
#define TRQ_DIR_NEGATIVE        0

/* Torquer info struct */
typedef struct {
    // User initialized fields
    uint8_t     trq_num;            // torquer number (0, 1, 2)
    uint32_t    timer_period_ns;    // nanoseconds
    // HWLIB managed fields
    int         timerfd; 
    int         direction_pin_fd;
    uint32_t    timer_high_ns;      // nanoseconds 
    bool        positive_direction; // TRUE = Positive direction, FALSE = Negative direction
    bool        enabled;
} trq_info_t;

void trq_dummy(void);

int32_t trq_set_time_high(trq_info_t* device, uint32_t new_time);
int32_t trq_set_period(trq_info_t* device);
int32_t trq_set_direction(trq_info_t* device, bool direction);

int32_t trq_init(trq_info_t* device); 
int32_t trq_command(trq_info_t *device, uint8_t percent_high, bool pos_dir);
void trq_close(trq_info_t* device);

#endif
