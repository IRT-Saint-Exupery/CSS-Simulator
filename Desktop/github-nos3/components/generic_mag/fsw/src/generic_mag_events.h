/************************************************************************
** File:
**    generic_mag_events.h
**
** Purpose:
**  Define GENERIC_MAG application event IDs
**
*************************************************************************/

#ifndef _GENERIC_MAG_EVENTS_H_
#define _GENERIC_MAG_EVENTS_H_

/* Standard app event IDs */
#define GENERIC_MAG_RESERVED_EID              0
#define GENERIC_MAG_STARTUP_INF_EID           1
#define GENERIC_MAG_LEN_ERR_EID               2
#define GENERIC_MAG_PIPE_ERR_EID              3
#define GENERIC_MAG_SUB_CMD_ERR_EID           4
#define GENERIC_MAG_SUB_REQ_HK_ERR_EID        5
#define GENERIC_MAG_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define GENERIC_MAG_CMD_ERR_EID               10
#define GENERIC_MAG_CMD_NOOP_INF_EID          11
#define GENERIC_MAG_CMD_RESET_INF_EID         12
#define GENERIC_MAG_CMD_ENABLE_INF_EID        13
#define GENERIC_MAG_ENABLE_INF_EID            14
#define GENERIC_MAG_ENABLE_ERR_EID            15
#define GENERIC_MAG_CMD_DISABLE_INF_EID       16
#define GENERIC_MAG_DISABLE_INF_EID           17
#define GENERIC_MAG_DISABLE_ERR_EID           18

/* Standard telemetry event IDs */
#define GENERIC_MAG_DEVICE_TLM_ERR_EID        30

/* Device specific telemetry event IDs */
#define GENERIC_MAG_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define GENERIC_MAG_SPI_INIT_ERR_EID          40
#define GENERIC_MAG_SPI_READ_ERR_EID          41

#endif /* _GENERIC_MAG_EVENTS_H_ */
