/*******************************************************************************
** File: novatel_oem615_app.h
**
** Purpose:
**   This is the main header file for the NOVATEL_OEM615 application.
**
*******************************************************************************/
#ifndef _NOVATEL_OEM615_APP_H_
#define _NOVATEL_OEM615_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "novatel_oem615_child.h"
#include "novatel_oem615_device.h"
#include "novatel_oem615_events.h"
#include "novatel_oem615_platform_cfg.h"
#include "novatel_oem615_perfids.h"
#include "novatel_oem615_msg.h"
#include "novatel_oem615_msgids.h"
#include "novatel_oem615_version.h"
#include "hwlib.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define NOVATEL_OEM615_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define NOVATEL_OEM615_DEVICE_DISABLED       0
#define NOVATEL_OEM615_DEVICE_ENABLED        1

#define NOVATEL_OEM615_CHILD_TASK_NAME       "NAV_CHILD_TASK"
#define NOVATEL_OEM615_CHILD_TASK_STACK_SIZE 2048
#define NOVATEL_OEM615_CHILD_TASK_PRIORITY   162

//#define NOVATEL_OEM615_MUTEX_NAME            "NOVATEL_OEM615_MUTEX"
#define NOVATEL_OEM615_HK_MUTEX_NAME         "NAV_HK_MUTEX"

/*
** NOVATEL_OEM615 global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `novatel_oem615_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    NOVATEL_OEM615_Hk_tlm_t   HkTelemetryPkt;   /* NOVATEL_OEM615 Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;                 /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;                    /* Pipe Id for HK command pipe */
    uint32 RunStatus;                           /* App run status for controlling the application state */
    uint32 HkDataMutex;                         /* Locks all data */

    /*
	** Device data 
	*/
    uint32 DeviceID;		                 /* Device ID provided by CFS on initialization */
    NOVATEL_OEM615_Device_tlm_t DevicePkt;      /* Device specific data packet */

    /* 
    ** Device protocol
    */ 
    uart_info_t Novatel_oem615Uart;             /* Hardware protocol definition */

} NOVATEL_OEM615_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern NOVATEL_OEM615_AppData_t NOVATEL_OEM615_AppData; /* NOVATEL_OEM615 App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (NOVATEL_AppMain), these
**       functions are not called from any other source module.
*/
void  NOVATEL_AppMain(void);
int32 NOVATEL_OEM615_AppInit(void);
void  NOVATEL_OEM615_ProcessCommandPacket(void);
void  NOVATEL_OEM615_ProcessGroundCommand(void);
void  NOVATEL_OEM615_ProcessTelemetryRequest(void);
void  NOVATEL_OEM615_ReportHousekeeping(void);
void  NOVATEL_OEM615_ReportDeviceTelemetry(void);
void  NOVATEL_OEM615_ResetCounters(void);
void  NOVATEL_OEM615_Enable(void);
void  NOVATEL_OEM615_Disable(void);
int32 NOVATEL_OEM615_GetRunStatus(void);
void NOVATEL_OEM615_SetRunStatus(uint32 status);
int32 NOVATEL_OEM615_GetDeviceEnabledStatus(void);
void NOVATEL_OEM615_SetDeviceEnabledStatus(uint32 status);
void NOVATEL_OEM615_IncrementDeviceCount(void);
void NOVATEL_OEM615_IncrementDeviceErrorCount(void);
void NOVATEL_OEM615_IncrementCommandCount(void);
void NOVATEL_OEM615_IncrementCommandErrorCount(void);
int32 NOVATEL_OEM615_SafeCommandDeviceCustom(uint8_t cmd_code, int8_t log_type, int8_t period_option);
int32 NOVATEL_OEM615_SafeRequestHK(NOVATEL_OEM615_Device_HK_tlm_t* data);
int32 NOVATEL_OEM615_SafeRequestData(NOVATEL_OEM615_Device_Data_tlm_t* data);
int32 NOVATEL_OEM615_ChildProcessRequestData(NOVATEL_OEM615_Device_Data_tlm_t* data);
int32 NOVATEL_OEM615_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

#endif /* _NOVATEL_OEM615_APP_H_ */
