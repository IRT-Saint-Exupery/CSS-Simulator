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

#include "libgpio.h"

int32_t gpio_init(gpio_info_t* device) 
{    
    char buffer[128];
    int  bytes_written;
    int  write_size;
    int  fd;

    snprintf(buffer, 128, "/sys/class/gpio/gpio%d/direction", device->pin);

    // Check if pin already exported
    if (access(buffer, W_OK) < 0)
    {   // File does not exist
        fd = open("/sys/class/gpio/export", O_WRONLY);
        if (fd < 0) 
        {
            return GPIO_FD_OPEN_ERR;
        }
        write_size = snprintf(buffer, 128, "%d", device->pin);
        bytes_written = write(fd, buffer, write_size);
        if (bytes_written < write_size) 
        {
            return GPIO_WRITE_ERR;
        }
        close(fd);
    }

    // Set direction
    snprintf(buffer, 128, "/sys/class/gpio/gpio%d/direction", device->pin);
    fd = open(buffer, O_WRONLY);
    if (fd < 0) 
    {
        return GPIO_FD_OPEN_ERR;
    }

    if (device->direction == GPIO_INPUT)
    {
        snprintf(buffer, 128, "IN");
        bytes_written = write(fd, buffer, 4);
    }
    else
    {
        snprintf(buffer, 128, "OUT");
        bytes_written = write(fd, buffer, 4);
    }
    close(fd);

    // Set open
    device->isOpen = GPIO_OPEN;
    return GPIO_SUCCESS;
}

int32_t gpio_read(gpio_info_t* device, uint8_t* value)
{
    int32_t status = GPIO_SUCCESS;
    char buffer[128];
    char readValue;
    int fd;

    if(device->isOpen != GPIO_OPEN) 
    {
        status = GPIO_READ_ERR;
        return status;
    }

    snprintf(buffer, 128, "/sys/class/gpio/gpio%d/value", device->pin);
    fd = open(buffer, O_RDONLY);
    if (fd < 0) 
    {
        status = GPIO_FD_OPEN_ERR;
        return status;
    }
    if (read(fd, &readValue, 3) < 0) 
    {
        status = GPIO_READ_ERR;
        return status;
    }

    //convert from ASCII to decimal
    if(readValue == '0') *value = 0;
    else if(readValue == '1') *value = 1;
    else {
        *value = 0;
    }

    close(fd);
    return status;
}

int32_t gpio_write(gpio_info_t* device, uint8_t value)
{
    char buffer[128];
    char charVal;
    int fd;

    if (value == 1) 
    {
        charVal = '1';
    }
    else 
    {
        charVal = '0';
    }

    snprintf(buffer, 128, "/sys/class/gpio/gpio%d/value", device->pin);
    fd = open(buffer, O_WRONLY);
    if (fd < 0) 
    {
         return GPIO_FD_OPEN_ERR;
    }

    if (write(fd, &charVal, 1) != 1) 
    {
        return GPIO_WRITE_ERR;
    }

    close(fd);
    return GPIO_SUCCESS;
}

int32_t gpio_close(gpio_info_t* device)
{
    char buffer[128];
    int  bytes_written;
    int  write_size;
    int  fd;
    
    if (device->isOpen == GPIO_OPEN)
    {   // Unexport pin
        fd = open("/sys/class/gpio/unexport", O_WRONLY);
        if (fd < 0) 
        {
            return GPIO_FD_OPEN_ERR;
        }
        write_size = snprintf(buffer, 128, "%d", device->pin);
        bytes_written = write(fd, buffer, write_size);
        if (bytes_written < write_size) 
        {
            return GPIO_WRITE_ERR;
        }
        close(fd);
        device->isOpen = GPIO_CLOSED;
    }
    return GPIO_SUCCESS;
}
