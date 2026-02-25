/* Copyright (C) 2009 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include "nos_link.h"
#include <stdint.h>
#include <stdlib.h>

/* nos */
#include <Can/Client/CInterface.h>

#include "libcan.h"

#define CAN_BASE_CMD_LEN  8

/* can device handles */
static NE_CanHandle *can_device[NUM_CAN_DEVICES] = {0};

/* get spi device */
static NE_CanHandle* nos_get_can_device(can_info_t* device)
{
    NE_CanHandle *dev = NULL;
    
    dev = can_device[device->handle];
    if(dev == NULL)
    {
        can_init_dev(device);
        dev = can_device[device->handle];
    }
    
    return dev;
}

/* destroy nos engine can link */
void nos_destroy_can_link(void)
{
    /* clean up can buses */
    int i;
    for (i = 0; i < NUM_CAN_DEVICES; i++)
    {
        NE_CanHandle *dev = can_device[i];
        if (dev) 
            NE_can_close(&dev);
    }
}

// Bring CAN network interface
int32_t can_init_dev(can_info_t* device)
{
    int32_t result = OS_SUCCESS;
    NE_CanHandle **dev;
    const nos_connection_t *con;

    dev = &can_device[device->handle];
    if (*dev == NULL)
    {
        /* get nos can connection params */
        con = &nos_can_connection[device->handle];
    }

    /* try to initialize master */
    *dev = NE_can_init_master3(hub, 10, con->uri, con->bus);
    device->isUp = CAN_INTERFACE_UP;
    if (*dev == NULL)
    {
        result = OS_ERROR;
        OS_printf("LIBCAN: %s:  FAILED TO INITIALIZE NOS CAN MASTER\n", __func__);
        device->isUp = CAN_INTERFACE_DOWN;
    }
    return result;        
}

// TODO: NOT IMPLEMENTED!
int32_t can_set_modes(can_info_t* device) 
{
	return CAN_SUCCESS;
}

// Write a can_frame  from `device->tx_Frame` to CAN bus from SocketCAN socket specified by `device`
int32_t can_write(can_info_t* device)
{
    return can_master_transaction(device);
}

// Read a can_frame from SocketCAN interface specified by `device` into `device->rx_frame`
int32_t can_read(can_info_t* device)
{
    return can_master_transaction(device);
}

int32_t can_master_transaction(can_info_t* device)
{
    int result = CAN_ERROR;
    int i;
    
    /* get can device handle */
    NE_CanHandle *dev = nos_get_can_device(device);

    /* can transaction */
    if(dev)
    {
        result = NE_can_transaction(dev, device->tx_frame.can_id,
                                    (uint8_t*) &device->tx_frame, device->tx_frame.can_dlc + CAN_BASE_CMD_LEN, 
                                    (uint8_t*) &device->rx_frame, CAN_BASE_CMD_LEN + CAN_MAX_DLEN);
    }

    #ifdef LIBCAN_VERBOSE
        OS_printf("can_master_transaction: \n");
        OS_printf("  can_id = 0x%08x \t tx: 0x", device->tx_frame.can_id);
        for (i = 0; i < device->tx_frame.can_dlc; i++)
        {
        OS_printf("%02x ", device->tx_frame.data[i]);
        }
        OS_printf("\n");
        OS_printf("  can_id = 0x%08x \t rx: 0x", device->rx_frame.can_id);
        for (i = 0; i < device->rx_frame.can_dlc; i++)
        {
        OS_printf("%02x ", device->rx_frame.data[i]);
        }
        OS_printf("\n");
    #endif

    return result;
}

// Bring CAN network interface down
int32_t can_close_device(can_info_t* device)
{
    /* clean up can device */
    NE_CanHandle *dev = can_device[device->handle];
    if(dev) 
    {
        NE_can_close(&dev);
        can_device[device->handle] = 0;
        device->isUp = CAN_INTERFACE_DOWN;
    }
    return NE_CAN_SUCCESS;
}