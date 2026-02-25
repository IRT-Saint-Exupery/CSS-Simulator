/*******************************************************************************
** File: ids_device.h
**
** Purpose:
**   This is the header file for the IDS device.
**
*******************************************************************************/
#ifndef _IDS_DEVICE_H_
#define _IDS_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "ids_platform_cfg.h"


/*
** Type definitions
** TODO: Make specific to your application
*/
#define IDS_DEVICE_HDR              0xDEAD
#define IDS_DEVICE_HDR_0            0xDE
#define IDS_DEVICE_HDR_1            0xAD

#define IDS_DEVICE_NOOP_CMD         0x00
#define IDS_DEVICE_REQ_HK_CMD       0x01
#define IDS_DEVICE_REQ_DATA_CMD     0x02
#define IDS_DEVICE_CFG_CMD          0x03

#define IDS_DEVICE_TRAILER          0xBEEF
#define IDS_DEVICE_TRAILER_0        0xBE
#define IDS_DEVICE_TRAILER_1        0xEF

#define IDS_DEVICE_HDR_TRL_LEN      4
#define IDS_DEVICE_CMD_SIZE         9

/*
** IDS device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint32_t  DeviceConfig;
    uint32_t  DeviceStatus;
    uint16_t  IdsStatus;
    uint16_t  PacketsDetected;
    uint16_t  BadCrc;
    uint16_t  SuccessiveBadCrc;

} __attribute__((packed)) IDS_Device_HK_tlm_t;
#define IDS_DEVICE_HK_LNGTH sizeof ( IDS_Device_HK_tlm_t )
#define IDS_DEVICE_HK_SIZE IDS_DEVICE_HK_LNGTH + IDS_DEVICE_HDR_TRL_LEN


/*
** IDS device data telemetry definition
*/
typedef struct
{
    uint8_t  Module;
    uint8_t  RuleNb;
    //uint8_t  PacketData[IDS_PDATA_SIZE];
    //uint16_t DataLen;
    char     Taskname[IDS_TSKNAME_SIZE];

} __attribute__((packed)) IDS_Device_Data_tlm_t;
#define IDS_DEVICE_DATA_LNGTH sizeof ( IDS_Device_Data_tlm_t )
#define IDS_DEVICE_DATA_SIZE IDS_DEVICE_DATA_LNGTH + IDS_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
int32_t IDS_ReadData(uart_info_t* device, uint8_t* read_data, uint8_t data_length);
int32_t IDS_CommandDevice(uart_info_t* device, uint8_t cmd, uint32_t payload);
int32_t IDS_RequestHK(uart_info_t* device, IDS_Device_HK_tlm_t* data);
int32_t IDS_RequestData(uart_info_t* device, IDS_Device_Data_tlm_t* data);


#endif /* _IDS_DEVICE_H_ */
