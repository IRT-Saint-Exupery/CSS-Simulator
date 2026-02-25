/*******************************************************************************
** File:
**   novatel_oem615_msg.h
**
** Purpose:
**  Define NOVATEL_OEM615 application commands and telemetry messages
**
*******************************************************************************/
#ifndef _NOVATEL_OEM615_MSG_H_
#define _NOVATEL_OEM615_MSG_H_

#include "cfe.h"
#include "novatel_oem615_device.h"


/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define NOVATEL_OEM615_NOOP_CC                 0
#define NOVATEL_OEM615_RESET_COUNTERS_CC       1
#define NOVATEL_OEM615_ENABLE_CC               2
#define NOVATEL_OEM615_DISABLE_CC              3
#define NOVATEL_OEM615_LOG_CC                  4
#define NOVATEL_OEM615_UNLOG_CC                5
#define NOVATEL_OEM615_UNLOGALL_CC             6
#define NOVATEL_OEM615_SERIALCONFIG_CC         7

/* 
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define NOVATEL_OEM615_REQ_HK_TLM              0
#define NOVATEL_OEM615_REQ_DATA_TLM            1

/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} NOVATEL_OEM615_NoArgs_cmd_t;


/*
** NOVATEL_OEM615 write configuration command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint32   DeviceCfg;

} NOVATEL_OEM615_Config_cmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8    LogType;
    uint8    PeriodOption;

} NOVATEL_OEM615_Log_cmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8    LogType;

} NOVATEL_OEM615_Unlog_cmd_t;

/*
** NOVATEL_OEM615 device telemetry definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    NOVATEL_OEM615_Device_Data_tlm_t Novatel_oem615;

} __attribute__((packed)) NOVATEL_OEM615_Device_tlm_t;
#define NOVATEL_OEM615_DEVICE_TLM_LNGTH sizeof ( NOVATEL_OEM615_Device_tlm_t )


/*
** NOVATEL_OEM615 housekeeping type definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8   CommandErrorCount;
    uint8   CommandCount;
    uint8   DeviceErrorCount;
    uint8   DeviceCount;
    uint8   DeviceEnabled;
    NOVATEL_OEM615_Device_HK_tlm_t DeviceHK;

} __attribute__((packed)) NOVATEL_OEM615_Hk_tlm_t;
#define NOVATEL_OEM615_HK_TLM_LNGTH sizeof ( NOVATEL_OEM615_Hk_tlm_t )

#endif /* _NOVATEL_OEM615_MSG_H_ */
