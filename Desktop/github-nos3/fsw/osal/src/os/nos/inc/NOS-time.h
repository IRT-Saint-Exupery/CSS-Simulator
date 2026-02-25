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


/* NOS stub replacement for time.h */
#ifndef _NOS_STUB_TIME_H_
#define _NOS_STUB_TIME_H_

#include <time.h>

#define NOS_NANO 1000000000

/* ----------------------------------------- */
/* prototypes normally declared in time.h */
/* ----------------------------------------- */

int NOS_clock_getres (clockid_t clock_id, struct timespec * res);
int NOS_clock_gettime (clockid_t clock_id, struct timespec * tp);
int NOS_clock_nanosleep (clockid_t clock_id, int flags, const struct timespec * req, struct timespec * rem);
int NOS_clock_settime (clockid_t clock_id, const struct timespec * tp);
int NOS_timer_create (clockid_t clock_id, struct sigevent * evp, timer_t * timerid);
int NOS_timer_delete (timer_t timerid) ;
// int NOS_timer_gettime (timer_t timerid, struct itimerspec * value); // unused
int NOS_timer_settime (timer_t timerid, int flags, const struct itimerspec * value, struct itimerspec * ovalue);
void NOS_to_real_timespec(const struct timespec *nos, struct timespec *real);

#endif /* _NOS_STUB_TIME_H_ */

