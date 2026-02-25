/************************************************************************
** File:
**    ids_events.h
**
** Purpose:
**  Define IDS application event IDs
**
*************************************************************************/

#ifndef _IDS_EVENTS_H_
#define _IDS_EVENTS_H_

/* Standard app event IDs */
#define IDS_RESERVED_EID              0
#define IDS_STARTUP_INF_EID           1
#define IDS_LEN_ERR_EID               2
#define IDS_PIPE_ERR_EID              3
#define IDS_SUB_CMD_ERR_EID           4
#define IDS_SUB_REQ_HK_ERR_EID        5
#define IDS_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define IDS_CMD_ERR_EID               10
#define IDS_CMD_NOOP_INF_EID          11
#define IDS_CMD_RESET_INF_EID         12
#define IDS_CMD_ENABLE_INF_EID        13
#define IDS_ENABLE_INF_EID            14
#define IDS_ENABLE_ERR_EID            15
#define IDS_CMD_DISABLE_INF_EID       16
#define IDS_DISABLE_INF_EID           17
#define IDS_DISABLE_ERR_EID           18

/* IDS Child IDs */
#define IDS_CHILD_INIT_ERR_EID        19

/* Device specific command event IDs */
#define IDS_CMD_CONFIG_INF_EID        20

/* Standard telemetry event IDs */
#define IDS_DEVICE_TLM_ERR_EID        30
#define IDS_REQ_HK_ERR_EID            31

/* Device specific telemetry event IDs */
#define IDS_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define IDS_UART_INIT_ERR_EID         40
#define IDS_UART_CLOSE_ERR_EID        41

#endif /* _IDS_EVENTS_H_ */
