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


/* IOCTL commands */
#include <sys/ioctl.h>

#define TMRCTR_MAGIC                            15

#define TMRCTR_START                _IO(    TMRCTR_MAGIC, 0         )
#define TMRCTR_STOP                 _IO(    TMRCTR_MAGIC, 1         )
#define TMRCTR_PWM_ENABLE           _IO(    TMRCTR_MAGIC, 2         )
#define TMRCTR_PWM_SET_PERIOD       _IOW(   TMRCTR_MAGIC, 3,    long)
#define TMRCTR_PWM_SET_HIGH_TIME    _IOW(   TMRCTR_MAGIC, 4,    long)
#define TMRCTR_PWM_DISABLE          _IO(    TMRCTR_MAGIC, 5         )
#define TMRCTR_SELFTEST             _IO(    TMRCTR_MAGIC, 6         )
#define TMRCTR_SETOPTION            _IOW(   TMRCTR_MAGIC, 7,    long)
#define TMRCTR_GETOPTION            _IOR(   TMRCTR_MAGIC, 8,    long)
#define TMRCTR_RESET                _IO(    TMRCTR_MAGIC, 9         )
#define TMRCTR_SET_RESET_VALUE      _IOW(   TMRCTR_MAGIC, 10,   long)
#define TMRCTR_GET_VALUE            _IOR(   TMRCTR_MAGIC, 11,   long)
#define TMRCTR_GET_CAPTURE_VALUE    _IOR(   TMRCTR_MAGIC, 12,   long)
#define TMRCTR_CHECK_UPDATE         _IOR(   TMRCTR_MAGIC, 13,   long)
#define TMRCTR_TMR_SELECT           _IOW(   TMRCTR_MAGIC, 14,   long)

/* IOCTL Errors */
#define TMRCTR_BAD_CMD            (-1)

