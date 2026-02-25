/*******************************************************************************
** File: generic_eps_device.c
**
** Purpose:
**   This file contains the source code for the GENERIC_EPS device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_eps_device.h"


/*
** Generic slow CRC8 calculator
*/
uint8_t GENERIC_EPS_CRC8(uint8_t* payload, uint32_t length)
{
    uint8_t crc = 0xFF;
    uint32_t i;
    uint32_t j;

    for (i = 0; i < length; i++) 
    {
        crc ^= payload[i];
        for (j = 0; j < 8; j++) 
        {
            if ((crc & 0x80) != 0)
            {
                crc = (uint8_t)((crc << 1) ^ 0x31);
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}


/* 
** Generic command to device
** Note that confirming the echoed response is specific to this implementation
*/
int32_t GENERIC_EPS_CommandDevice(i2c_bus_info_t* device, uint8_t cmd, uint8_t value)
{
    int32_t status = OS_SUCCESS;
    uint8_t write_data[3] = {0};
  /*  uint8_t i; */

    /* Confirm command valid */
    if (cmd < 0xAB)
    {
        /* Prepare command */
        write_data[0] = cmd;
        write_data[1] = value;
        write_data[2] = GENERIC_EPS_CRC8(write_data, 2);

        /* Initiate transaction */
        i2c_master_transaction(device, GENERIC_EPS_CFG_I2C_ADDRESS,
                               write_data, 3, 
                               NULL, 0, 
                               GENERIC_EPS_CFG_I2C_TIMEOUT);
    }
    else
    {
        status = OS_ERROR;
        #ifdef GENERIC_EPS_CFG_DEBUG
            OS_printf("  GENERIC_EPS_CommandDevice: Command 0x%02x is above valid range! (>= 0xAB expected) \n", cmd);
        #endif 
    }
    return status;
}


/*
** Request housekeeping command
*/
int32_t GENERIC_EPS_RequestHK(i2c_bus_info_t* device, GENERIC_EPS_Device_HK_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t write_data[3] = {0};
    uint8_t read_data[GENERIC_EPS_DEVICE_HK_LEN+1] = {0};
    uint8_t calc_crc = 0;
    uint8_t offset = 16;

    /* Prepare command */
    write_data[0] = 0x70;
    write_data[1] = 0;
    write_data[2] = GENERIC_EPS_CRC8(write_data,2);

    /* Initiate transaction */
    i2c_master_transaction(device, GENERIC_EPS_CFG_I2C_ADDRESS,
                           write_data, 3, 
                           read_data, GENERIC_EPS_DEVICE_HK_LEN+1, 
                           GENERIC_EPS_CFG_I2C_TIMEOUT);

    /* Confirm CRC */
    calc_crc = GENERIC_EPS_CRC8(read_data, GENERIC_EPS_DEVICE_HK_LEN);
    if (calc_crc == read_data[GENERIC_EPS_DEVICE_HK_LEN])
    {
        /* Interpret Data */
        data->BatteryVoltage        = (read_data[0] << 8) | read_data[1];
        data->BatteryTemperature    = (read_data[2] << 8) | read_data[3];

        data->Bus3p3Voltage         = (read_data[4] << 8) | read_data[5];
        data->Bus5p0Voltage         = (read_data[6] << 8) | read_data[7];
        data->Bus12Voltage          = (read_data[8] << 8) | read_data[9];
        data->EPSTemperature        = (read_data[10] << 8) | read_data[11];

        data->SolarArrayVoltage     = (read_data[12] << 8) | read_data[13];
        data->SolarArrayTemperature = (read_data[14] << 8) | read_data[15];

        for(uint8_t i = 0; i < 8; i++)
        {
            data->Switch[i].Voltage = (read_data[offset] << 8)   | read_data[offset+1];
            data->Switch[i].Current = (read_data[offset+2] << 8) | read_data[offset+3];
            data->Switch[i].Status  = (read_data[offset+4] << 8) | read_data[offset+5];
            offset = offset + 6;
        }
    }
    else
    {
        status = OS_ERROR;
        #ifdef GENERIC_EPS_CFG_DEBUG
            OS_printf("  GENERIC_EPS_RequestHK: CRC error, expected 0x%02x and received 0x%02x \n", calc_crc, read_data[GENERIC_EPS_DEVICE_HK_LEN]);
        #endif 
    }

    #ifdef GENERIC_EPS_CFG_DEBUG
        OS_printf("  GENERIC_EPS_RequestHK read: ");
        for(uint8_t i; i < GENERIC_EPS_DEVICE_HK_LEN+1; i++)
        {
            OS_printf("0x%02x ",read_data[i]);
        }
        OS_printf("\n");
        OS_printf("  GENERIC_EPS_RequestHK: Battery Voltage = 0x%04x \n", data->BatteryVoltage);
        OS_printf("  GENERIC_EPS_RequestHK: Battery Temperature = 0x%04x \n", data->BatteryTemperature);
        OS_printf("  GENERIC_EPS_RequestHK: Bus 3.3V = 0x%04x \n", data->Bus3p3Voltage);
    #endif

    return status;
}


/*
** Command EPS Switch
*/
int32_t GENERIC_EPS_CommandSwitch(i2c_bus_info_t* device, uint8_t switch_num, uint8_t value, GENERIC_EPS_Device_HK_tlm_t* data)
{
    int32_t status = OS_SUCCESS;

    /* Check switch number valid */
    if (switch_num < 8)
    {
        /* Check value valid */
        if ((value == 0x00) || (value == 0xAA))
        {
            /* Command switch state */
            status = GENERIC_EPS_CommandDevice(device, switch_num, value);
            if (status == OS_SUCCESS)
            {
                /* Get HK */
                status = GENERIC_EPS_RequestHK(device, data);
                
                /* Confirm switch state changed in HK */
                if (status == OS_SUCCESS)
                {
                    if ((data->Switch[switch_num].Status & 0x00FF) != value)
                    {
                        status = OS_ERROR;
                        #ifdef GENERIC_EPS_CFG_DEBUG
                            OS_printf("  GENERIC_EPS_CommandSwitch: HK reported incorrect switch state after command! (0x%02x expected, 0x%02x actual) \n", value, (data->Switch[switch_num].Status & 0x00FF));
                        #endif 
                    }
                }
            }
        }
        else
        {
            status = OS_ERROR;
            #ifdef GENERIC_EPS_CFG_DEBUG
                OS_printf("  GENERIC_EPS_CommandSwitch: Value number of 0x%02x is invalid! (0x00 or 0xAA expected) \n", value);
            #endif 
        }   
    }
    else
    {
        status = OS_ERROR;
        #ifdef GENERIC_EPS_CFG_DEBUG
            OS_printf("  GENERIC_EPS_CommandSwitch: Switch number of %d is invalid! (>= 8 expected) \n", switch_num);
        #endif 
    }
    return status;
}
