/*******************************************************************************
** File:
**   ids_msg.h
**
** Purpose:
**  Define IDS application commands and telemetry messages
**
*******************************************************************************/
#ifndef _IDS_MSG_H_
#define _IDS_MSG_H_

#include "cfe.h"
#include "ids_device.h"


/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define IDS_NOOP_CC                 0
#define IDS_RESET_COUNTERS_CC       1
#define IDS_ENABLE_CC               2
#define IDS_DISABLE_CC              3
#define IDS_CONFIG_CC               4


/* 
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define IDS_REQ_HK_TLM              0
#define IDS_REQ_DATA_TLM            1


/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} IDS_NoArgs_cmd_t;


/*
** IDS write configuration command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint32   DeviceCfg;

} IDS_Config_cmd_t;


/*
** IDS device telemetry definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    IDS_Device_Data_tlm_t Ids;

} __attribute__((packed)) IDS_Device_tlm_t;
#define IDS_DEVICE_TLM_LNGTH sizeof ( IDS_Device_tlm_t )


/*
** IDS housekeeping type definition
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
    IDS_Device_HK_tlm_t DeviceHK;

} __attribute__((packed)) IDS_Hk_tlm_t;
#define IDS_HK_TLM_LNGTH sizeof ( IDS_Hk_tlm_t )

#endif /* _IDS_MSG_H_ */
