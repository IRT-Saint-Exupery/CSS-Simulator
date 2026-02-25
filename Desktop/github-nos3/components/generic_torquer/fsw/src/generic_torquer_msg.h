/*******************************************************************************
** File:
**   generic_torquer_msg.h
**
** Purpose:
**  Define GENERIC_TORQUER application commands and telemetry messages
**
*******************************************************************************/
#ifndef _GENERIC_TORQUER_MSG_H_
#define _GENERIC_TORQUER_MSG_H_

#include "cfe.h"


/*
** Ground Command Codes
*/
#define GENERIC_TORQUER_NOOP_CC                 0
#define GENERIC_TORQUER_RESET_COUNTERS_CC       1
#define GENERIC_TORQUER_ENABLE_CC               2
#define GENERIC_TORQUER_DISABLE_CC              3
#define GENERIC_TORQUER_CONFIG_CC               4
#define GENERIC_TORQUER_CONFIG_ALL_CC           5


/* 
** Telemetry Request Command Codes
*/
#define GENERIC_TORQUER_REQ_HK_TLM              0


/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} GENERIC_TORQUER_NoArgs_cmd_t;


/*
** Percent on command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8   TrqNum;
    uint8   Direction;
    uint8   PercentOn;

} __attribute__((packed)) GENERIC_TORQUER_Percent_On_cmd_t;
#define GENERIC_TORQUER_PERCENT_ON_CMD_LEN sizeof(GENERIC_TORQUER_Percent_On_cmd_t)


/*
** All percent on command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8   Direction_0;
    uint8   PercentOn_0;
    uint8   Direction_1;
    uint8   PercentOn_1;
    uint8   Direction_2;
    uint8   PercentOn_2;

} __attribute__((packed)) GENERIC_TORQUER_All_Percent_On_cmd_t;
#define GENERIC_TORQUER_ALL_PERCENT_ON_CMD_LEN sizeof(GENERIC_TORQUER_All_Percent_On_cmd_t)


/*
** GENERIC_TORQUER device telemetry definition
*/
typedef struct 
{
    uint8   Direction;
    uint8   PercentOn;
    
} __attribute__((packed)) GENERIC_TORQUER_Device_tlm_t;
#define GENERIC_TORQUER_DEVICE_TLM_LNGTH sizeof ( GENERIC_TORQUER_Device_tlm_t )


/*
** GENERIC_TORQUER housekeeping type definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8   CommandErrorCount;
    uint8   CommandCount;
    uint8   DeviceErrorCount;
    uint8   DeviceCount;
    uint8   DeviceEnabled;
    uint32  TorquerPeriod;
    GENERIC_TORQUER_Device_tlm_t TrqInfo[3];

} __attribute__((packed)) GENERIC_TORQUER_Hk_tlm_t;
#define GENERIC_TORQUER_HK_TLM_LNGTH sizeof ( GENERIC_TORQUER_Hk_tlm_t )

#endif /* _GENERIC_TORQUER_MSG_H_ */
