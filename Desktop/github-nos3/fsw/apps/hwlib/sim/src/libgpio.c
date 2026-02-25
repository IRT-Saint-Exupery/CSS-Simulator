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

//#include <cfe_psp.h>
#include "libgpio.h"
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t gpio_init(gpio_info_t* device) 
{    
    char buffer[128];
    int  write_size;
    int  fd;

    if (device->pin > 30-1) {
        printf("Please provide a pin # from 0 to %d\n", 30-1);
    }
    // Using a /tmp/ directory to avoid permission issues
    mkdir("/tmp/gpio-fake/", 0777);
    snprintf(buffer, 128, "/tmp/gpio-fake/gpio%d/", device->pin);
    mkdir(buffer, 0777);
    snprintf(buffer, 128, "/tmp/gpio-fake/gpio%d/direction", device->pin);

    // Set direction
    fd = open(buffer, O_WRONLY | O_CREAT, 0777);
    if (fd < 0) 
    {
        return GPIO_FD_OPEN_ERR;
    }

    if (device->direction == GPIO_INPUT)
    {
        snprintf(buffer, 128, "IN");
        write_size = 2;
    } else {
        snprintf(buffer, 128, "OUT");
        write_size = 3;
    }
    write(fd, buffer, write_size);
    close(fd);

    //snprintf(buffer, 128, "/tmp/gpio-fake/gpio%d/value", device->pin);
    //fd = open(buffer, O_WRONLY | O_CREAT);
    // Set open
    device->isOpen = GPIO_OPEN;
    gpio_write(device, 0x00);
    return GPIO_SUCCESS;
}

int32_t gpio_read(gpio_info_t* device, uint8_t* value)
{
    char buffer[128];
    int fd;

    snprintf(buffer, 128, "/tmp/gpio-fake/gpio%d/value", device->pin);
    fd = open(buffer, O_RDONLY | O_CREAT, 0777);
    if (fd < 0) 
    {
         return GPIO_FD_OPEN_ERR;
    }
    if (read(fd, value, 3) < 0) 
    {
        return GPIO_READ_ERR;
    }
    close(fd);
    if (*value == '1') {
        *value = 0x01;
    } else {
        *value = 0x00;
    }
    return GPIO_SUCCESS;
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

    snprintf(buffer, 128, "/tmp/gpio-fake/gpio%d/value", device->pin);
    fd = open(buffer, O_WRONLY | O_CREAT, 0777);
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

    
    if (device->isOpen == GPIO_OPEN)
    {
        device->isOpen = GPIO_CLOSED;
    }
    return GPIO_SUCCESS;
}

#ifdef __cplusplus
}
#endif
