/*******************************************************************************
** File: generic_mag_device.c
**
** Purpose:
**   This file contains the source code for the GENERIC_MAG device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_mag_device.h"

/*
** Request data command
*/
int32_t GENERIC_MAG_RequestData(spi_info_t *device, GENERIC_MAG_Device_Data_tlm_t* data)
{
    int32_t status = OS_ERROR;
    uint8_t read_data[GENERIC_MAG_DEVICE_DATA_SIZE];
    uint32_t mag_x_packet;
    uint32_t mag_y_packet;
    uint32_t mag_z_packet;

    /* Read data */
    if ((spi_select_chip(device) == SPI_SUCCESS) &&
        (spi_read(device, read_data, sizeof(read_data)) == SPI_SUCCESS) &&
        (spi_unselect_chip(device) == SPI_SUCCESS)) {
        status = OS_SUCCESS;
    }

    if (status == OS_SUCCESS)
    {
        #ifdef GENERIC_MAG_CFG_DEBUG
            OS_printf("  GENERIC_MAG_RequestData = 0x");
            for (uint32_t i = 0; i < sizeof(read_data); i++)
            {
                OS_printf("%02x, ", read_data[i]);
            }
            OS_printf("\n");
        #endif
        if ((read_data[0]  == GENERIC_MAG_DEVICE_HDR_0) && 
            (read_data[1]  == GENERIC_MAG_DEVICE_HDR_1) && 
            (read_data[2]  == GENERIC_MAG_DEVICE_HDR_2) && 
            (read_data[3]  == GENERIC_MAG_DEVICE_HDR_3))
        {
            mag_x_packet = (read_data[4] << 24) | (read_data[5] << 16) | (read_data[6] << 8) | (read_data[7]);
            mag_y_packet = (read_data[8] << 24) | (read_data[9] << 16) | (read_data[10] << 8) | (read_data[11]);
            mag_z_packet = (read_data[12] << 24) | (read_data[13] << 16) | (read_data[14] << 8) | (read_data[15]);

            data->MagneticIntensityX = ((((int32_t) mag_x_packet)-(MAG_CONV_CONST*MAG_RANGE))/(MAG_CONV_CONST));
            data->MagneticIntensityY = ((((int32_t) mag_y_packet)-(MAG_CONV_CONST*MAG_RANGE))/(MAG_CONV_CONST));
            data->MagneticIntensityZ = ((((int32_t) mag_z_packet)-(MAG_CONV_CONST*MAG_RANGE))/(MAG_CONV_CONST));

            #ifdef GENERIC_MAG_CFG_DEBUG
                OS_printf("  Magnetic Field Intensity X = 0x%08x, %d (nT)\n", data->MagneticIntensityX, data->MagneticIntensityX);
                OS_printf("  Magnetic Field Intensity Y = 0x%08x, %d (nT)\n", data->MagneticIntensityY, data->MagneticIntensityY);
                OS_printf("  Magnetic Field Intensity Z = 0x%08x, %d (nT)\n", data->MagneticIntensityZ, data->MagneticIntensityZ);
            #endif
        }
    } 
    else
    {
        #ifdef GENERIC_MAG_CFG_DEBUG
            OS_printf("  GENERIC_MAG_RequestData: Invalid data read! \n");
        #endif 
        status = OS_ERROR;
    }
    return status;
}