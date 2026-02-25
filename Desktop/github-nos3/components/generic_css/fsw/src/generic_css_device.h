/*******************************************************************************
** File: generic_css_device.h
**
** Purpose:
**   This is the header file for the GENERIC_CSS device.
**
*******************************************************************************/
#ifndef _GENERIC_CSS_DEVICE_H_
#define _GENERIC_CSS_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "generic_css_platform_cfg.h"


/*
** Type definitions
*/
#define GENERIC_CSS_I2C_ADDRESS             0x40
#define GENERIC_CSS_NUM_CHANNELS            6


/*
** GENERIC_CSS device data telemetry definition
*/
typedef struct
{
    uint16_t Voltage[GENERIC_CSS_NUM_CHANNELS];
} __attribute__((packed)) GENERIC_CSS_Device_Data_tlm_t;
#define GENERIC_CSS_DEVICE_DATA_LNGTH sizeof ( GENERIC_CSS_Device_Data_tlm_t )


/*
** Prototypes
*/
int32_t GENERIC_CSS_RequestData(i2c_bus_info_t* device, GENERIC_CSS_Device_Data_tlm_t* data);

#endif /* _GENERIC_CSS_DEVICE_H_ */
