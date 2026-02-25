/*******************************************************************************
** File: generic_radio_device.h
**
** Purpose:
**   This is the header file for the GENERIC_RADIO device.
**
*******************************************************************************/
#ifndef _GENERIC_RADIO_DEVICE_H_
#define _GENERIC_RADIO_DEVICE_H_

/*
** Required header files.
*/
#include "cfe_endian.h"
#include "device_cfg.h"
#include "hwlib.h"
#include "generic_radio_platform_cfg.h"


/*
** Type definitions
*/
#define GENERIC_RADIO_DEVICE_HDR_0            0xDE
#define GENERIC_RADIO_DEVICE_HDR_1            0xAD

#define GENERIC_RADIO_DEVICE_REQ_HK_CMD       0x00
#define GENERIC_RADIO_DEVICE_CFG_CMD          0x01

#define GENERIC_RADIO_DEVICE_TRAILER_0        0xBE
#define GENERIC_RADIO_DEVICE_TRAILER_1        0xEF

#define GENERIC_RADIO_DEVICE_HDR_TRL_LEN      4
#define GENERIC_RADIO_DEVICE_CMD_SIZE         9

/*
** GENERIC_RADIO device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint32_t  DeviceConfig;
    uint32_t  ProxSignal;

} __attribute__((packed)) GENERIC_RADIO_Device_HK_tlm_t;
#define GENERIC_RADIO_DEVICE_HK_LNGTH sizeof ( GENERIC_RADIO_Device_HK_tlm_t )
#define GENERIC_RADIO_DEVICE_HK_SIZE GENERIC_RADIO_DEVICE_HK_LNGTH + GENERIC_RADIO_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
int32_t GENERIC_RADIO_SetConfiguration(socket_info_t* device, uint32_t config);
int32_t GENERIC_RADIO_ProximityForward(socket_info_t* device, uint16_t scid, uint8_t* data, uint16_t data_len);
int32_t GENERIC_RADIO_RequestHK(socket_info_t* device, GENERIC_RADIO_Device_HK_tlm_t* data);

#endif /* _GENERIC_RADIO_DEVICE_H_ */
