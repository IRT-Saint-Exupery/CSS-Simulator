/*******************************************************************************
** File: generic_eps_device.h
**
** Purpose:
**   This is the header file for the GENERIC_EPS device.
**
*******************************************************************************/
#ifndef _GENERIC_EPS_DEVICE_H_
#define _GENERIC_EPS_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "generic_eps_platform_cfg.h"

/*
** GENERIC_EPS device switch telemetry definition
*/
typedef struct
{
    uint16_t Voltage;
    uint16_t Current;
    uint16_t Status;

} __attribute__((packed)) GENERIC_EPS_Switch_tlm_t;


/*
** GENERIC_EPS device housekeeping telemetry definition
*/
typedef struct
{
    uint16_t  BatteryVoltage;
    uint16_t  BatteryTemperature;
    uint16_t  Bus3p3Voltage;
    uint16_t  Bus5p0Voltage;
    uint16_t  Bus12Voltage;
    uint16_t  EPSTemperature;
    uint16_t  SolarArrayVoltage;
    uint16_t  SolarArrayTemperature;
    GENERIC_EPS_Switch_tlm_t  Switch[8];

} __attribute__((packed)) GENERIC_EPS_Device_HK_tlm_t;
#define GENERIC_EPS_DEVICE_HK_LEN sizeof ( GENERIC_EPS_Device_HK_tlm_t )


/*
** Prototypes
*/
uint8_t GENERIC_EPS_CRC8(uint8_t* payload, uint32_t length);
int32_t GENERIC_EPS_CommandDevice(i2c_bus_info_t* device, uint8_t reg, uint8_t value);
int32_t GENERIC_EPS_RequestHK(i2c_bus_info_t* device, GENERIC_EPS_Device_HK_tlm_t* data);
int32_t GENERIC_EPS_CommandSwitch(i2c_bus_info_t* device, uint8_t switch_num, uint8_t value, GENERIC_EPS_Device_HK_tlm_t* data);

#endif /* _GENERIC_EPS_DEVICE_H_ */
