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

#include "libcan.h"

/* The `libsocketcan` library wraps many of the netlink operations to control a SocketCAN interface
 * Documentation here: https://lalten.github.io/libsocketcan/Documentation/html/group__extern.html */

// Bring CAN network interface
int32_t can_init_dev(can_info_t* device) 
{
  int retVal;
  struct timeval tv;
  char devname[10];

  // Get device name from handle
  snprintf(devname, 10, "can%d", device->handle);

  retVal = can_set_bitrate(devname, device->bitrate);
  if (retVal < 0) 
  {
    OS_printf("Errno: %d \n", errno);
    OS_printf("\n");
    OS_printf("Try the following before running again: \n");
    OS_printf("  modprobe -r xilinx_can \n");
    OS_printf("  modprobe xilinx_can \n");
    OS_printf("\n");
    return CAN_SET_BITRATE_ERR;
  }

  /* Running this function on the zybo spits out "RTNETLINK answers: Operation not supported" */
  retVal = can_set_modes(device);
  if (retVal < 0) 
  {
    return CAN_SET_MODES_ERR;
  }

  /* TODO: Add option for filters */

  retVal = can_do_start(devname);
  if (retVal < 0) 
  {
    return CAN_UP_ERR;
  }

  device->sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (device->sock < 0) 
  { 
    return CAN_SOCK_OPEN_ERR; 
  }

  strcpy(device->ifr.ifr_name, devname);

  retVal = ioctl(device->sock, SIOCGIFINDEX, &device->ifr);
  if (retVal < 0) 
  { 
    return CAN_SOCK_FLAGSET_ERR; 
  }
  
  device->addr.can_family  = AF_CAN;
  device->addr.can_ifindex = device->ifr.ifr_ifindex;

  if (bind(device->sock, (struct sockaddr*)&device->addr, sizeof(device->addr)) < 0) 
  {
    return CAN_SOCK_BIND_ERR;
  }
  
  // Set timeouts
  tv.tv_sec = device->second_timeout;
  tv.tv_usec = device->microsecond_timeout;
  retVal = setsockopt(device->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  if(retVal < 0) 
  {
    return CAN_SOCK_SETOPT_ERR;
  }

  // Set to non-blocking
	fcntl(device->sock, F_SETFL, O_NONBLOCK);

  device->isUp = CAN_INTERFACE_UP;

  return CAN_SUCCESS;
}

// Call the `libsocketcan` function to set each CAN mode on/off
int32_t can_set_modes(can_info_t* device) 
{
  struct can_ctrlmode cm;
  memset(&cm, 0, sizeof(cm));
  char devname[10];

  // Get device name from handle
  snprintf(devname, 10, "can%d", device->handle);

  cm.mask = CAN_CTRLMODE_LOOPBACK     | // Remaining flags not supported
            //CAN_CTRLMODE_LISTENONLY     | 
            //CAN_CTRLMODE_3_SAMPLES      | 
            //CAN_CTRLMODE_ONE_SHOT       | 
            CAN_CTRLMODE_BERR_REPORTING;
            //CAN_CTRLMODE_FD             | 
            //CAN_CTRLMODE_PRESUME_ACK;

  cm.flags = (device->presumeAck << 6)     | 
             (device->fd << 5)             | 
             (device->berrReporting << 4)  | 
             (device->oneShot << 3)        | 
             (device->tripleSampling << 2) | 
             (device->listenOnly << 1)     | 
              device->loopback;

  int retVal = can_set_ctrlmode(devname, &cm);

  if (retVal < 0) 
  {
    return CAN_SET_MODES_ERR;
  }

  return CAN_SUCCESS;
}

// Write a can_frame  from `device->tx_Frame` to CAN bus from SocketCAN socket specified by `device`
int32_t can_write(can_info_t* device) 
{  
  int ret;

  // If can_id is > 11 bits, means it is extended frame format (EFF) and need to set bit 31 of the can_id field to signal EFF
  if (device->tx_frame.can_id > 0x7FF) 
  {
    device->tx_frame.can_id |= 1 << 31;
  }

  ret = write(device->sock, &device->tx_frame, sizeof(struct can_frame));
  if (ret < 0) 
  { 
    ret = CAN_WRITE_ERR; 
  }
  else
  {
    ret = CAN_SUCCESS;
  }

  #ifdef LIBCAN_VERBOSE
    printf("can_write ret = %d \n", ret);
  #endif

  return ret;
}

// Read a can_frame from SocketCAN interface specified by `device` into `device->rx_frame`
// Does a nonblocking read call
int32_t can_read(can_info_t* device) 
{		
  int ret;

  ret = read(device->sock, &device->rx_frame, sizeof(struct can_frame));
  if(ret < 0) 
  {
    if(errno == 11) 
    {
      ret = CAN_READ_TIMEOUT_ERR;
    }
    else 
    {
      ret = CAN_READ_ERR;
    }
  }

  if (ret == 16)
  {
    ret = CAN_SUCCESS;
  }

  #ifdef LIBCAN_VERBOSE
    OS_printf("can_read ret = %d \n", ret);
  #endif

  return ret;
}

// Bring CAN network interface down
int32_t can_close_device(can_info_t* device) 
{
  int retVal;
  char devname[10];

  // Get device name from handle
  snprintf(devname, 10, "can%d", device->handle);
  
  close(device->sock);

  retVal = can_do_stop(devname);
  if (retVal < 0) 
  {
    return CAN_DOWN_ERR;
  }
  device->isUp = CAN_INTERFACE_DOWN;

  return CAN_SUCCESS;
}

// Perform non-blocking can transaction
int32_t can_master_transaction(can_info_t* device) 
{
  int32_t status;
  uint8_t i;

  status = can_write(device);
  if (status != CAN_SUCCESS)
  {
      return status;
  }

  usleep(device->xfer_us_delay);

  status = can_read(device);
  if (status != CAN_SUCCESS)
  {
      return status;
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

  return status;
}
