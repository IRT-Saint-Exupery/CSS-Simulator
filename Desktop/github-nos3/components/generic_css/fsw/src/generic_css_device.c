/*******************************************************************************
** File: generic_css_device.c
**
** Purpose:
**   This file contains the source code for the GENERIC_CSS device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_css_device.h"


/*
** Request data command
*/
int32_t GENERIC_CSS_RequestData(i2c_bus_info_t* device, GENERIC_CSS_Device_Data_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_CSS_DEVICE_DATA_LNGTH] = {0};

    status = i2c_master_transaction(device, GENERIC_CSS_I2C_ADDRESS, NULL, 0, read_data, GENERIC_CSS_DEVICE_DATA_LNGTH, GENERIC_CSS_CFG_MS_TIMEOUT);
    if (status == OS_SUCCESS)
    {
        #ifdef GENERIC_CSS_CFG_DEBUG
            OS_printf("  GENERIC_CSS_RequestData = 0x");
            for (uint32_t i = 0; i < sizeof(read_data); i++)
            {
                OS_printf("%02x, ", read_data[i]);
            }
            OS_printf("\n");
        #endif

        data->Voltage[0] = (read_data[0] << 8) | read_data[1];
        data->Voltage[1] = (read_data[2] << 8) | read_data[3];
        data->Voltage[2] = (read_data[4] << 8) | read_data[5];
        data->Voltage[3] = (read_data[6] << 8) | read_data[7];
        data->Voltage[4] = (read_data[8] << 8) | read_data[9];
        data->Voltage[5] = (read_data[10] << 8) | read_data[11];

        #ifdef GENERIC_CSS_CFG_DEBUG
            OS_printf("  Voltage ADC 1  = 0x%04x, %d  \n", data->Voltage[0], data->Voltage[0]);
            OS_printf("  Voltage ADC 2  = 0x%04x, %d  \n", data->Voltage[1], data->Voltage[1]);
            OS_printf("  Voltage ADC 3  = 0x%04x, %d  \n", data->Voltage[2], data->Voltage[2]);
            OS_printf("  Voltage ADC 4  = 0x%04x, %d  \n", data->Voltage[3], data->Voltage[3]);
            OS_printf("  Voltage ADC 5  = 0x%04x, %d  \n", data->Voltage[4], data->Voltage[4]);
            OS_printf("  Voltage ADC 6  = 0x%04x, %d  \n", data->Voltage[5], data->Voltage[5]);
        #endif
    } 
    else
    {
        #ifdef GENERIC_CSS_CFG_DEBUG
            OS_printf("  GENERIC_CSS_RequestData: Invalid data read! \n");
        #endif 
        status = OS_ERROR;
    }
    return status;
}
