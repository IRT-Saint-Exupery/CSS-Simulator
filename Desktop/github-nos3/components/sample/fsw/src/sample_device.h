/*******************************************************************************
** File: sample_device.h
**
** Purpose:
**   This is the header file for the SAMPLE device.
**
*******************************************************************************/
#ifndef _SAMPLE_DEVICE_H_
#define _SAMPLE_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "sample_platform_cfg.h"


/*
** Type definitions
** TODO: Make specific to your application
*/
#define SAMPLE_DEVICE_HDR              0xDEAD
#define SAMPLE_DEVICE_HDR_0            0xDE
#define SAMPLE_DEVICE_HDR_1            0xAD

#define SAMPLE_DEVICE_NOOP_CMD         0x00
#define SAMPLE_DEVICE_REQ_HK_CMD       0x01
#define SAMPLE_DEVICE_REQ_DATA_CMD     0x02
#define SAMPLE_DEVICE_CFG_CMD          0x03

#define SAMPLE_DEVICE_TRAILER          0xBEEF
#define SAMPLE_DEVICE_TRAILER_0        0xBE
#define SAMPLE_DEVICE_TRAILER_1        0xEF

#define SAMPLE_DEVICE_HDR_TRL_LEN      4
#define SAMPLE_DEVICE_CMD_SIZE         9

/*
** SAMPLE device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint32_t  DeviceConfig;
    uint32_t  DeviceStatus;

} __attribute__((packed)) SAMPLE_Device_HK_tlm_t;
#define SAMPLE_DEVICE_HK_LNGTH sizeof ( SAMPLE_Device_HK_tlm_t )
#define SAMPLE_DEVICE_HK_SIZE SAMPLE_DEVICE_HK_LNGTH + SAMPLE_DEVICE_HDR_TRL_LEN


/*
** SAMPLE device data telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint16_t  DeviceDataX;
    uint16_t  DeviceDataY;
    uint16_t  DeviceDataZ;

} __attribute__((packed)) SAMPLE_Device_Data_tlm_t;
#define SAMPLE_DEVICE_DATA_LNGTH sizeof ( SAMPLE_Device_Data_tlm_t )
#define SAMPLE_DEVICE_DATA_SIZE SAMPLE_DEVICE_DATA_LNGTH + SAMPLE_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
int32_t SAMPLE_ReadData(uart_info_t* device, uint8_t* read_data, uint8_t data_length);
int32_t SAMPLE_CommandDevice(uart_info_t* device, uint8_t cmd, uint32_t payload);
int32_t SAMPLE_RequestHK(uart_info_t* device, SAMPLE_Device_HK_tlm_t* data);
int32_t SAMPLE_RequestData(uart_info_t* device, SAMPLE_Device_Data_tlm_t* data);


#endif /* _SAMPLE_DEVICE_H_ */
