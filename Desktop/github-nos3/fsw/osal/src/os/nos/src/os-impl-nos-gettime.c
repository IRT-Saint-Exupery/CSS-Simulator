/* Copyright (C) 2022 - 2022 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

/**
 * \file   os-impl-nos-gettime.c
 *
 * This file contains implementation for OS_GetTime() and OS_SetTime()
 * that map to the C library clock_gettime() and clock_settime() calls.
 * This should be usable on any OS that supports those standard calls.
 * The OS-specific code must \#include the correct headers that define the
 * prototypes for these functions before including this implementation file.
 *
 * NOTE: This is a "template" file and not a directly usable source file.
 *       It must be adapted/instantiated from within the OS-specific
 *       implementation on platforms that wish to use this template.
 */

/****************************************************************************************
                                    INCLUDE FILES
 ***************************************************************************************/

/*
 * Inclusions Defined by OSAL layer.
 *
 * This must provide the prototypes of these functions:
 *
 *   clock_gettime()
 *   clock_settime()
 *
 * and the "struct timespec" definition
 */
#include <string.h>
#include <errno.h>

#include "osapi-clock.h"
#include "os-impl-gettime.h"
#include "os-shared-clock.h"

#include "NOS-time.h"

extern int64_t          CFE_PSP_ticks_per_second;

/****************************************************************************************
                                FUNCTIONS
 ***************************************************************************************/


/*----------------------------------------------------------------
 *
 * Function: OS_GetLocalTime_Impl
 *
 *  Purpose: Implemented per internal OSAL API
 *           See prototype for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 OS_GetLocalTime_Impl(OS_time_t *time_struct)
{
    int             Status;
    int32           ReturnCode;
    struct timespec TimeSp;

    Status = NOS_clock_gettime(OSAL_GETTIME_SOURCE_CLOCK, &TimeSp);

    if (Status == 0)
    {
        //*time_struct = OS_TimeAssembleFromNanoseconds(TimeSp.tv_sec, TimeSp.tv_nsec);
        time_struct->ticks = (TimeSp.tv_sec * CFE_PSP_ticks_per_second) + 
                             (TimeSp.tv_nsec * (NOS_NANO / CFE_PSP_ticks_per_second));
        ReturnCode = OS_SUCCESS;
    }
    else
    {
        OS_DEBUG("Error calling clock_gettime: %s\n",strerror(errno));
        ReturnCode = OS_ERROR;
    }

    return ReturnCode;
}


/*----------------------------------------------------------------
 *
 * Function: OS_SetLocalTime_Impl
 *
 *  Purpose: Implemented per internal OSAL API
 *           See prototype for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 OS_SetLocalTime_Impl(const OS_time_t *time_struct)
{
    int             Status;
    int32           ReturnCode;
    struct timespec TimeSp;

    TimeSp.tv_sec  = OS_TimeGetTotalSeconds(*time_struct);
    TimeSp.tv_nsec = OS_TimeGetNanosecondsPart(*time_struct);

    Status = NOS_clock_settime(OSAL_GETTIME_SOURCE_CLOCK, &TimeSp);

    if (Status == 0)
    {
        ReturnCode = OS_SUCCESS;
    }
    else
    {
        ReturnCode = OS_ERROR;
    }

    return ReturnCode;

}
