/*******************************************************************************
** File: generic_mag_device.h
**
** Purpose:
**   This is the header file for the GENERIC_MAG device.
**
*******************************************************************************/
#ifndef _GENERIC_MAG_DEVICE_H_
#define _GENERIC_MAG_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "generic_mag_platform_cfg.h"


/*
** Type definitions
*/

#define GENERIC_MAG_DEVICE_HDR_0            0xDE
#define GENERIC_MAG_DEVICE_HDR_1            0xAD
#define GENERIC_MAG_DEVICE_HDR_2            0xBE
#define GENERIC_MAG_DEVICE_HDR_3            0xEF
#define GENERIC_MAG_DEVICE_HDR_LEN          4

#define MAG_CONV_CONST                      21474
#define MAG_RANGE                           100000

/*
** GENERIC_MAG device data telemetry definition
*/
typedef struct
{
    int32_t  MagneticIntensityX;
    int32_t  MagneticIntensityY;
    int32_t  MagneticIntensityZ;
    
} __attribute__((packed)) GENERIC_MAG_Device_Data_tlm_t;
#define GENERIC_MAG_DEVICE_DATA_LNGTH sizeof ( GENERIC_MAG_Device_Data_tlm_t )
#define GENERIC_MAG_DEVICE_DATA_SIZE GENERIC_MAG_DEVICE_DATA_LNGTH + GENERIC_MAG_DEVICE_HDR_LEN


/*
** Prototypes
*/
int32_t GENERIC_MAG_RequestData(spi_info_t *device, GENERIC_MAG_Device_Data_tlm_t* data);


#endif /* _GENERIC_MAG_DEVICE_H_ */
