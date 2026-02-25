/************************************************************************
** File:
**    generic_fss_events.h
**
** Purpose:
**  Define GENERIC_FSS application event IDs
**
*************************************************************************/

#ifndef _GENERIC_FSS_EVENTS_H_
#define _GENERIC_FSS_EVENTS_H_

/* Standard app event IDs */
#define GENERIC_FSS_RESERVED_EID              0
#define GENERIC_FSS_STARTUP_INF_EID           1
#define GENERIC_FSS_LEN_ERR_EID               2
#define GENERIC_FSS_PIPE_ERR_EID              3
#define GENERIC_FSS_SUB_CMD_ERR_EID           4
#define GENERIC_FSS_SUB_REQ_HK_ERR_EID        5
#define GENERIC_FSS_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define GENERIC_FSS_CMD_ERR_EID               10
#define GENERIC_FSS_CMD_NOOP_INF_EID          11
#define GENERIC_FSS_CMD_RESET_INF_EID         12
#define GENERIC_FSS_CMD_ENABLE_INF_EID        13
#define GENERIC_FSS_ENABLE_INF_EID            14
#define GENERIC_FSS_ENABLE_ERR_EID            15
#define GENERIC_FSS_CMD_DISABLE_INF_EID       16
#define GENERIC_FSS_DISABLE_INF_EID           17
#define GENERIC_FSS_DISABLE_ERR_EID           18

/* Standard telemetry event IDs */
#define GENERIC_FSS_DEVICE_TLM_ERR_EID        30

/* Device specific telemetry event IDs */
#define GENERIC_FSS_REQ_DATA_ERR_EID          31

/* Hardware protocol event IDs */
#define GENERIC_FSS_SPI_INIT_ERR_EID          40
#define GENERIC_FSS_SPI_CLOSE_ERR_EID         41

#endif /* _GENERIC_FSS_EVENTS_H_ */
