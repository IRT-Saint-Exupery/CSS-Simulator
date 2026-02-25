/*******************************************************************************
** File: generic_radio_device.c
**
** Purpose:
**   This file contains the source code for the GENERIC_RADIO device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_radio_device.h"


/* 
** Set Configuration
*/
int32_t GENERIC_RADIO_SetConfiguration(socket_info_t* device, uint32_t config)
{
    int32_t status = OS_SUCCESS;
    uint8_t write_data[GENERIC_RADIO_DEVICE_CMD_SIZE] = {0};
    size_t  bytes_sent = 0;

    config = CFE_MAKE_BIG32(config);

    /* Prepare command */
    write_data[0] = GENERIC_RADIO_DEVICE_HDR_0;
    write_data[1] = GENERIC_RADIO_DEVICE_HDR_1;
    write_data[2] = GENERIC_RADIO_DEVICE_CFG_CMD;
    write_data[3] = config >> 24;
    write_data[4] = config >> 16;
    write_data[5] = config >> 8;
    write_data[6] = config;
    write_data[7] = GENERIC_RADIO_DEVICE_TRAILER_0;
    write_data[8] = GENERIC_RADIO_DEVICE_TRAILER_1;

    /* Write command */
    status = socket_send(device, write_data,
                         GENERIC_RADIO_DEVICE_CMD_SIZE, &bytes_sent,
                         GENERIC_RADIO_CFG_DEVICE_IP, GENERIC_RADIO_CFG_UDP_FSW_TO_RADIO);
    if (bytes_sent != GENERIC_RADIO_DEVICE_CMD_SIZE)
    {
        #ifdef GENERIC_RADIO_CFG_DEBUG
            OS_printf("GENERIC_RADIO_SetConfiguration sent %d, but attempted %d \n", bytes_sent, GENERIC_RADIO_DEVICE_CMD_SIZE);
        #endif 
        status = OS_ERROR;
    }
    return status;
}


/* 
** Proximity Forward
*/
int32_t GENERIC_RADIO_ProximityForward(socket_info_t* device, uint16_t scid, uint8_t* data, uint16_t data_len)
{
    int32_t status = OS_SUCCESS;
    size_t  bytes_sent = 0;

    scid = CFE_MAKE_BIG16(scid);

    #ifdef GENERIC_RADIO_CFG_DEBUG
        OS_printf("GENERIC_RADIO_ProximityForward forwarding: ");
        for(uint16_t i = 0; i < data_len; i++)
        {
            OS_printf("0x%02x ", data[i]);
        }
        OS_printf("\n");
    #endif

    /* Write command */
    status = socket_send(device, data,
                         data_len, &bytes_sent,
                         GENERIC_RADIO_CFG_DEVICE_IP, GENERIC_RADIO_CFG_UDP_FSW_TO_PROX);
    if (bytes_sent != data_len)
    {
        #ifdef GENERIC_RADIO_CFG_DEBUG
            OS_printf("GENERIC_RADIO_ProximityForward sent %d, but attempted %d \n", bytes_sent, data_len + 2);
        #endif
        status = OS_ERROR;
    }
    return status;
}


