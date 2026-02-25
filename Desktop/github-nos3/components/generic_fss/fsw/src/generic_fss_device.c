/*******************************************************************************
** File: generic_fss_device.c
**
** Purpose:
**   This file contains the source code for the GENERIC_FSS device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "device_cfg.h"
#include "generic_fss_platform_cfg.h"
#include "generic_fss_device.h"

/*
** Message definitions
*/
#define GENERIC_FSS_DEVICE_HDR_0            0xDE
#define GENERIC_FSS_DEVICE_HDR_1            0xAD
#define GENERIC_FSS_DEVICE_HDR_2            0xBE
#define GENERIC_FSS_DEVICE_HDR_3            0xEF

#define GENERIC_FSS_DEVICE_NOOP_CMD         0x00
#define GENERIC_FSS_DEVICE_REQ_DATA_CMD     0x01

#define GENERIC_FSS_DEVICE_HDR_TRL_LEN      4
#define GENERIC_FSS_DEVICE_CMD_SIZE         7   // 7 IRT CSS

#define GENERIC_FSS_DEVICE_DATA_SIZE        16

// Forward declarations
static int32_t GENERIC_FSS_ReadData(spi_info_t *device, uint8_t* write_data, uint8_t* read_data, uint8_t data_length);
static int32_t GENERIC_FSS_CommandDevice(spi_info_t *device, uint8_t cmd);
static uint8_t compute_checksum(uint8_t in[], int starting_byte, int number_of_bytes);
static float fourbytes_little_endian_to_float(uint8_t bytes[]);

/*
** Request data command
*/
int32_t GENERIC_FSS_RequestData(spi_info_t *device, GENERIC_FSS_Device_Data_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[GENERIC_FSS_DEVICE_DATA_SIZE];
    uint8_t write_data[GENERIC_FSS_DEVICE_DATA_SIZE];

    /* Command device to send HK */
    status = GENERIC_FSS_CommandDevice(device, GENERIC_FSS_DEVICE_REQ_DATA_CMD);
    if (status == OS_SUCCESS)
    {
        status = GENERIC_FSS_ReadData(device, write_data, read_data, sizeof(read_data));
        if (status == OS_SUCCESS)
        {
            #ifdef GENERIC_FSS_CFG_DEBUG
                OS_printf("  GENERIC_FSS_RequestData = ");
                for (uint32_t i = 0; i < sizeof(read_data); i++)
                {
                    OS_printf("%02x", read_data[i]);
                }
                OS_printf("\n");
            #endif

            uint8_t checksum = compute_checksum(read_data, 4, 11);
            /* Verify data header, command code, length, and checksum */
            if ((read_data[0]  == GENERIC_FSS_DEVICE_HDR_0) && 
                (read_data[1]  == GENERIC_FSS_DEVICE_HDR_1) && 
                (read_data[2]  == GENERIC_FSS_DEVICE_HDR_2) && 
                (read_data[3]  == GENERIC_FSS_DEVICE_HDR_3) &&
                (read_data[4]  == 0x01)                     &&
                (read_data[5]  == 0x0A)                     &&
                (read_data[15] == checksum))
            {
                // alpha
                data->Alpha = fourbytes_little_endian_to_float(&read_data[6]);
                // beta
                data->Beta = fourbytes_little_endian_to_float(&read_data[10]);
                // error code
                data->ErrorCode = read_data[14];
                
                #ifdef GENERIC_FSS_CFG_DEBUG
                    OS_printf("  Header           = 0x%02x%02x%02x%02x  \n", read_data[0], read_data[1], read_data[2], read_data[3]);
                    OS_printf("  Command          = 0x%08x              \n", read[4]);
                    OS_printf("  Length           = 0x%08x              \n", read[5]);
                    OS_printf("  Checksum         = 0x%08x              \n", read[15]);
                    OS_printf("  Data alpha       = 0x%04x, %d          \n", data->Alpha, data->Alpha);
                    OS_printf("  Data beta        = 0x%04x, %d          \n", data->Beta, data->Beta);
                    OS_printf("  Data error code  = 0x%04x, %d          \n", data->ErrorCode, data->ErrorCode);
                #endif
            }
        } 
        else
        {
            #ifdef GENERIC_FSS_CFG_DEBUG
                OS_printf("  GENERIC_FSS_RequestData: Invalid data read! \n");
            #endif 
            status = OS_ERROR;
        }
    }
    else
    {
        #ifdef GENERIC_FSS_CFG_DEBUG
            OS_printf("  GENERIC_FSS_RequestData: GENERIC_FSS_CommandDevice reported error %d \n", status);
        #endif 
    }
    return status;
}

