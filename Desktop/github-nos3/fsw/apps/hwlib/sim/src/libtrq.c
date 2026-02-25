/* Copyright (C) 2009 - 2020 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

/* nos */
#include <Spi/Client/CInterface.h>

/* hwlib API */
#include "libtrq.h"

static int num_conn_errors = 0;
static int num_send_errors = 0;
static const int PORT = 14242;
static int sockfd = 0;
static struct sockaddr_in servaddr;

int32_t trq_update(trq_info_t* device)
{
    int32_t status = TRQ_SUCCESS;
    ssize_t bytes_sent;
    char message[512];
    float percent_high_dir = 100.0 * device->timer_high_ns / device->timer_period_ns;

    // Take into account the direction
    if (device->positive_direction == false)
    {
        percent_high_dir = percent_high_dir * -1;
    }

    // Send to MTB sim, MTB sim must then calculate A-m^2
    sprintf(message, "%d %f\n", device->trq_num, percent_high_dir);

    if (sockfd >= 0) 
    {
        bytes_sent = sendto(sockfd, message, strlen(message), MSG_CONFIRM | MSG_DONTWAIT, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        if ((bytes_sent < 0) || ((size_t)bytes_sent != strlen(message))) 
        {
            if (num_send_errors++ < 10) 
            { // don't spam
                printf("NOS command_torquer:  Only sent %d bytes of %d bytes.  Message was:  %s\n", bytes_sent, strlen(message), message);
            }
            status = TRQ_ERROR;
        }
    } else 
    {
        if (num_conn_errors++ < 10) 
        { // don't spam
            printf("NOS command_torquer:  Socket not connected (%d).\n", sockfd);
        }
        status = TRQ_ERROR;
    }

    return status;
}

int32_t trq_set_time_high(trq_info_t* device, uint32_t new_time)
{
    int32_t status = TRQ_SUCCESS;

    device->timer_high_ns = new_time;
    status = trq_update(device);

    return status;
}

int32_t trq_set_period(trq_info_t* device)
{
    int32_t status = TRQ_SUCCESS;
    
    status = trq_update(device);

    return status;
}

int32_t trq_set_direction(trq_info_t* device, bool direction)
{
    int32_t status = TRQ_SUCCESS;
    
    device->positive_direction = direction;
    status = trq_update(device);

    return status;
}

int32_t trq_init(trq_info_t* device)
{
    int32_t status = TRQ_SUCCESS;
    device->enabled = true;
    device->timer_high_ns = 0;  // no pulse

    // int socket(int domain, int type, int protocol);
    // int close(int fd)

    if (sockfd == 0) {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            OS_printf("NOS trq_init:  Failed to create UDP socket\n");
        }
    
        memset(&servaddr, 0, sizeof(servaddr));
        // Filling server information 
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        servaddr.sin_addr.s_addr = INADDR_ANY;
    }

    return status;
}

int32_t trq_command(trq_info_t* device, uint8_t percent_high, bool pos_dir)
{
    int32_t status = TRQ_SUCCESS;

    // Calculate time high
    if (percent_high > 100)
    {
        printf("trq_command: Error setting percent high greater than 100! \n");
        return TRQ_ERROR;
    }

    device->timer_high_ns = device->timer_period_ns * (percent_high / 100.00);
    device->positive_direction = pos_dir;
    status = trq_update(device);

    return status;
}

void trq_close(trq_info_t* device)
{
    device->enabled = false;
    close(sockfd);
}
