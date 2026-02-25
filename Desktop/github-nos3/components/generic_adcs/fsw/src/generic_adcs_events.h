/************************************************************************
** Purpose:
**  Define GENERIC_ADCS application event IDs
**
*************************************************************************/

#ifndef _GENERIC_ADCS_EVENTS_H_
#define _GENERIC_ADCS_EVENTS_H_

/* Standard app event IDs */
#define GENERIC_ADCS_RESERVED_EID                    0
#define GENERIC_ADCS_STARTUP_INF_EID                 1
#define GENERIC_ADCS_LEN_ERR_EID                     2
#define GENERIC_ADCS_PIPE_ERR_EID                    3
#define GENERIC_ADCS_SUB_CMD_ERR_EID                 4
#define GENERIC_ADCS_SUB_REQ_HK_ERR_EID              5
#define GENERIC_ADCS_PROCESS_CMD_ERR_EID             6
#define GENERIC_ADCS_FOPEN_ERR_EID                   7

/* Standard command event IDs */
#define GENERIC_ADCS_CMD_ERR_EID                     10
#define GENERIC_ADCS_CMD_NOOP_INF_EID                11
#define GENERIC_ADCS_CMD_RESET_INF_EID               12

#define GENERIC_ADCS_SET_MODE_INF_EID                20
#define GENERIC_ADCS_SET_MOMENTUM_MANAGEMENT_INF_EID 21
#define GENERIC_ADCS_SET_TARGET_INF_EID              22

#endif
