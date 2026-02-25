/*******************************************************************************
** File:
**   isl_msg.h
**
** Purpose:
**  Define ISL application commands and telemetry messages
**
*******************************************************************************/
#ifndef _ISL_MSG_H_
#define _ISL_MSG_H_

#include "cfe.h"
#include "isl_device.h"


/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define ISL_NOOP_CC                 0
#define ISL_RESET_COUNTERS_CC       1
#define ISL_ENABLE_CC               2
#define ISL_DISABLE_CC              3
#define ISL_CONFIG_CC               4


/* 
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define ISL_REQ_HK_TLM              0
#define ISL_REQ_DATA_TLM            1

/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} ISL_NoArgs_cmd_t;


/*
** ISL write configuration command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint32   DeviceCfg;
    char     RouteFilename[64];

} ISL_Config_cmd_t;


/*
** ISL device telemetry definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    ISL_Device_Data_tlm_t Isl;

} __attribute__((packed)) ISL_Device_tlm_t;
#define ISL_DEVICE_TLM_LNGTH sizeof ( ISL_Device_tlm_t )


/*
** ISL housekeeping type definition
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
    ISL_Device_HK_tlm_t DeviceHK;

} __attribute__((packed)) ISL_Hk_tlm_t;
#define ISL_HK_TLM_LNGTH sizeof ( ISL_Hk_tlm_t )

#endif /* _ISL_MSG_H_ */
