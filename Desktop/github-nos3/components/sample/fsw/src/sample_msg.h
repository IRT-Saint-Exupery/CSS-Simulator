/*******************************************************************************
** File:
**   sample_msg.h
**
** Purpose:
**  Define SAMPLE application commands and telemetry messages
**
*******************************************************************************/
#ifndef _SAMPLE_MSG_H_
#define _SAMPLE_MSG_H_

#include "cfe.h"
#include "sample_device.h"


/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define SAMPLE_NOOP_CC                 0
#define SAMPLE_RESET_COUNTERS_CC       1
#define SAMPLE_ENABLE_CC               2
#define SAMPLE_DISABLE_CC              3
#define SAMPLE_CONFIG_CC               4


/* 
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define SAMPLE_REQ_HK_TLM              0
#define SAMPLE_REQ_DATA_TLM            1


/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} SAMPLE_NoArgs_cmd_t;


/*
** SAMPLE write configuration command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint32   DeviceCfg;

} SAMPLE_Config_cmd_t;


/*
** SAMPLE device telemetry definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    SAMPLE_Device_Data_tlm_t Sample;

} __attribute__((packed)) SAMPLE_Device_tlm_t;
#define SAMPLE_DEVICE_TLM_LNGTH sizeof ( SAMPLE_Device_tlm_t )


/*
** SAMPLE housekeeping type definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8   CommandErrorCount;
    uint8   CommandCount;
    uint8   DeviceErrorCount;
    uint8   DeviceCount;
  
    /*
    ** TODO: Edit and add specific telemetry values to this struct
    */
    uint8   DeviceEnabled;
    SAMPLE_Device_HK_tlm_t DeviceHK;

} __attribute__((packed)) SAMPLE_Hk_tlm_t;
#define SAMPLE_HK_TLM_LNGTH sizeof ( SAMPLE_Hk_tlm_t )

#endif /* _SAMPLE_MSG_H_ */
