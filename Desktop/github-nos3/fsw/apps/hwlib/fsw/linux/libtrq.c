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

#include <fcntl.h>
#include "libtrq.h"
#include "libtrq_ioctl.h"

#define TRQ_FNAME_SIZE 50

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * trq_set_time_high(): Configure the time high per period in nanoseconds for a TRQ device. Time high lengths
 *                      may not exceed a device's period length. 
 *
 * Inputs:              trq_info_t *device      -   TRQ device info structure 
 *                      uint32_t    new_time    -   New time high length for the device period in nanoseconds
 *
 * Outputs:             trq_info_t *device      -   High time set to new_time if successful
 *                      returns int32_t         -   TRQ_ERROR_* type on failure, TRQ_SUCCESS on success
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t trq_set_time_high(trq_info_t* device, uint32_t new_time) 
{
    if(!device->enabled)
    {
        OS_printf("trq_set_time_high: Error setting trq %d timer period because it's disabled! \n", device->trq_num); 
        return TRQ_ERROR; 
    }

	// Make sure the time high isn't greater than the period
	if(new_time > device->timer_period_ns) 
    {
		OS_printf("trq_set_time_high: Error setting trq %d time high, must not exceed the period! \n", device->trq_num);
		return TRQ_TIME_HIGH_VAL_ERR;
	}

    ioctl(device->timerfd, TMRCTR_PWM_DISABLE); 
    if(ioctl(device->timerfd, TMRCTR_PWM_SET_HIGH_TIME, new_time) < 0)
    {
        OS_printf("trq_set_time_high: Error setting trq %d high time! \n", device->trq_num); 
        return TRQ_ERROR; 
    }
	device->timer_high_ns = new_time;

    if(device->timer_period_ns && device->timer_high_ns)
    {
        ioctl(device->timerfd, TMRCTR_PWM_ENABLE);
    }

    //OS_printf("trq_set_time_high: timer_period_ns = %d, timer_high_ns = %d\n", device->timer_period_ns, device->timer_high_ns); 
    return TRQ_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * trq_set_period():    Configure the period length for a TRQ device in nanoseconds. Note that timer time high 
 *                      is set to zero before this function is called. 
 *
 * Inputs:              trq_info_t *device      -   TRQ device info structure 
 *
 * Outputs:             returns int32_t         -   TRQ_ERROR on failure, TRQ_SUCCESS on success
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t trq_set_period(trq_info_t* device) 
{
    if(!device->enabled)
    {
        OS_printf("trq_set_period: Error setting trq %d timer period because it's disabled! \n", device->trq_num); 
        return TRQ_ERROR; 
    }

    // Make sure timer is disabled first
    ioctl(device->timerfd, TMRCTR_PWM_DISABLE); 

    if(ioctl(device->timerfd, TMRCTR_PWM_SET_PERIOD, device->timer_period_ns) < 0)
    {
        OS_printf("trq_set_period: Error setting trq %d timer period! \n", device->trq_num); 
        return TRQ_ERROR; 
    }

    // Enable timer
    ioctl(device->timerfd, TMRCTR_PWM_ENABLE); 

    return TRQ_SUCCESS;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * trq_set_direction():    Configure the direction of the TRQ device. 
 *
 * Inputs:              trq_info_t *device      -   TRQ device info structure 
 *                      bool        direction   -   New direction desired for device
 *
 * Outputs:             trq_info_t *device      -   positive_direction set to direction if successful
 *                      returns int32_t         -   TRQ_ERROR on failure, TRQ_SUCCESS on success
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t trq_set_direction(trq_info_t* device, bool direction) 
{
    char charVal;
    
    if (direction == TRQ_DIR_NEGATIVE) 
    {
        charVal = '1'; // Inverted due to hardware
    }
    else 
    {
        charVal = '0';
    }

    if (write(device->direction_pin_fd, &charVal, 1) != 1) 
    {
        OS_printf("trq_set_direction: Error setting trq %d direction! \n", device->trq_num);
        return TRQ_ERROR;
    }
    device->positive_direction = direction;

    return TRQ_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * trq_init():          Opens a TRQ device and initializes it. This function can also be
 *                      used on an already opened TRQ number to reset it. 
 *
 * Inputs:              trq_info_t *device  -   TRQ Device info structure to initialize   
 * 
 * Outputs:             trq_info_t *device  -   Info structure contains AXI timer device file descriptor. 
 *                      returns int32_t     -   <0 on failure, TRQ_SUCCESS on success
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t trq_init(trq_info_t* device) 
{
    int32_t status = TRQ_SUCCESS;
    char devname[TRQ_FNAME_SIZE];
    char pinname[TRQ_FNAME_SIZE]; 

    if(!device->enabled)
    {
        // Open axi timer
        snprintf(devname, TRQ_FNAME_SIZE, "/dev/tmrctr%d", device->trq_num); 
        if((device->timerfd = open(devname, O_RDWR, 0)) < 0)
        {
            OS_printf("trq_init: Error opening axi timer device %s \n", devname); 
            return TRQ_INIT_ERR; 
        } 
        //printf("trq_init: Initialized AXI Timer Device: %s for device->trq_num %d \n", devname, device->trq_num);

        // Open direction pin
        snprintf(devname, TRQ_FNAME_SIZE, "/dev/hb%d", device->trq_num); 
        if((device->direction_pin_fd = open(devname, O_RDWR, 0)) < 0)
        {
            OS_printf("trq_init: Error opening axi timer device %s \n", devname); 
            return TRQ_INIT_ERR; 
        }

        // Set direction
        status = trq_set_direction(device, TRQ_DIR_POSITIVE);
        if(status != TRQ_SUCCESS)
        {
            return TRQ_INIT_ERR;
        }
        //OS_printf("trq_init: Initialized direction pin: %s for device->trq_num %d\n", pinname, device->trq_num);

        device->enabled = true;
    }

    // Set timer time high to zero
    status = trq_set_time_high(device, 0);
    if(status != TRQ_SUCCESS)
    {
        return TRQ_INIT_ERR;
    }
    device->timer_high_ns = 0;  // no pulse

    // Set timer period. Expects timer_period_ns to be set by app. 
    status = trq_set_period(device);
    if(status != TRQ_SUCCESS)
    {
        return TRQ_INIT_ERR;
    }

    return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * trq_command():   Change TQR period, time high, or direction.
 *
 * Inputs:              trq_info_t* device      -   TQR device info structure to modify
 *                      uint8_t percent_high    -   Percent of the period to be high (0-100)
 *                      bool pos_dir            -   Direction - True for positive, False for negative
 *
 * Outputs:             trq_info_t *device      -   Parameters set to new values if successful
 *                      returns int32_t         -   TRQ_ERROR_* type on failure, TRQ_SUCCESS on success
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t trq_command(trq_info_t *device, uint8_t percent_high, bool pos_dir)
{
    int32_t status = TRQ_SUCCESS;
    uint32_t time_high_ns;
    char charVal;

    if(!device->enabled)
    {
        status = TRQ_ERROR; 
        return status;
    }
    
    // Calculate time high
    if (percent_high > 100)
    {
        OS_printf("trq_command: Error setting percent high greater than 100! \n");
        return TRQ_ERROR;
    }
    time_high_ns = device->timer_period_ns * (percent_high / 100.00);
    
    // Change time high? 
    if(device->timer_high_ns != time_high_ns)
    {
        status=trq_set_time_high(device, time_high_ns);
        if(status != TRQ_SUCCESS)
        {
            status = TRQ_ERROR; 
            return status;
        }
        device->timer_high_ns = time_high_ns;
    }

    // Change direction?
    if(device->positive_direction != pos_dir)
    {
        // Set direction
        status = trq_set_direction(device, pos_dir);
        if(status != TRQ_SUCCESS)
        {
            OS_printf("trq_command: Error setting trq %d direction! \n", device->trq_num);
            status = TRQ_INIT_ERR;
            return status;
        }
    }

    return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 * trq_close():         Disables and closes an active TRQ device. 
 * 
 * Inputs:              trq_info_t *device - TRQ device info structure for TRQ device to disable
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void trq_close(trq_info_t* device)
{
    if(!device->enabled)
    {
        OS_printf("trq_close: Error closing trq %d, already disabled! \n", device->trq_num); 
        return; 
    }

    close(device->timerfd); 
    close(device->direction_pin_fd);
    device->enabled = false; 
}
