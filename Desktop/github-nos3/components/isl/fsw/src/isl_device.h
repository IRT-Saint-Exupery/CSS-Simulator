/*******************************************************************************
** File: isl_device.h
**
** Purpose:
**   This is the header file for the ISL device.
**
*******************************************************************************/
#ifndef _ISL_DEVICE_H_
#define _ISL_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "isl_platform_cfg.h"


/*
** Type definitions
** TODO: Make specific to your application
*/
#define ISL_DEVICE_HDR              0xDEAD
#define ISL_DEVICE_HDR_0            0xDE
#define ISL_DEVICE_HDR_1            0xAD

#define ISL_DEVICE_NOOP_CMD         0x00
#define ISL_DEVICE_REQ_HK_CMD       0x01
#define ISL_DEVICE_REQ_DATA_CMD     0x02
#define ISL_DEVICE_CFG_CMD          0x03

#define ISL_DEVICE_TRAILER          0xBEEF
#define ISL_DEVICE_TRAILER_0        0xBE
#define ISL_DEVICE_TRAILER_1        0xEF

#define ISL_DEVICE_HDR_TRL_LEN      4
#define ISL_DEVICE_CMD_SIZE         9

/*
** ISL device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint32_t  DeviceConfig;
    uint32_t  DeviceStatus;
    uint32_t  DropPktCounter; // IRT CSS
    

} __attribute__((packed)) ISL_Device_HK_tlm_t;
#define ISL_DEVICE_HK_LNGTH sizeof ( ISL_Device_HK_tlm_t )
#define ISL_DEVICE_HK_SIZE ISL_DEVICE_HK_LNGTH + ISL_DEVICE_HDR_TRL_LEN


/*
** ISL device data telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    //uint32_t  DropPktCounter; // IRT CSS


} __attribute__((packed)) ISL_Device_Data_tlm_t;
#define ISL_DEVICE_DATA_LNGTH sizeof ( ISL_Device_Data_tlm_t )
#define ISL_DEVICE_DATA_SIZE ISL_DEVICE_DATA_LNGTH + ISL_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
int32_t ISL_ReadData(uart_info_t* device, uint8_t* read_data, uint8_t data_length);
int32_t ISL_CommandDevice(uart_info_t* device, uint8_t cmd, uint32_t payload);
int32_t ISL_RequestHK(uart_info_t* device, ISL_Device_HK_tlm_t* data);
int32_t ISL_RequestData(uart_info_t* device, ISL_Device_Data_tlm_t* data);


#endif /* _ISL_DEVICE_H_ */
