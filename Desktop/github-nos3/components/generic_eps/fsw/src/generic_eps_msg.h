/*******************************************************************************
** File:
**   generic_eps_msg.h
**
** Purpose:
**  Define GENERIC_EPS application commands and telemetry messages
**
*******************************************************************************/
#ifndef _GENERIC_EPS_MSG_H_
#define _GENERIC_EPS_MSG_H_

#include "cfe.h"
#include "generic_eps_device.h"


/*
** Ground Command Codes
*/
#define GENERIC_EPS_NOOP_CC                 0
#define GENERIC_EPS_RESET_COUNTERS_CC       1
#define GENERIC_EPS_SWITCH_CC               2

/* 
** Telemetry Request Command Codes
*/
#define GENERIC_EPS_REQ_HK_TLM              0


/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} GENERIC_EPS_NoArgs_cmd_t;


/*
** GENERIC_EPS write configuration command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8    SwitchNumber;
    uint8    State;

} GENERIC_EPS_Switch_cmd_t;


/*
** GENERIC_EPS housekeeping type definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8   CommandErrorCount;
    uint8   CommandCount;
    uint8   DeviceErrorCount;
    uint8   DeviceCount;
    GENERIC_EPS_Device_HK_tlm_t DeviceHK;

} __attribute__((packed)) GENERIC_EPS_Hk_tlm_t;
#define GENERIC_EPS_HK_TLM_LNGTH sizeof ( GENERIC_EPS_Hk_tlm_t )

#endif /* _GENERIC_EPS_MSG_H_ */
