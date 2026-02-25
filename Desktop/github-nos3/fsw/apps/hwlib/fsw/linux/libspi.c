/* Copyright (C) 2009 - 2018 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include "osapi.h"
#include "libspi.h"

spi_mutex_t spi_bus_mutex[MAX_SPI_BUSES];

int32_t spi_init_dev(spi_info_t* device)
{
  int32_t status = SPI_SUCCESS;
  char    buffer[16];

  // Initialize the bus mutex
  if (device->bus < MAX_SPI_BUSES)
  {
    if (spi_bus_mutex[device->bus].users == 0)
    {
      snprintf(buffer, 16, "spi_%d_mutex", device->bus);
      status = OS_MutSemCreate(&spi_bus_mutex[device->bus].spi_mutex, buffer, 0);
      if (status != OS_SUCCESS)
      {
        OS_printf("HWLIB: Create spi mutex error %d", status);
        return status;
      }
    }
    spi_bus_mutex[device->bus].users++;
  }
  else
  {
    OS_printf("HWLIB: Create spi mutex error %d, bus invalid!", status);
    return status;
  }

  if (OS_MutSemTake(spi_bus_mutex[device->bus].spi_mutex) == OS_SUCCESS)
  {
    // Open the device
    device->handle = open(device->deviceString, O_RDWR, 0); 
    if (device->handle < 0)
    {
      device->isOpen = SPI_DEVICE_CLOSED;
      status = SPI_ERR_FILE_OPEN;
      OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);
      OS_printf("HWLIB: Open SPI device \"%s\" error %d", device->deviceString, status);
      return status;
    }
  }
  OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);

  // Set the mode 
  status = spi_set_mode(device);
  if(status != SPI_SUCCESS)
  {
    return status;
  }

  // Set open flag
  device->isOpen = SPI_DEVICE_OPEN;

  return status;
}

int32_t spi_set_mode(spi_info_t* device)
{
  uint8_t mode; 
  int32_t ret;
  int32_t status = SPI_SUCCESS; 

  switch(device->spi_mode)
  {
    case 0:
      mode = SPI_MODE_0;
      break;
    case 1:
      mode = SPI_MODE_1;
      break;
    case 2:
      mode = SPI_MODE_2;
      break;
    case 3:
      mode = SPI_MODE_3;
      break;
    default: 
      status = SPI_ERR_INVAL_MD;
      OS_printf("HWLIB: Invalid spi mode error %d", status);
      return status;
  }

  if (OS_MutSemTake(spi_bus_mutex[device->bus].spi_mutex) == OS_SUCCESS)
  {
    // Set mode
    ret = ioctl(device->handle, SPI_IOC_WR_MODE, &mode);
    if (ret != SPI_SUCCESS)
    {
      status = SPI_ERR_WR_MODE;
      OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);
      return status;
    }  
    // Set bits per word
    ret = ioctl(device->handle, SPI_IOC_WR_BITS_PER_WORD, &(device->bits_per_word));
    if (ret != SPI_SUCCESS)
    {
      status = SPI_ERR_WR_BPW;
      OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);
      return status;
    }
    // Set max speed
    ret = ioctl(device->handle, SPI_IOC_WR_MAX_SPEED_HZ, &(device->baudrate));
    if (ret != SPI_SUCCESS)
    {
      status = SPI_ERR_WR_SD_HZ;
      OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);
      return status;
    }
  }
  OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);
  
  return status;
}

int32_t spi_get_mode(spi_info_t* device)
{
  int32_t  ret;
  int32_t  status = SPI_SUCCESS; 
  uint8_t  spi_mode = 0;
  uint8_t  bits_per_word = 0;
  uint32_t baudrate = 0;
  
  if (OS_MutSemTake(spi_bus_mutex[device->bus].spi_mutex) == OS_SUCCESS)
  {
    // Get mode
    ret = ioctl(device->handle, SPI_IOC_RD_MODE, &spi_mode);
    if (ret != SPI_SUCCESS)
    {
      status = SPI_ERR_RD_MODE;
      OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);
      return status;
    }  
    // Get bits per word
    ret = ioctl(device->handle, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word);
    if (ret != SPI_SUCCESS)
    {
      status = SPI_ERR_RD_BPW;
      OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);
      return status;
    }  
    // Get max speed
    ret = ioctl(device->handle, SPI_IOC_RD_MAX_SPEED_HZ, &baudrate);
    if (ret != SPI_SUCCESS)
    {
      status = SPI_ERR_RD_SD_HZ;
      OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);
      return status;
    }
  }
  OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);

  device->spi_mode = spi_mode;
  device->bits_per_word = bits_per_word;
  device->baudrate = baudrate;

  return status;
}

int32_t spi_write(spi_info_t* device, uint8_t data[], const uint32_t numBytes)
{
  int32_t status = SPI_SUCCESS;

  status = spi_transaction(device, NULL, data, numBytes, 0, 8, 0);

  return status;     
}

int32_t spi_read(spi_info_t* device, uint8_t data[], const uint32_t numBytes)
{
  int32_t status = SPI_SUCCESS;

  status = spi_transaction(device, NULL, data, numBytes, 0, 8, 0);

  return status;  
}

int32_t spi_transaction(spi_info_t* device, uint8_t *txBuff, uint8_t * rxBuffer, uint32_t length, uint16_t delay, uint8_t bits, uint8_t deselect)
{
  int32_t status = SPI_SUCCESS;
  int ret;
  struct spi_ioc_transfer xfer;

  // Clear the xfer struct
  memset((void *)&xfer, 0, sizeof(struct spi_ioc_transfer));

  // Setup the transfer structure for the transaction 
  xfer.tx_buf = (unsigned long) txBuff;
  xfer.rx_buf = (unsigned long) rxBuffer; 

  xfer.len = length;
  xfer.speed_hz = device->baudrate;
  
  xfer.delay_usecs = delay;
  xfer.bits_per_word = bits;
  xfer.cs_change = deselect;
  xfer.tx_nbits = (bits * length);
  xfer.rx_nbits = (bits * length);

  // Perform a full duplex transaction
  ret = ioctl(device->handle, SPI_IOC_MESSAGE(1), &xfer);
  if (ret < 1)
  {
      status = SPI_ERR_IOC_MSG;
      return status;
  }
  
  return status;
}

int32_t spi_select_chip(spi_info_t* device)
{
  int32_t status = SPI_SUCCESS;

  /* 
  * See "/usr/share/doc/linux-doc/spi/spidev":
  * "From userspace, you can't currently change the chip select polarity;
  *  that could corrupt transfers to other devices sharing the SPI bus.
  *  Each SPI device is deselected when it's not in active use, allowing
  *  other drivers to talk to other devices."
  */
  status = OS_MutSemTake(spi_bus_mutex[device->bus].spi_mutex);
  
  return status;
}

int32_t spi_unselect_chip(spi_info_t* device)
{
  int32_t status = SPI_SUCCESS;

  /* 
  * See "/usr/share/doc/linux-doc/spi/spidev":
  * "From userspace, you can't currently change the chip select polarity;
  *  that could corrupt transfers to other devices sharing the SPI bus.
  *  Each SPI device is deselected when it's not in active use, allowing
  *  other drivers to talk to other devices."
  */
  status = OS_MutSemGive(spi_bus_mutex[device->bus].spi_mutex);
  
  return status;
}

int32_t spi_close_device(spi_info_t* device)
{
  int32_t status = SPI_SUCCESS;

  // Check for valid handle
  if (device->handle >= 0)
  {
    spi_bus_mutex[device->bus].users--;
    if (spi_bus_mutex[device->bus].users == 0)
    {
      OS_MutSemDelete(spi_bus_mutex[device->bus].spi_mutex);
    }

    status = close(device->handle);
    if (status != SPI_SUCCESS) 
    {
      status = SPI_ERR_FILE_CLOSE;
      return status;
    }
    // Set open flag
    device->isOpen = SPI_DEVICE_CLOSED;
  }
  else
  {
    status = SPI_ERR_FILE_HANDLE;
    return status;
  }

  return status;
}
