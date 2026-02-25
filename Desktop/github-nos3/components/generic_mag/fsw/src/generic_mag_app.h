/*******************************************************************************
** File: generic_mag_app.h
**
** Purpose:
**   This is the main header file for the GENERIC_MAG application.
**
*******************************************************************************/
#ifndef _GENERIC_MAG_APP_H_
#define _GENERIC_MAG_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "generic_mag_device.h"
#include "generic_mag_events.h"
#include "generic_mag_platform_cfg.h"
#include "generic_mag_perfids.h"
#include "generic_mag_msg.h"
#include "generic_mag_msgids.h"
#include "generic_mag_version.h"
#include "hwlib.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define GENERIC_MAG_PIPE_DEPTH            50 //default = 32 //IRT CSS


/*
** Enabled and Disabled Definitions
*/
#define GENERIC_MAG_DEVICE_DISABLED       0
#define GENERIC_MAG_DEVICE_ENABLED        1


/*
** GENERIC_MAG global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `generic_mag_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    GENERIC_MAG_Hk_tlm_t   HkTelemetryPkt;   /* GENERIC_MAG Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    /*
	** Device data 
	*/
	uint32 DeviceID;		            /* Device ID provided by CFS on initialization */
    GENERIC_MAG_Device_tlm_t DevicePkt;      /* Device specific data packet */

    /* 
    ** Device protocol
    ** TODO: Make specific to your application
    */ 
    spi_info_t Generic_magSpi;             /* Hardware protocol definition */

} GENERIC_MAG_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern GENERIC_MAG_AppData_t GENERIC_MAG_AppData; /* GENERIC_MAG App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (MAG_AppMain), these
**       functions are not called from any other source module.
*/
void  MAG_AppMain(void);
int32 GENERIC_MAG_AppInit(void);
void  GENERIC_MAG_ProcessCommandPacket(void);
void  GENERIC_MAG_ProcessGroundCommand(void);
void  GENERIC_MAG_ProcessTelemetryRequest(void);
void  GENERIC_MAG_ReportHousekeeping(void);
void  GENERIC_MAG_ReportDeviceTelemetry(void);
void  GENERIC_MAG_ResetCounters(void);
void  GENERIC_MAG_Enable(void);
void  GENERIC_MAG_Disable(void);
int32 GENERIC_MAG_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

#endif /* _GENERIC_MAG_APP_H_ */
