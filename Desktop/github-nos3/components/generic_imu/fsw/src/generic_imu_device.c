/*******************************************************************************
** File: generic_imu_device.c
**
** Purpose:
**   This file contains the source code for the GENERIC_IMU device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_imu_device.h"

// Both of the below constants are defined to allow the greatest possible precision
// without overflow on a range of -10<x<10 for x=linear acceleration (in g) and
// -400<x<400 for x=angular rotation rate (in deg/s).
#define LIN_CONV_CONST 214748364.0 //multiply by 10 to get the added constant
#define ANG_CONV_CONST 5368709.0 //multiply by 400 to get the added constant

/*
** Helper Function
*/
void GENERIC_IMU_FramePrep(can_info_t *device, uint8_t* data, uint8_t data_len)
{
    /* TX Frame */
    device->tx_frame.can_id = GENERIC_IMU_CFG_CAN_ID;
    device->tx_frame.can_dlc = data_len;
    CFE_PSP_MemCpy((void*)device->tx_frame.data, data, CAN_MAX_DLEN);
    
    /* RX Frame */
    device->rx_frame.can_id = GENERIC_IMU_CFG_CAN_ID;
    device->rx_frame.can_dlc = 0;
    CFE_PSP_MemSet((void*)device->rx_frame.data, 0x00, CAN_MAX_DLEN);
}

/* 
** Generic read data from device
*/
int32_t GENERIC_IMU_ReadData(can_info_t *canDevice, uint8_t data_length)
{
    int32_t status = OS_SUCCESS;

    /* Wait until all data received or timeout occurs */
    status = can_master_transaction(canDevice);
    if (status != CAN_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("GENERIC_IMU_ReadData: GENERIC_IMU_ReadData can_master_transaction failed with %d error! \n", status);
        #endif
        status = CAN_ERROR;
    }
    return status;
}


/* 
** Generic command to device
** Note that confirming the echoed response is specific to this implementation
*/
int32_t GENERIC_IMU_CommandDevice(can_info_t *canDevice, uint8_t cmd_code)
{
    int32_t status = OS_SUCCESS;
    int32_t bytes = 0;
    uint8_t write_data[GENERIC_IMU_DEVICE_CMD_SIZE] = {0};

    /* Prepare command */
    write_data[0] = GENERIC_IMU_DEVICE_HDR;
    write_data[1] = cmd_code;

    GENERIC_IMU_FramePrep(canDevice, write_data, GENERIC_IMU_DEVICE_CMD_SIZE);
    status = GENERIC_IMU_ReadData(canDevice, GENERIC_IMU_DEVICE_CMD_SIZE);
    if (status != OS_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
        OS_printf("GENERIC_IMU_CommandDevice - GENERIC_IMU_ReadData returned %d \n", status);
        #endif
    }
    return status;
}


/*
** Request housekeeping command
*/
int32_t GENERIC_IMU_RequestHK(can_info_t *canDevice, GENERIC_IMU_Device_HK_tlm_t* data)
{
    int32_t status = OS_SUCCESS;

    status = GENERIC_IMU_CommandDevice(canDevice, GENERIC_IMU_DEVICE_REQ_HK_CMD);
    if (status == OS_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestHK[%d] = ", canDevice->rx_frame.can_dlc);
            for (uint32_t i = 0; i < canDevice->rx_frame.can_dlc; i++)
            {
                OS_printf("%02x", canDevice->rx_frame.data[i]);
            }
            OS_printf("\n");
        #endif

        data->DeviceCounter  = canDevice->rx_frame.data[0] << 24;
        data->DeviceCounter |= canDevice->rx_frame.data[1] << 16;
        data->DeviceCounter |= canDevice->rx_frame.data[2] << 8;
        data->DeviceCounter |= canDevice->rx_frame.data[3];

        data->DeviceStatus  = canDevice->rx_frame.data[4] << 24;
        data->DeviceStatus |= canDevice->rx_frame.data[5] << 16;
        data->DeviceStatus |= canDevice->rx_frame.data[6] << 8;
        data->DeviceStatus |= canDevice->rx_frame.data[7];

        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  Counter = 0x%08x      \n", data->DeviceCounter);
            OS_printf("  Status  = 0x%08x      \n", data->DeviceStatus);
        #endif
    }
    return status;
}


