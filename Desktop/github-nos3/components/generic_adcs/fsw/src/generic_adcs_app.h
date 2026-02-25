/*******************************************************************************
** Purpose:
**   This is the main header file for the GENERIC_ADCS application.
**
*******************************************************************************/
#ifndef _GENERIC_ADCS_APP_H_
#define _GENERIC_ADCS_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "generic_torquer_msg.h"
#include "generic_reaction_wheel_msg.h"
#include "generic_adcs_msg.h"

/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define GENERIC_ADCS_PIPE_DEPTH            32

/*
** GENERIC_ADCS global data structure
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
    Generic_ADCS_Hk_tlm_t   HkTelemetryPkt;   /* GENERIC_ADCS Housekeeping Telemetry Packet */
    Generic_ADCS_DI_Tlm_t   DIPacket;
    Generic_ADCS_AD_Tlm_t   ADPacket;
    Generic_ADCS_GNC_Tlm_t  GNCPacket;
    Generic_ADCS_AC_Tlm_t   ACSPacket;
    Generic_ADCS_DO_Tlm_t   DOPacket;
    
    /*
    ** Command packets to other apps
    */
    GENERIC_TORQUER_All_Percent_On_cmd_t MtbPctOnCmd;
    GENERIC_RW_Cmd_t                     RwCmd;

    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

} Generic_ADCS_AppData_t;

/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern Generic_ADCS_AppData_t Generic_ADCS_AppData; /* GENERIC_ADCS App Data */

/*
**
** Local function prototypes.
**
** Note: Except for the entry point (ADCS_AppMain), these
**       functions are not called from any other source module.
*/
void  ADCS_AppMain(void);

#endif

