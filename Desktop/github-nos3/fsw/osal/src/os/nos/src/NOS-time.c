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

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <osconfig.h>
#include "os-impl-timebase.h"
#include "NOS-time.h"

#include "Client/CInterface.h"

extern NE_Bus          *CFE_PSP_Bus;
extern int64_t          CFE_PSP_ticks_per_second;
extern pthread_mutex_t  CFE_PSP_sim_time_mutex;
extern NE_SimTime       CFE_PSP_sim_time;

int NOS_clock_getres (clockid_t clock_id, struct timespec * res)
{
    res->tv_sec = 0;
    res->tv_nsec = NOS_NANO / CFE_PSP_ticks_per_second;
    return 0;
}

int NOS_clock_gettime (clockid_t clock_id, struct timespec * tp)
{
    pthread_mutex_lock(&CFE_PSP_sim_time_mutex);
    NE_SimTime sim_time = CFE_PSP_sim_time;
    pthread_mutex_unlock(&CFE_PSP_sim_time_mutex);
    tp->tv_sec = sim_time / CFE_PSP_ticks_per_second;
    tp->tv_nsec = (sim_time % CFE_PSP_ticks_per_second) * (NOS_NANO / CFE_PSP_ticks_per_second);
    return 0;
}

int NOS_clock_nanosleep (clockid_t clock_id, int flags, const struct timespec * req, struct timespec * rem)
{
    if ((req->tv_sec < 0) || (req->tv_nsec < 0) || (req->tv_nsec >= NOS_NANO)) {
        errno = EINVAL;
        return -1;
    }
    pthread_mutex_lock(&CFE_PSP_sim_time_mutex);
    NE_SimTime sim_time = CFE_PSP_sim_time;
    pthread_mutex_unlock(&CFE_PSP_sim_time_mutex);
    NE_SimTime end_time = req->tv_sec * CFE_PSP_ticks_per_second + req->tv_nsec * CFE_PSP_ticks_per_second / NOS_NANO;
    if (flags == 0) end_time += sim_time; // relative time
    struct timespec delay, real_rem;
    delay.tv_sec = 0;
    delay.tv_nsec = NOS_NANO / CFE_PSP_ticks_per_second;
    int interrupt = 0;
    while ((sim_time < end_time) && !interrupt) {
        interrupt = clock_nanosleep (clock_id, flags, &delay, &real_rem);
        pthread_mutex_lock(&CFE_PSP_sim_time_mutex);
        sim_time = CFE_PSP_sim_time;
        pthread_mutex_unlock(&CFE_PSP_sim_time_mutex);
    }
    if (rem != NULL) {
        NE_SimTime delta = end_time - sim_time;
        if (delta < 0) delta = 0;
        rem->tv_sec = delta / CFE_PSP_ticks_per_second;
        rem->tv_nsec = ((delta % CFE_PSP_ticks_per_second) * NOS_NANO / CFE_PSP_ticks_per_second) + real_rem.tv_nsec;
    }
    return interrupt;
}

int NOS_clock_settime (clockid_t clock_id, const struct timespec * tp)
{
    if ((tp->tv_sec < 0) || (tp->tv_nsec < 0) || (tp->tv_nsec >= NOS_NANO)) {
        errno = EINVAL;
        return -1;
    }
    NE_SimTime sim_time = tp->tv_sec * CFE_PSP_ticks_per_second + tp->tv_nsec * CFE_PSP_ticks_per_second / NOS_NANO;
    NE_bus_set_time(CFE_PSP_Bus, sim_time);
    return 0;
}


typedef struct NOS_timer_t {
    int in_use;
    struct sigevent evp;
    int armed;
    struct timespec it_interval;
    NE_SimTime expire_time;
} NOS_timer_t;

static pthread_mutex_t NOS_timer_table_mutex;
static NOS_timer_t NOS_timer_table[OS_MAX_TIMEBASES];

int NOS_timer_create (clockid_t clock_id, struct sigevent * evp, timer_t * timerid)
{
    int retval = 0;
    int i;
    pthread_mutex_lock(&NOS_timer_table_mutex);
    for (i = 0; i < OS_MAX_TIMEBASES; i++) {
        if (NOS_timer_table[i].in_use == 0) break;
    }
    if (i >= OS_MAX_TIMEBASES) {
        errno = ENOMEM;
        retval = -1;
    } else {
        *timerid = (void *)i;
        NOS_timer_table[i].in_use = 1;
        NOS_timer_table[i].evp = *evp;
        retval = 0;
    }
    pthread_mutex_unlock(&NOS_timer_table_mutex);
    return retval;
}

