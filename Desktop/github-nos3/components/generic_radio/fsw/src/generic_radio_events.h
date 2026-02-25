/************************************************************************
** File:
**    generic_radio_events.h
**
** Purpose:
**  Define GENERIC_RADIO application event IDs
**
*************************************************************************/

#ifndef _GENERIC_RADIO_EVENTS_H_
#define _GENERIC_RADIO_EVENTS_H_

/* Standard app event IDs */
#define GENERIC_RADIO_RESERVED_EID              0
#define GENERIC_RADIO_STARTUP_INF_EID           1
#define GENERIC_RADIO_LEN_ERR_EID               2
#define GENERIC_RADIO_PIPE_ERR_EID              3
#define GENERIC_RADIO_SUB_CMD_ERR_EID           4
#define GENERIC_RADIO_SUB_REQ_HK_ERR_EID        5
#define GENERIC_RADIO_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define GENERIC_RADIO_CMD_ERR_EID               10
#define GENERIC_RADIO_CMD_NOOP_INF_EID          11
#define GENERIC_RADIO_CMD_RESET_INF_EID         12

/* Device specific command event IDs */
#define GENERIC_RADIO_CMD_CONFIG_INF_EID        20

/* Standard telemetry event IDs */
#define GENERIC_RADIO_DEVICE_TLM_ERR_EID        30
#define GENERIC_RADIO_REQ_HK_ERR_EID            31

/* Device specific telemetry event IDs */

/* Hardware protocol event IDs */
#define GENERIC_RADIO_SOCK_OPEN_ERR_EID         40
#define GENERIC_RADIO_PROX_OPEN_ERR_EID         41

/* Device task even IDs */
#define GENERIC_RADIO_TASK_REG_ERR_EID          50
#define GENERIC_RADIO_TASK_REG_INF_EID          51


#endif /* _GENERIC_RADIO_EVENTS_H_ */
