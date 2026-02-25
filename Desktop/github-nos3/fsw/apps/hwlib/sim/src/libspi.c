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
#include <pthread.h>

/* nos */
#include <Spi/Client/CInterface.h>

/* hwlib API */
#include "libspi.h"

/* spi bus mutex */
pthread_mutex_t spi_bus_mutex[MAX_SPI_BUSES];
uint32_t handle_count = 0;

/* spi device handles */
static NE_SpiHandle *spi_device[NUM_SPI_DEVICES] = {0};

/* public prototypes */
void nos_init_spi_link(void);
void nos_destroy_spi_link(void);

/* private prototypes */
static NE_SpiHandle* nos_get_spi_device(spi_info_t* device);

/* initialize nos engine spi link */
void nos_init_spi_link(void)
{
    // Init the mutexes for chip select
    int i;
    for(i = 0; i < MAX_SPI_BUSES; i++)
    {
        if (pthread_mutex_init(&spi_bus_mutex[i], NULL) != 0)
        {
            OS_printf("HWLIB: Create spi mutex error for spi bus %d", i);
        }
    }
}

/* destroy nos engine spi link */
void nos_destroy_spi_link(void)
{
    /* clean up spi buses */
    int i;
    for(i = 0; i < MAX_SPI_BUSES; i++)
    {
        NE_SpiHandle *dev = spi_device[i];
        if(dev) NE_spi_close(&dev);

        if (pthread_mutex_destroy(&spi_bus_mutex[i]) != 0)
        {
            OS_printf("HWLIB: Destroy spi mutex error for spi bus %d", i);
        }
    }
}

/* nos spi init */
int32_t spi_init_dev(spi_info_t* device)
{
    int     status = SPI_SUCCESS;
    char    buffer[16];
    
    pthread_mutex_lock(&spi_bus_mutex[device->bus]);
    
    /* get spi device handle */
    NE_SpiHandle **dev = &spi_device[device->handle];
    if(*dev == NULL)
    {
        /* get nos spi connection params */
        const nos_connection_t *con = &nos_spi_connection[(device->bus * 10) + device->cs];

        /* try to initialize master */
        *dev = NE_spi_init_master3(hub, con->uri, con->bus);
        if(*dev)
        {
            status = SPI_SUCCESS;
        }
        else
        {
            pthread_mutex_unlock(&spi_bus_mutex[device->bus]);
            OS_printf("HWLIB: Open SPI device \"%s\" error %d", device->deviceString, status);
            return status;
        }
    }

    pthread_mutex_unlock(&spi_bus_mutex[device->bus]);

    // Set open flag
    device->isOpen = SPI_DEVICE_OPEN;

    return status;
}

/* get spi device */
static NE_SpiHandle* nos_get_spi_device(spi_info_t* device)
{
    NE_SpiHandle *dev = NULL;
    if(device->handle < NUM_SPI_DEVICES)
    {
        dev = spi_device[device->handle];
        if(dev == NULL)
        {
            spi_init_dev(device);
            dev = spi_device[device->handle];
        }
    }
    return dev;
}

/* nos spi chip select */
int32_t spi_select_chip(spi_info_t* device)
{
    int32_t status = SPI_SUCCESS;

    pthread_mutex_lock(&spi_bus_mutex[device->bus]);

    NE_SpiHandle *dev = nos_get_spi_device(device);
    if(dev)
    {
        NE_spi_select_chip(dev, device->cs);
    }

    return status;
}

/* nos spi chip unselect */
int32_t spi_unselect_chip(spi_info_t* device)
{
    int32_t status = SPI_SUCCESS;

    pthread_mutex_unlock(&spi_bus_mutex[device->bus]);

    NE_SpiHandle *dev = nos_get_spi_device(device);
    if(dev)
    {
        NE_spi_unselect_chip(dev);
    }

    return status;
}

/* nos spi write */
int32_t spi_write(spi_info_t* device, uint8_t data[], const uint32_t numBytes)
{
    int status = SPI_SUCCESS;

    NE_SpiHandle *dev = nos_get_spi_device(device);
    if(dev)
    {
        if(NE_spi_write(dev, data, numBytes) != NE_SPI_SUCCESS)
        {
            status = SPI_ERROR;
        }
    }

    return status;
}

/* nos spi read */
int32_t spi_read(spi_info_t* device, uint8_t data[], const uint32_t numBytes)
{
    int status = SPI_SUCCESS;

    NE_SpiHandle *dev = nos_get_spi_device(device);
    if(dev)
    {
        if(NE_spi_read(dev, data, numBytes) != NE_SPI_SUCCESS)
        {
            status = SPI_ERROR;
        }
    }

    return status;
}

int32_t spi_transaction(spi_info_t* device, uint8_t *txBuff, uint8_t * rxBuffer, uint32_t length, uint16_t delay, uint8_t bits, uint8_t deselect)
{
    int status = SPI_SUCCESS;

    NE_SpiHandle *dev = nos_get_spi_device(device);
    if(dev)
    {
        if(NE_spi_transaction(dev, txBuff, length, rxBuffer, length) != NE_SPI_SUCCESS)
        {
            status = SPI_ERROR;
        }
    }

    return status;
}

int32_t spi_close_device(spi_info_t* device)
{
	if (device->handle >= 0)
    {
        NE_SpiHandle *dev = nos_get_spi_device(device);
        if(dev)
        {
            NE_spi_close(&dev);
            spi_device[device->handle] = 0;
            device-> isOpen = SPI_DEVICE_CLOSED;
        }
    }
    return OS_SUCCESS;
}
