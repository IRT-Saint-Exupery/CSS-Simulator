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
#include <Uart/Client/CInterface.h>

/* hwlib API */
#include "libuart.h"

/* size of uart buffer */
#define USART_RX_BUF_SIZE    4096

/* usart device handles */
static NE_Uart *usart_device[NUM_USARTS] = {0};

/* public prototypes */
void nos_destroy_usart_link(void);

/* private prototypes */
static NE_Uart* nos_get_usart_device(int handle);

/* destroy nos engine usart link */
void nos_destroy_usart_link(void)
{
    int i;

    /* clean up usart buses */
    for(i = 0; i <= NUM_USARTS; i++)
    {
        NE_Uart *dev = usart_device[i];
        if(dev) NE_uart_close(&dev);
    }
}

/* init usart */
int32_t uart_init_port(uart_info_t* device)
{
    int32_t status = OS_SUCCESS;
    if(device->handle >= 0 && device->handle < NUM_USARTS)
    {

        /* get usart device handle */
        NE_Uart **dev = &usart_device[device->handle];
        if(*dev == NULL)
        {
            /* get nos usart connection params */
            const nos_connection_t *con = &nos_usart_connection[device->handle];

            /* try to initialize usart */
            *dev = NE_uart_open3(hub, "fsw", con->uri, con->bus, device->handle);

            if(*dev)
            {
                /* set default queue size */
                NE_uart_set_queue_size(*dev, USART_RX_BUF_SIZE);

                device->isOpen = PORT_OPEN;           
	        }
            else
            {
                OS_printf("nos uart_open failed\n");
		        device->isOpen = PORT_CLOSED;
		        status = OS_ERR_FILE;
            }
        }
    }
    else
    {
        OS_printf("Handle not found\n");
        device->isOpen = PORT_CLOSED;
        status = OS_ERR_FILE;
    }
    return status;
}

/* get usart device */
static NE_Uart* nos_get_usart_device(int handle)
{
    NE_Uart *dev = NULL;
    if(handle < NUM_USARTS)
    {
        dev = usart_device[handle];
    }
    return dev;
}

/* usart flush */
int32_t uart_flush(uart_info_t* device)
{
    NE_Uart *dev = nos_get_usart_device((int)device->handle);
    if(dev)
    {
        NE_uart_flush(dev);
    }
    return UART_SUCCESS;
}

/* usart write */
int32_t uart_write_port(uart_info_t* device, uint8_t data[], const uint32_t numBytes)
{
    int32_t status = OS_ERR_FILE;
    NE_Uart *dev = nos_get_usart_device((int)device->handle);
    if(dev)
    {
        status = NE_uart_write(dev, (const uint8_t*)data, numBytes); //Can this function return -1?
    }
    return status;
}

/* usart read */
int32_t uart_read_port(uart_info_t* device, uint8_t data[], const uint32_t numBytes)
{
    uint32_t status = OS_ERR_FILE;

    if (data != NULL) //Check that there is actually data to read
    { 
        uint8_t c = 0xFF;
        int  i;
        int stat;
        NE_Uart *dev = nos_get_usart_device((int)device->handle);
        if(dev)
        {
            for (i = 0; i < (int)numBytes; i++) //TODO: Add ability to switch between blocking and non-blocking?
            {
                /*
                //NON BLOCKING MODE
                stat = NE_uart_getc(dev, (uint8_t*)&c); //Returns 0 if byte read, 1 if no byte actually read
                if(stat == 1)
                {
                    return i; //Causes app to immediately enter service mode
                }
                else {
                    data[i] = c;
                }
                */
                //BLOCKING MODE
                do {
                    stat = NE_uart_getc(dev, (uint8_t*)&c);
                    if (stat == 1)
                    {
                        OS_TaskDelay(1);
                    }
                } while(stat); 
                data[i] = c;
            }
            status = numBytes;
            
            return status;
        }
        return status; //There is data, but can't read from device
    }
    return status; //Following arm_inux model
}

/* usart number bytes available */
int32_t uart_bytes_available(uart_info_t* device)
{
    int bytes = 0;
    NE_Uart *dev = nos_get_usart_device((int)device->handle);
    if(dev)
    {
        bytes = (int)NE_uart_available(dev);
    }
    return bytes;
}

int32_t uart_close_port(uart_info_t* device) 
{
    NE_UartStatus status;
    NE_Uart *dev = nos_get_usart_device((int)device->handle);
    if (device->handle >= 0)
    {
        if(dev)
        {
            status = NE_uart_close(&dev);
            usart_device[device->handle] = 0;
            device->isOpen = PORT_CLOSED;
        }
    }
    if (status == NE_UART_SUCCESS) {
        return OS_SUCCESS;
    }
    else
    {
        return OS_ERROR;
    }
    
}