/*
** Request Axis
*/
int32_t GENERIC_IMU_RequestAxis(can_info_t *canDevice, GENERIC_IMU_Device_Axis_Data_t* data, uint8_t cmd_code)
{
    int32_t status = OS_SUCCESS;
    uint32_t la_tmp;
    uint32_t aa_tmp;

    status = GENERIC_IMU_CommandDevice(canDevice, cmd_code);
    if (status == OS_SUCCESS)
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestAxis %d, [%d] = ", cmd_code, canDevice->rx_frame.can_dlc);
            for (uint32_t i = 0; i < canDevice->rx_frame.can_dlc; i++)
            {
                OS_printf("%02x", canDevice->rx_frame.data[i]);
            }
            OS_printf("\n");
        #endif

        /* Verify return frame length */

        /* Proces data */
        la_tmp  = canDevice->rx_frame.data[0] << 24;
        la_tmp |= canDevice->rx_frame.data[1] << 16;
        la_tmp |= canDevice->rx_frame.data[2] << 8;
        la_tmp |= canDevice->rx_frame.data[3];

        aa_tmp  = canDevice->rx_frame.data[4] << 24;
        aa_tmp |= canDevice->rx_frame.data[5] << 16;
        aa_tmp |= canDevice->rx_frame.data[6] << 8;
        aa_tmp |= canDevice->rx_frame.data[7];

        /* Float conversion */
        data->LinearAcc  = (float) ((la_tmp - (LIN_CONV_CONST*10.0)) / LIN_CONV_CONST);
        data->AngularAcc  = (float) ((aa_tmp - (ANG_CONV_CONST*400.0)) / ANG_CONV_CONST);
    } 
    else
    {
        #ifdef GENERIC_IMU_CFG_DEBUG
            OS_printf("  GENERIC_IMU_RequestAxis: Invalid data read! \n");
        #endif 
        status = OS_ERROR;
    } 
    return status;
}


/*
** Request data command
*/
int32_t GENERIC_IMU_RequestData(can_info_t *canDevice, GENERIC_IMU_Device_Data_tlm_t* data)
{
    int32_t status = OS_SUCCESS;

    status = GENERIC_IMU_RequestAxis(canDevice, &data->X_Data, GENERIC_IMU_DEVICE_REQ_X_DATA_CMD);
    if (status == OS_SUCCESS)
    {
        status = GENERIC_IMU_RequestAxis(canDevice, &data->Y_Data, GENERIC_IMU_DEVICE_REQ_Y_DATA_CMD);
        if (status == OS_SUCCESS)
        {   
            status = GENERIC_IMU_RequestAxis(canDevice, &data->Z_Data, GENERIC_IMU_DEVICE_REQ_Z_DATA_CMD);
        }
    }

    #ifdef GENERIC_IMU_CFG_DEBUG
        if (status != OS_SUCCESS)
        {
            OS_printf("  GENERIC_IMU_RequestData: Error %d reported in GENERIC_IMU_RequestAxis \n", status);
        }
        OS_printf("GENERIC_IMU_RequestData\n");
        OS_printf("  Linear X  = 0x%08x, ", data->X_Data.LinearAcc);
        OS_printf("%f  \n", data->X_Data.LinearAcc);
        OS_printf("  Angular X = 0x%08x, ", data->X_Data.AngularAcc);
        OS_printf("%f  \n",  data->X_Data.AngularAcc);
        OS_printf("  Linear Y  = 0x%08x, ", data->Y_Data.LinearAcc);
        OS_printf("%f  \n", data->Y_Data.LinearAcc);
        OS_printf("  Angular Y = 0x%08x, ", data->Y_Data.AngularAcc);
        OS_printf("%f  \n",  data->Y_Data.AngularAcc);
        OS_printf("  Linear Z  = 0x%08x, ", data->Z_Data.LinearAcc);
        OS_printf("%f  \n", data->Z_Data.LinearAcc);
        OS_printf("  Angular Z = 0x%08x, ", data->Z_Data.AngularAcc);
        OS_printf("%f  \n",  data->Z_Data.AngularAcc);
        OS_printf("\n");
    #endif 

    return status;
}

