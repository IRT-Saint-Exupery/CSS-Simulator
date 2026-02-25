/*******************************************************************************
** File:
**   generic_css_msg.h
**
** Purpose:
**  Define GENERIC_CSS application commands and telemetry messages
**
*******************************************************************************/
#ifndef _GENERIC_CSS_MSG_H_
#define _GENERIC_CSS_MSG_H_

#include "cfe.h"
#include "generic_css_device.h"


/*
** Ground Command Codes
*/
#define GENERIC_CSS_NOOP_CC                 0
#define GENERIC_CSS_RESET_COUNTERS_CC       1
#define GENERIC_CSS_ENABLE_CC               2
#define GENERIC_CSS_DISABLE_CC              3


/* 
** Telemetry Request Command Codes
*/
#define GENERIC_CSS_REQ_HK_TLM              0
#define GENERIC_CSS_REQ_DATA_TLM            1


/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} GENERIC_CSS_NoArgs_cmd_t;


/*
** GENERIC_CSS device telemetry definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    GENERIC_CSS_Device_Data_tlm_t Generic_css;

} __attribute__((packed)) GENERIC_CSS_Device_tlm_t;
#define GENERIC_CSS_DEVICE_TLM_LNGTH sizeof ( GENERIC_CSS_Device_tlm_t )


/*
** GENERIC_CSS housekeeping type definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8   CommandErrorCount;
    uint8   CommandCount;
    uint8   DeviceErrorCount;
    uint8   DeviceCount;
    uint8   DeviceEnabled;

} __attribute__((packed)) GENERIC_CSS_Hk_tlm_t;
#define GENERIC_CSS_HK_TLM_LNGTH sizeof ( GENERIC_CSS_Hk_tlm_t )

#endif /* _GENERIC_CSS_MSG_H_ */
