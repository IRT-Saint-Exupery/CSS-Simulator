/*******************************************************************************
** File: generic_thruster_app.h
**
** Purpose:
**   This is the main header file for the GENERIC_THRUSTER application.
**
*******************************************************************************/
#ifndef _GENERIC_THRUSTER_APP_H_
#define _GENERIC_THRUSTER_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "generic_thruster_events.h"
#include "generic_thruster_platform_cfg.h"
#include "generic_thruster_perfids.h"
#include "generic_thruster_msg.h"
#include "generic_thruster_msgids.h"
#include "generic_thruster_version.h"
#include "hwlib.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define GENERIC_THRUSTER_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define GENERIC_THRUSTER_DEVICE_DISABLED       0
#define GENERIC_THRUSTER_DEVICE_ENABLED        1


/*
** GENERIC_THRUSTER global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `generic_thruster_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    GENERIC_THRUSTER_Hk_tlm_t   HkTelemetryPkt;   /* GENERIC_THRUSTER Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    /* 
    ** Device protocol
    ** TODO: Make specific to your application
    */ 
    uart_info_t Generic_thrusterUart;             /* Hardware protocol definition */

} GENERIC_THRUSTER_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern GENERIC_THRUSTER_AppData_t GENERIC_THRUSTER_AppData; /* GENERIC_THRUSTER App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (GENERIC_THR_AppMain), these
**       functions are not called from any other source module.
*/
void  GENERIC_THR_AppMain(void);
int32 GENERIC_THRUSTER_AppInit(void);
void  GENERIC_THRUSTER_ProcessCommandPacket(void);
void  GENERIC_THRUSTER_ProcessGroundCommand(CFE_MSG_Message_t * Msg);
void  GENERIC_THRUSTER_ProcessTelemetryRequest(void);
void  GENERIC_THRUSTER_ReportHousekeeping(void);
void  GENERIC_THRUSTER_ReportDeviceTelemetry(void);
void  GENERIC_THRUSTER_ResetCounters(void);
void  GENERIC_THRUSTER_Enable(void);
void  GENERIC_THRUSTER_Disable(void);
void  GENERIC_THRUSTER_Percentage(GENERIC_THRUSTER_Percentage_cmd_t *Msg);
int32 GENERIC_THRUSTER_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

#endif /* _GENERIC_THRUSTER_APP_H_ */
