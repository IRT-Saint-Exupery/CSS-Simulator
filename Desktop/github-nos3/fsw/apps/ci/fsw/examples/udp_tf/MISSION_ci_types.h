/******************************************************************************/
/** \file  MISSION_ci_types.h
*
*   Copyright 2017 United States Government as represented by the Administrator
*   of the National Aeronautics and Space Administration.  No copyright is
*   claimed in the United States under Title 17, U.S. Code.
*   All Other Rights Reserved.
*
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Command and telemetry data strucutres for CI application
*
*   \par
*       This header file contains definitions of command and telemetry data
*       structures for CI applications for the UDP TCTF transport protocol example.
*
*   \par Limitations, Assumptions, External Events, and Notes:
*     - Make use of the setup.sh script to move / link this file to the
*     {MISSION_HOME}/apps/inc/ folder.
*     - Default HK Telemetry structure is defined in ci_hktlm.h
*
*   \par Modification History:
*     - 2015-01-09 | Guy de Carufel | Code Started
*     - 2015-10-16 | Guy de Carufel | Moved hktlm to ci_hktlm.h
*******************************************************************************/
#ifndef _MISSION_CI_TYPES_H_
#define _MISSION_CI_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfe.h"
#include "../ci/fsw/src/ci_hktlm.h"
#include "../to/fsw/mission_inc/to_mission_cfg.h"

#include "crypto.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/
typedef struct
{
    CFE_MSG_CommandHeader_t  ucCmdHeader;
} CI_NoArgCmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t  ucCmdHeader;
    char    cDestIp[TO_MAX_IP_STRING_SIZE];   /* Destination Socket IP */	
    uint16  usDestPort;                       /* Destination Socket Port */ 
} CI_EnableTOCmd_t;


typedef struct
{
    CFE_MSG_TelemetryHeader_t  ucTlmHeader;
} CI_OutData_t;


#ifdef __cplusplus
}
#endif

#endif /* _CI_TO_DEV_CI_TYPES_H_ */

/*==============================================================================
** End of file MISSION_ci_types.h
**============================================================================*/
    
