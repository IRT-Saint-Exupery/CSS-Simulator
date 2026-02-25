/*******************************************************************************
** File: sample_app.h
**
** Purpose:
**   This is the main header file for the SAMPLE application.
**
*******************************************************************************/
#ifndef _SAMPLE_APP_H_
#define _SAMPLE_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "sample_device.h"
#include "sample_events.h"
#include "sample_platform_cfg.h"
#include "sample_perfids.h"
#include "sample_msg.h"
#include "sample_msgids.h"
#include "sample_version.h"
#include "hwlib.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define SAMPLE_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define SAMPLE_DEVICE_DISABLED       0
#define SAMPLE_DEVICE_ENABLED        1


/*
** SAMPLE global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `sample_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    SAMPLE_Hk_tlm_t   HkTelemetryPkt;   /* SAMPLE Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    /*
	** Device data 
    ** TODO: Make specific to your application
	*/
    SAMPLE_Device_tlm_t DevicePkt;      /* Device specific data packet */

    /* 
    ** Device protocol
    ** TODO: Make specific to your application
    */ 
    uart_info_t SampleUart;             /* Hardware protocol definition */

} SAMPLE_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern SAMPLE_AppData_t SAMPLE_AppData; /* SAMPLE App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (SAMPLE_AppMain), these
**       functions are not called from any other source module.
*/
void  SAMPLE_AppMain(void);
int32 SAMPLE_AppInit(void);
void  SAMPLE_ProcessCommandPacket(void);
void  SAMPLE_ProcessGroundCommand(void);
void  SAMPLE_ProcessTelemetryRequest(void);
void  SAMPLE_ReportHousekeeping(void);
void  SAMPLE_ReportDeviceTelemetry(void);
void  SAMPLE_ResetCounters(void);
void  SAMPLE_Enable(void);
void  SAMPLE_Disable(void);
int32 SAMPLE_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

#endif /* _SAMPLE_APP_H_ */
