/************************************************************************
** File:
**    generic_eps_events.h
**
** Purpose:
**  Define GENERIC_EPS application event IDs
**
*************************************************************************/

#ifndef _GENERIC_EPS_EVENTS_H_
#define _GENERIC_EPS_EVENTS_H_

/* Standard app event IDs */
#define GENERIC_EPS_RESERVED_EID              0
#define GENERIC_EPS_STARTUP_INF_EID           1
#define GENERIC_EPS_LEN_ERR_EID               2
#define GENERIC_EPS_PIPE_ERR_EID              3
#define GENERIC_EPS_SUB_CMD_ERR_EID           4
#define GENERIC_EPS_SUB_REQ_HK_ERR_EID        5
#define GENERIC_EPS_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define GENERIC_EPS_CMD_ERR_EID               10
#define GENERIC_EPS_CMD_NOOP_INF_EID          11
#define GENERIC_EPS_CMD_RESET_INF_EID         12

/* Device specific command event IDs */
#define GENERIC_EPS_CMD_SWITCH_INF_EID        20
#define GENERIC_EPS_SWITCH_INF_EID            21
#define GENERIC_EPS_SWITCH_ERR_EID            22

/* Standard telemetry event IDs */
#define GENERIC_EPS_DEVICE_TLM_ERR_EID        30
#define GENERIC_EPS_REQ_HK_ERR_EID            31

/* Hardware protocol event IDs */
#define GENERIC_EPS_I2C_INIT_ERR_EID          40

#endif /* _GENERIC_EPS_EVENTS_H_ */