int NOS_timer_delete (timer_t timerid)
{
    int retval = 0;
    int i = (int)timerid;
    pthread_mutex_lock(&NOS_timer_table_mutex);
    if ((i < 0) || (i >= OS_MAX_TIMEBASES)) {
        errno = EINVAL;
        retval = -1;
    } else {
        NOS_timer_table[i].armed = 0;
        NOS_timer_table[i].in_use = 0;
        retval = 0;
    }
    pthread_mutex_unlock(&NOS_timer_table_mutex);
    return retval;
}

// unused
//int NOS_timer_gettime (timer_t timerid, struct itimerspec * value)
//{
//    return timer_gettime(timerid, value);
//}

int NOS_timer_settime (timer_t timerid, int flags, const struct itimerspec * value, struct itimerspec * ovalue)
{
    // Only call passes ovalue = NULL, so don't need to do anything with it
    int retval = 0;
    int i = (int)timerid;
    pthread_mutex_lock(&NOS_timer_table_mutex);
    if ((i < 0) || (i >= OS_MAX_TIMEBASES)) {
        errno = EINVAL;
        retval = -1;
    } else {
        if ((value->it_value.tv_sec == 0) && (value->it_value.tv_nsec == 0)) {
            NOS_timer_table[i].armed = 0;
        } else {
            NOS_timer_table[i].armed = 1;
            NOS_timer_table[i].it_interval = value->it_interval;
            NOS_timer_table[i].expire_time = value->it_value.tv_sec * CFE_PSP_ticks_per_second + value->it_value.tv_nsec * CFE_PSP_ticks_per_second / NOS_NANO;
            if (flags == 0) { // relative time
                pthread_mutex_lock(&CFE_PSP_sim_time_mutex);
                NE_SimTime sim_time = CFE_PSP_sim_time;
                pthread_mutex_unlock(&CFE_PSP_sim_time_mutex);
                NOS_timer_table[i].expire_time += sim_time;
            }
        }
        retval = 0;
    }
    pthread_mutex_unlock(&NOS_timer_table_mutex);
    return retval;
}

void NOS_timer_fire(NE_SimTime time)
{
    pthread_mutex_lock(&NOS_timer_table_mutex);
    for (int i = 0; i < OS_MAX_TIMEBASES; i++) {
        if (NOS_timer_table[i].in_use && NOS_timer_table[i].armed) {
            if (NOS_timer_table[i].expire_time <= time) {
                // timer expired
                if ((NOS_timer_table[i].it_interval.tv_sec == 0) && (NOS_timer_table[i].it_interval.tv_nsec == 0)) {
                    NOS_timer_table[i].armed = 0; // one shot, disable it
                } else {
                    // increment to the next expiration time
                    NOS_timer_table[i].expire_time += NOS_timer_table[i].it_interval.tv_sec * CFE_PSP_ticks_per_second + 
                        NOS_timer_table[i].it_interval.tv_nsec * CFE_PSP_ticks_per_second / NOS_NANO;
                     // make sure the next expiration time is at least now, if so it will get triggered at the next tick
                    if (NOS_timer_table[i].expire_time < time) NOS_timer_table[i].expire_time = time;
                }
                // Use evp to do the timer action
                struct sigevent evp = NOS_timer_table[i].evp;
                if (evp.sigev_notify == SIGEV_SIGNAL) {
                    pthread_kill(OS_impl_timebase_table[evp.sigev_value.sival_int].handler_thread, evp.sigev_signo);
                } 
                //else {
                //    OS_printf("NOS_timer_fire:  timer %d, evp.sigev_notify %d not implemented\n", i, evp.sigev_notify);
                //}
            }
        }
    }
    pthread_mutex_unlock(&NOS_timer_table_mutex);
}

static void NOS_canonicalize_timespec(struct timespec *ts)
{
    while (ts->tv_nsec < 0) {
        ts->tv_nsec += 1000000000L;
        ts->tv_sec--;
    }
    while (ts->tv_nsec >= 1000000000L) {
        ts->tv_nsec -= 1000000000L;
        ts->tv_sec++;
    }
}

static void NOS_minus_real_timeoffset(struct timespec *offset)
{
    struct timespec nos, real;
    clock_gettime(CLOCK_REALTIME, &real);
    NOS_clock_gettime (CLOCK_REALTIME, &nos);
    offset->tv_sec = nos.tv_sec - real.tv_sec;
    offset->tv_nsec = nos.tv_nsec - real.tv_nsec;
    NOS_canonicalize_timespec(offset);
}

void NOS_to_real_timespec(const struct timespec *nos, struct timespec *real)
{
    struct timespec offset;
    NOS_minus_real_timeoffset(&offset);
    real->tv_sec = nos->tv_sec - offset.tv_sec;
    real->tv_nsec = nos->tv_nsec - offset.tv_nsec;
    NOS_canonicalize_timespec(real);
}