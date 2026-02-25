/************************************************************************
** File:
**    isl_events.h
**
** Purpose:
**  Define ISL application event IDs
**
*************************************************************************/

#ifndef _ISL_EVENTS_H_
#define _ISL_EVENTS_H_

/* Standard app event IDs */
#define ISL_RESERVED_EID              0
#define ISL_STARTUP_INF_EID           1
#define ISL_LEN_ERR_EID               2
#define ISL_PIPE_ERR_EID              3
#define ISL_SUB_CMD_ERR_EID           4
#define ISL_SUB_REQ_HK_ERR_EID        5
#define ISL_PROCESS_CMD_ERR_EID       6
#define ISL_SUB_REQ_WD_ERR_EID        7

/* Standard command event IDs */
#define ISL_CMD_ERR_EID               10
#define ISL_CMD_NOOP_INF_EID          11
#define ISL_CMD_RESET_INF_EID         12
#define ISL_CMD_ENABLE_INF_EID        13
#define ISL_ENABLE_INF_EID            14
#define ISL_ENABLE_ERR_EID            15
#define ISL_CMD_DISABLE_INF_EID       16
#define ISL_DISABLE_INF_EID           17
#define ISL_DISABLE_ERR_EID           18

/* Device specific command event IDs */
#define ISL_CMD_CONFIG_INF_EID        20

/* Standard telemetry event IDs */
#define ISL_DEVICE_TLM_ERR_EID        30
#define ISL_REQ_HK_ERR_EID            31

/* Device specific telemetry event IDs */
#define ISL_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define ISL_UART_INIT_ERR_EID         40
#define ISL_UART_CLOSE_ERR_EID        41

/* Hardware protocol event IDs */
#define ISL_ERR_EID         50
#define ISL_WARN_EID        51
#define ISL_INFO_EID        52


/* Device task even IDs */
#define ISL_TASK_REG_ERR_EID          60
#define ISL_TASK_REG_INF_EID          61
#define ISL_TASK_CREATE_EID           61


#endif /* _ISL_EVENTS_H_ */
