/******************************************************************************/
/** \file  ci_platform_cfg.h
*
*   Copyright 2017 United States Government as represented by the Administrator
*   of the National Aeronautics and Space Administration.  No copyright is
*   claimed in the United States under Title 17, U.S. Code.
*   All Other Rights Reserved.
*
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Sample config file for CI Application with UDP TCTF with COP1
*
*   \par Limitations, Assumptions, External Events, and Notes:
*       - Make use of the setup.sh script to copy / link this file to the 
*       {MISSION_HOME}/apps/to/fsw/platform_inc folder.
*
*   \par Modification History:
*     - 2015-08-01 | Guy de Carufel | Code Started
*******************************************************************************/
#ifndef _CI_PLATFORM_CFG_H_
#define _CI_PLATFORM_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
** Pragmas
*/

/*
** Local Defines
*/

//#define CI_CUSTOM_DEBUG

/* Check new commands every 10Hz if not scheduled */
#define CI_WAKEUP_TIMEOUT  50

#define CI_SCH_PIPE_DEPTH  10
#define CI_CMD_PIPE_DEPTH  10
#define CI_TLM_PIPE_DEPTH  10

#define CI_CUSTOM_UDP_PORT 5010
#define CI_CUSTOM_UDP_ADDR IO_TRANS_UDP_INADDR_ANY
#define CI_CUSTOM_UDP_TIMEOUT 100
#define CI_CUSTOM_MAX_IP_STRING_SIZE  16 

#define CI_CUSTOM_CLTU_RANDOM_UDP    0

#define CI_CUSTOM_CLTU_BUFF_SIZE 1000
#define CI_CUSTOM_TF_BUFF_SIZE 1000
#define CI_CUSTOM_BUFFER_SIZE 1000

#define CI_CUSTOM_TASK_STACK_PTR NULL
#define CI_CUSTOM_TASK_STACK_SIZE 0x4000
#define CI_CUSTOM_TASK_PRIO 118

#define CI_CUSTOM_TF_CHANNELS 1

/*
** Include Files
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/

#ifdef __cplusplus
}
#endif

#endif /* _CI_PLATFORM_CFG_H_ */

/*==============================================================================
** End of file ci_platform_cfg.h
**============================================================================*/
    
