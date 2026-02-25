/************************************************************************
** File:
**    generic_torquer_events.h
**
** Purpose:
**  Define GENERIC_TORQUER application event IDs
**
*************************************************************************/

#ifndef _GENERIC_TORQUER_EVENTS_H_
#define _GENERIC_TORQUER_EVENTS_H_

/* Standard app event IDs */
#define GENERIC_TORQUER_RESERVED_EID              0
#define GENERIC_TORQUER_STARTUP_INF_EID           1
#define GENERIC_TORQUER_LEN_ERR_EID               2
#define GENERIC_TORQUER_PIPE_ERR_EID              3
#define GENERIC_TORQUER_SUB_CMD_ERR_EID           4
#define GENERIC_TORQUER_SUB_REQ_HK_ERR_EID        5
#define GENERIC_TORQUER_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define GENERIC_TORQUER_CMD_ERR_EID               10
#define GENERIC_TORQUER_CMD_NOOP_INF_EID          11
#define GENERIC_TORQUER_CMD_RESET_INF_EID         12
#define GENERIC_TORQUER_CMD_ENABLE_INF_EID        13
#define GENERIC_TORQUER_ENABLE_INF_EID            14
#define GENERIC_TORQUER_ENABLE_ERR_EID            15
#define GENERIC_TORQUER_CMD_DISABLE_INF_EID       16
#define GENERIC_TORQUER_DISABLE_INF_EID           17
#define GENERIC_TORQUER_DISABLE_ERR_EID           18

/* Device specific command event IDs */
#define GENERIC_TORQUER_CMD_CONFIG_INF_EID        20
#define GENERIC_TORQUER_CMD_CONFIG_ERR_EID        21
#define GENERIC_TORQUER_CMD_CONFIG_ALL_INF_EID    22
#define GENERIC_TORQUER_CMD_CONFIG_ALL_ERR_EID    23

/* Standard telemetry event IDs */
#define GENERIC_TORQUER_DEVICE_TLM_ERR_EID        30
#define GENERIC_TORQUER_REQ_HK_ERR_EID            31

/* Device specific telemetry event IDs */
/* None */

/* Hardware protocol event IDs */
#define GENERIC_TORQUER_INIT_ERR_EID              40
#define GENERIC_TORQUER_CONFIG_ERR_EID            41

#endif /* _GENERIC_TORQUER_EVENTS_H_ */
