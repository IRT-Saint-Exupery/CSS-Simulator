/*******************************************************************************
** File:
**  generic_reaction_wheel_app.h
**
** Purpose:
**   This file is main hdr file for the Generic Reaction Wheel application.
**
**
*******************************************************************************/

#ifndef _generic_reaction_wheel_app_h_
#define _generic_reaction_wheel_app_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "generic_reaction_wheel_perfids.h"
#include "generic_reaction_wheel_msgids.h"
#include "generic_reaction_wheel_msg.h"

/***********************************************************************/
#define GENERIC_RW_PIPE_DEPTH                     32 /* Depth of the Command Pipe for Application */

/************************************************************************
** Type Definitions
*************************************************************************/

/*
** Global Data
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet...
    */
    GENERIC_RW_HkTlm_t  HkTlm;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t    CommandPipe;
    CFE_MSG_Message_t *    MsgPtr;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char     PipeName[16];
    uint16   PipeDepth;

    CFE_EVS_BinFilter_t  EventFilters[GENERIC_RW_EVENT_COUNTS];

} GENERIC_RW_AppData_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (RW_AppMain), these
**       functions are not called from any other source module.
*/
void  RW_AppMain(void);
int32 GENERIC_RW_AppInit(void);
void  GENERIC_RW_ProcessCommandPacket(CFE_MSG_Message_t * Msg);
void  GENERIC_RW_ProcessGroundCommand(CFE_MSG_Message_t * Msg);
int32 GENERIC_RW_ReportHousekeeping(void);
int32 GENERIC_RW_ResetCounters(const GENERIC_RW_ResetCounters_t *Msg);
int32 GENERIC_RW_Noop(const GENERIC_RW_Noop_t *Msg);
int32 GENERIC_RW_Current_Momentum(const GENERIC_RW_Noop_t *Msg );
int32 GENERIC_RW_Set_Torque(const GENERIC_RW_Cmd_t *Msg);

bool  GENERIC_RW_VerifyCmdLength(CFE_MSG_Message_t * Msg, uint16 ExpectedLength);

#endif /* _generic_reaction_wheel_app_h_ */
