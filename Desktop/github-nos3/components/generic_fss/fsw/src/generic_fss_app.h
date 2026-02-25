/*******************************************************************************
** File: generic_fss_app.h
**
** Purpose:
**   This is the main header file for the GENERIC_FSS application.
**
*******************************************************************************/
#ifndef _GENERIC_FSS_APP_H_
#define _GENERIC_FSS_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "hwlib.h"
#include "generic_fss_msg.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define GENERIC_FSS_PIPE_DEPTH            50  // 32 IRT CSS

/*
** Enabled and Disabled Definitions
*/
#define GENERIC_FSS_DEVICE_DISABLED       0
#define GENERIC_FSS_DEVICE_ENABLED        1

/*
** GENERIC_FSS global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `generic_fss_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    GENERIC_FSS_Hk_tlm_t   HkTelemetryPkt;   /* GENERIC_FSS Housekeeping Telemetry Packet */
    
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
    GENERIC_FSS_Device_tlm_t DevicePkt;      /* Device specific data packet */

    /* 
    ** Device protocol
    */ 
    spi_info_t Generic_fssSpi;             /* Hardware protocol definition */

} GENERIC_FSS_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern GENERIC_FSS_AppData_t GENERIC_FSS_AppData; /* GENERIC_FSS App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (FSS_AppMain), these
**       functions are not called from any other source module.
*/
void  FSS_AppMain(void);

#endif /* _GENERIC_FSS_APP_H_ */
