/************************************************************************
** File:
**    generic_thruster_events.h
**
** Purpose:
**  Define GENERIC_THRUSTER application event IDs
**
*************************************************************************/

#ifndef _GENERIC_THRUSTER_EVENTS_H_
#define _GENERIC_THRUSTER_EVENTS_H_

/* Standard app event IDs */
#define GENERIC_THRUSTER_RESERVED_EID              0
#define GENERIC_THRUSTER_STARTUP_INF_EID           1
#define GENERIC_THRUSTER_LEN_ERR_EID               2
#define GENERIC_THRUSTER_PIPE_ERR_EID              3
#define GENERIC_THRUSTER_SUB_CMD_ERR_EID           4
#define GENERIC_THRUSTER_SUB_REQ_HK_ERR_EID        5
#define GENERIC_THRUSTER_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define GENERIC_THRUSTER_CMD_ERR_EID               10
#define GENERIC_THRUSTER_CMD_NOOP_INF_EID          11
#define GENERIC_THRUSTER_CMD_RESET_INF_EID         12
#define GENERIC_THRUSTER_CMD_ENABLE_INF_EID        13
#define GENERIC_THRUSTER_ENABLE_INF_EID            14
#define GENERIC_THRUSTER_ENABLE_ERR_EID            15
#define GENERIC_THRUSTER_CMD_DISABLE_INF_EID       16
#define GENERIC_THRUSTER_DISABLE_INF_EID           17
#define GENERIC_THRUSTER_DISABLE_ERR_EID           18
#define GENERIC_THRUSTER_CMD_PERCENTAGE_INF_EID    19

/* Device specific command event IDs */
#define GENERIC_THRUSTER_CMD_CONFIG_INF_EID        20
#define GENERIC_THRUSTER_PERCENTAGE_INF_EID        21
#define GENERIC_THRUSTER_PERCENTAGE_ERR_EID        22
#define GENERIC_THRUSTER_CMD_PERCENTAGE_EID        23

/* Standard telemetry event IDs */
#define GENERIC_THRUSTER_DEVICE_TLM_ERR_EID        30
#define GENERIC_THRUSTER_REQ_HK_ERR_EID            31

/* Device specific telemetry event IDs */
#define GENERIC_THRUSTER_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define GENERIC_THRUSTER_UART_INIT_ERR_EID         40
#define GENERIC_THRUSTER_UART_CLOSE_ERR_EID        41

#endif /* _GENERIC_THRUSTER_EVENTS_H_ */
