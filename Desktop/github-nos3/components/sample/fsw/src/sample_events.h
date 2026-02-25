/************************************************************************
** File:
**    sample_events.h
**
** Purpose:
**  Define SAMPLE application event IDs
**
*************************************************************************/

#ifndef _SAMPLE_EVENTS_H_
#define _SAMPLE_EVENTS_H_

/* Standard app event IDs */
#define SAMPLE_RESERVED_EID              0
#define SAMPLE_STARTUP_INF_EID           1
#define SAMPLE_LEN_ERR_EID               2
#define SAMPLE_PIPE_ERR_EID              3
#define SAMPLE_SUB_CMD_ERR_EID           4
#define SAMPLE_SUB_REQ_HK_ERR_EID        5
#define SAMPLE_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define SAMPLE_CMD_ERR_EID               10
#define SAMPLE_CMD_NOOP_INF_EID          11
#define SAMPLE_CMD_RESET_INF_EID         12
#define SAMPLE_CMD_ENABLE_INF_EID        13
#define SAMPLE_ENABLE_INF_EID            14
#define SAMPLE_ENABLE_ERR_EID            15
#define SAMPLE_CMD_DISABLE_INF_EID       16
#define SAMPLE_DISABLE_INF_EID           17
#define SAMPLE_DISABLE_ERR_EID           18

/* Device specific command event IDs */
#define SAMPLE_CMD_CONFIG_INF_EID        20

/* Standard telemetry event IDs */
#define SAMPLE_DEVICE_TLM_ERR_EID        30
#define SAMPLE_REQ_HK_ERR_EID            31

/* Device specific telemetry event IDs */
#define SAMPLE_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define SAMPLE_UART_INIT_ERR_EID         40
#define SAMPLE_UART_CLOSE_ERR_EID        41

#endif /* _SAMPLE_EVENTS_H_ */
