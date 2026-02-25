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
#include <I2C/Client/CInterface.h>

/* hwlib API */
#include "libi2c.h"

/* i2c device handles */
static NE_I2CHandle *i2c_device[NUM_I2C_DEVICES] = {0};

/* destroy nos engine i2c link */
void nos_destroy_i2c_link(void)
{
    /* clean up i2c buses */
    int32_t i;
    for(i = 0; i < NUM_I2C_DEVICES; i++)
    {
        NE_I2CHandle *dev = i2c_device[i];
        if(dev) NE_i2c_close(&dev);
    }

}

static NE_I2CHandle* nos_get_i2c_device(int handle)
{
    NE_I2CHandle *dev = NULL;
    if(handle < NUM_I2C_DEVICES)
    {
        dev = i2c_device[handle];
    }
    return dev;
}

int32_t i2c_master_init(i2c_bus_info_t* device)
{
    int32_t status = I2C_SUCCESS;
    if(device->handle >= 0 && device->handle < NUM_I2C_DEVICES)
    {
        /* get i2c device handle */
        NE_I2CHandle **dev = &i2c_device[device->handle];
        if(*dev == NULL)
        {
            /* get nos i2c connection params */
            const nos_connection_t *con = &nos_i2c_connection[device->handle];

            /* try to initialize master */
            *dev = NE_i2c_init_master3(hub, 10, con->uri, con->bus); // the value 10 is used in the NOS3 case to indicate the master
            if(*dev == NULL)
            {
                OS_printf("nos i2c_init_master failed\n");
                device->isOpen = I2C_CLOSED;
                status = I2C_ERROR;
            }
            else
            {
                device->isOpen = I2C_OPEN;
            }
        }
    }
    else
    {
        OS_printf("i2c_init_master: Handle not found\n");
        device->isOpen = I2C_CLOSED;
        status = I2C_ERROR;
    }
    return status;
}

/* nos i2c transaction */
int32_t i2c_master_transaction(i2c_bus_info_t* device, uint8_t addr, void * txbuf, uint8_t txlen,
                               void * rxbuf, uint8_t rxlen, uint16_t timeout)
{
    int32_t result = I2C_ERROR;

    NE_I2CHandle *dev = nos_get_i2c_device((int)device->handle);

    /* i2c transaction */
    if(dev)
    {
        if ((txlen == 0) && (rxlen == 0)) { // force success if both buffer lengths are 0
            result = I2C_SUCCESS;
        } else if(NE_i2c_transaction(dev, addr, txbuf, txlen, rxbuf, rxlen) == NE_I2C_SUCCESS)
        {
            result = I2C_SUCCESS;
        }
    }

    return result;
}

int32_t i2c_multiple_transaction(i2c_bus_info_t* device, uint8_t addr, struct i2c_rdwr_ioctl_data* rdwr_data, uint16_t timeout)
{
    int32_t result = I2C_ERROR;
    uint32_t i;

    for (i = 0; i < rdwr_data->nmsgs; i++)
    {
        if (rdwr_data->msgs[i].flags == 0)
        {   // Write
            result = i2c_master_transaction(device, addr, (void*) rdwr_data->msgs[i].buf, (uint8_t) rdwr_data->msgs[i].len, (void*) NULL, 0, timeout);
        }
        else
        {   // Read
            result = i2c_master_transaction(device, addr, (void*) NULL, 0, (void*) rdwr_data->msgs[i].buf, (uint8_t) rdwr_data->msgs[i].len, timeout);
        }
        
        if (result != I2C_SUCCESS)
        {
            break;
        }
    }

    return result;
}

int32_t i2c_master_close(i2c_bus_info_t* device) 
{
    if (device->handle >= 0)
    {
        NE_I2CHandle *dev = nos_get_i2c_device((int)device->handle);
        if(dev)
        {
            NE_i2c_close(&dev);
            i2c_device[device->handle] = 0;
            device->isOpen = I2C_CLOSED;
        }
    }
    return I2C_SUCCESS;
}