/*
** Request housekeeping command
*/
int32_t GENERIC_RADIO_RequestHK(socket_info_t* device, GENERIC_RADIO_Device_HK_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t write_data[GENERIC_RADIO_DEVICE_CMD_SIZE] = {0};
    uint8_t read_data[GENERIC_RADIO_DEVICE_HK_SIZE] = {0};
    size_t  bytes = 0;

    /* Prepare command */
    write_data[0] = GENERIC_RADIO_DEVICE_HDR_0;
    write_data[1] = GENERIC_RADIO_DEVICE_HDR_1;
    write_data[2] = GENERIC_RADIO_DEVICE_REQ_HK_CMD;
    write_data[3] = 0;
    write_data[4] = 0;
    write_data[5] = 0;
    write_data[6] = 0;
    write_data[7] = GENERIC_RADIO_DEVICE_TRAILER_0;
    write_data[8] = GENERIC_RADIO_DEVICE_TRAILER_1;

    /* Write command */
    status = socket_send(device, write_data,
                         GENERIC_RADIO_DEVICE_CMD_SIZE, &bytes,
                         GENERIC_RADIO_CFG_DEVICE_IP, GENERIC_RADIO_CFG_UDP_FSW_TO_RADIO);
    if (bytes != GENERIC_RADIO_DEVICE_CMD_SIZE)
    {
        #ifdef GENERIC_RADIO_CFG_DEBUG
            OS_printf("GENERIC_RADIO_RequestHK sent %d, but attempted %d \n", bytes, GENERIC_RADIO_DEVICE_CMD_SIZE);
        #endif 
        status = OS_ERROR;
    }
    if (status != OS_SUCCESS)
    {
        #ifdef GENERIC_RADIO_CFG_DEBUG
            OS_printf("GENERIC_RADIO_RequestHK socket_send reported error of %d \n", status);
        #endif 
    }

    OS_TaskDelay(GENERIC_RADIO_CFG_DEVICE_DELAY_MS);

    /* Read response */
    status = socket_recv(device, read_data,
                         GENERIC_RADIO_DEVICE_HK_SIZE, &bytes);
    if (bytes != GENERIC_RADIO_DEVICE_HK_SIZE)
    {
        #ifdef GENERIC_RADIO_CFG_DEBUG
            OS_printf("GENERIC_RADIO_RequestHK received %d, but expected %d \n", bytes, GENERIC_RADIO_DEVICE_HK_SIZE);
        #endif 
        status = OS_ERROR;
    }
    else
    {
        if (status != OS_SUCCESS)
        {
            #ifdef GENERIC_RADIO_CFG_DEBUG
                OS_printf("GENERIC_RADIO_RequestHK socket_recv reported error of %d \n", status);
            #endif 
        }
        else
        {
            #ifdef GENERIC_RADIO_CFG_DEBUG
                OS_printf("GENERIC_RADIO_RequestHK received: ");
                for(int i = 0; i < (int) bytes; i++)
                {
                    OS_printf("0x%02x ", read_data[i]);
                }
                OS_printf("\n");
            #endif
            
            /* Verify data header and trailer */
            if ((read_data[0] == GENERIC_RADIO_DEVICE_HDR_0) && 
                (read_data[1] == GENERIC_RADIO_DEVICE_HDR_1) && 
                (read_data[GENERIC_RADIO_DEVICE_HK_SIZE-2] == GENERIC_RADIO_DEVICE_TRAILER_0) && 
                (read_data[GENERIC_RADIO_DEVICE_HK_SIZE-1] == GENERIC_RADIO_DEVICE_TRAILER_1) )
            {
                data->DeviceCounter  = read_data[2] << 24;
                data->DeviceCounter |= read_data[3] << 16;
                data->DeviceCounter |= read_data[4] << 8;
                data->DeviceCounter |= read_data[5];

                data->DeviceConfig  = read_data[6] << 24;
                data->DeviceConfig |= read_data[7] << 16;
                data->DeviceConfig |= read_data[8] << 8;
                data->DeviceConfig |= read_data[9];

                data->ProxSignal  = read_data[10] << 24;
                data->ProxSignal |= read_data[11] << 16;
                data->ProxSignal |= read_data[12] << 8;
                data->ProxSignal |= read_data[13];

                #ifdef GENERIC_RADIO_CFG_DEBUG
                    OS_printf("  Header     = 0x%02x%02x  \n", read_data[0], read_data[1]);
                    OS_printf("  Counter    = 0x%08x      \n", data->DeviceCounter);
                    OS_printf("  Config     = 0x%08x      \n", data->DeviceConfig);
                    OS_printf("  ProxSignal = 0x%08x      \n", data->ProxSignal);
                    OS_printf("  Trailer    = 0x%02x%02x  \n", read_data[14], read_data[15]);
                #endif
            }
            else
            {
                #ifdef GENERIC_RADIO_CFG_DEBUG
                    OS_printf("GENERIC_RADIO_RequestHK: Invalid header / trailer detected! \n");
                #endif 
                status = OS_ERROR;
            }
        }
    }
    return status;
}