/* 
** Generic read data from device
*/
static int32_t GENERIC_FSS_ReadData(spi_info_t *device, uint8_t* write_data, uint8_t* read_data, uint8_t data_length)
{
    int32_t status = OS_ERROR;
    //int32_t bytes = 0;
    //int32_t bytes_available = 0;
    //uint8_t ms_timeout_counter = 0;

    /* Read data */
    if ((spi_select_chip(device) == SPI_SUCCESS) &&
        (spi_transaction(device, write_data, read_data, data_length, GENERIC_FSS_CFG_DELAY, GENERIC_FSS_CFG_BITS_PER_WORD, 0) == SPI_SUCCESS) &&
        (spi_unselect_chip(device) == SPI_SUCCESS)) {
        status = OS_SUCCESS;
    }

    return status;
}


/* 
** Generic command to device
** Note that confirming the echoed response is specific to this implementation
*/
static int32_t GENERIC_FSS_CommandDevice(spi_info_t *device, uint8_t cmd_code)
{
    int32_t status = OS_ERROR;
    //int32_t bytes = 0;
    uint8_t write_data[GENERIC_FSS_DEVICE_CMD_SIZE]; 
    uint8_t read_data[GENERIC_FSS_DEVICE_CMD_SIZE];

    /* Prepare command */
    write_data[0] = GENERIC_FSS_DEVICE_HDR_0;
    write_data[1] = GENERIC_FSS_DEVICE_HDR_1;
    write_data[2] = GENERIC_FSS_DEVICE_HDR_2;
    write_data[3] = GENERIC_FSS_DEVICE_HDR_3;
    write_data[4] = cmd_code;
    write_data[5] = 0x01;
    write_data[6] = compute_checksum(write_data, 4, 2);

    #ifdef GENERIC_FSS_CFG_DEBUG
        OS_printf("  GENERIC_FSS_CommandDevice[%d] = ", status);
        for (uint32_t i = 0; i < GENERIC_FSS_DEVICE_CMD_SIZE; i++)
        {
            OS_printf("%02x", write_data[i]);
        }
        OS_printf("\n");
    #endif

    /* Write data */
    if ((spi_select_chip(device) == SPI_SUCCESS) &&
        (spi_transaction(device, write_data, read_data, GENERIC_FSS_DEVICE_CMD_SIZE, GENERIC_FSS_CFG_DELAY, GENERIC_FSS_CFG_BITS_PER_WORD, 0) == SPI_SUCCESS) &&
        (spi_unselect_chip(device) == SPI_SUCCESS)) {
        status = OS_SUCCESS;
    }
    
    return status;
}

static uint8_t compute_checksum(uint8_t in[], int starting_byte, int number_of_bytes)
{
    uint32_t sum = 0;
    uint8_t checksum;
    for (int i = starting_byte; i < starting_byte + number_of_bytes; i++) {
        sum += in[i];
    }
    checksum = (uint8_t)(sum & 0x000000FF);
    return checksum;
}

static float fourbytes_little_endian_to_float(uint8_t bytes[])
{
    union {float f; uint32_t u;} fu;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    fu.u = ((uint32_t)bytes[3] << 24) | ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[1] << 8) | (uint32_t)bytes[0];
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    fu.u = ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) | ((uint32_t)bytes[2] << 8) | (uint32_t)bytes[3];
#else
    #error "__BYTE_ORDER__ is not defined"
#endif
    return fu.f;
}

