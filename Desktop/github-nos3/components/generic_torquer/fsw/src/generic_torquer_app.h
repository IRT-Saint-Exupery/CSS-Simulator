/*******************************************************************************
** File: generic_torquer_app.h
**
** Purpose:
**   This is the main header file for the GENERIC_TORQUER application.
**
*******************************************************************************/
#ifndef _GENERIC_TORQUER_APP_H_
#define _GENERIC_TORQUER_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "generic_torquer_events.h"
#include "generic_torquer_platform_cfg.h"
#include "generic_torquer_perfids.h"
#include "generic_torquer_msg.h"
#include "generic_torquer_msgids.h"
#include "generic_torquer_version.h"
#include "hwlib.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define GENERIC_TORQUER_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define GENERIC_TORQUER_DEVICE_DISABLED       0
#define GENERIC_TORQUER_DEVICE_ENABLED        1


/*
** GENERIC_TORQUER global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `generic_torquer_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    GENERIC_TORQUER_Hk_tlm_t   HkTelemetryPkt;   /* GENERIC_TORQUER Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    /* 
    ** Device protocol
    */ 
    trq_info_t trqDevice[3];             /* Hardware protocol definition */

} GENERIC_TORQUER_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern GENERIC_TORQUER_AppData_t GENERIC_TORQUER_AppData; /* GENERIC_TORQUER App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (TORQUER_AppMain), these
**       functions are not called from any other source module.
*/
void  TORQUER_AppMain(void);
int32 GENERIC_TORQUER_AppInit(void);
void  GENERIC_TORQUER_ProcessCommandPacket(void);
void  GENERIC_TORQUER_ProcessGroundCommand(void);
void  GENERIC_TORQUER_ProcessTelemetryRequest(void);
void  GENERIC_TORQUER_ReportHousekeeping(void);
void  GENERIC_TORQUER_ReportDeviceTelemetry(void);
void  GENERIC_TORQUER_ResetCounters(void);
void  GENERIC_TORQUER_Enable(void);
void  GENERIC_TORQUER_Disable(void);
int32 GENERIC_TORQUER_Config(uint8 num, uint8 percent, uint8 dir);
int32 GENERIC_TORQUER_PercentOn(CFE_MSG_Message_t * msg);
int32 GENERIC_TORQUER_AllPercentOn(CFE_MSG_Message_t * msg);
int32 GENERIC_TORQUER_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

#endif /* _GENERIC_TORQUER_APP_H_ */
