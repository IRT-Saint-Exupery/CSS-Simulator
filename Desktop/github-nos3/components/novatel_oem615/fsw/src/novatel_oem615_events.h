/************************************************************************
** File:
**    novatel_oem615_events.h
**
** Purpose:
**  Define NOVATEL_OEM615 application event IDs
**
*************************************************************************/

#ifndef _NOVATEL_OEM615_EVENTS_H_
#define _NOVATEL_OEM615_EVENTS_H_

/* Standard app event IDs */
#define NOVATEL_OEM615_RESERVED_EID              0
#define NOVATEL_OEM615_STARTUP_INF_EID           1
#define NOVATEL_OEM615_LEN_ERR_EID               2
#define NOVATEL_OEM615_PIPE_ERR_EID              3
#define NOVATEL_OEM615_SUB_CMD_ERR_EID           4
#define NOVATEL_OEM615_SUB_REQ_HK_ERR_EID        5
#define NOVATEL_OEM615_PROCESS_CMD_ERR_EID       6

/* Standard command event IDs */
#define NOVATEL_OEM615_CMD_ERR_EID               10
#define NOVATEL_OEM615_CMD_NOOP_INF_EID          11
#define NOVATEL_OEM615_CMD_RESET_INF_EID         12
#define NOVATEL_OEM615_CMD_ENABLE_INF_EID        13
#define NOVATEL_OEM615_ENABLE_INF_EID            14
#define NOVATEL_OEM615_ENABLE_ERR_EID            15
#define NOVATEL_OEM615_CMD_DISABLE_INF_EID       16
#define NOVATEL_OEM615_DISABLE_INF_EID           17
#define NOVATEL_OEM615_DISABLE_ERR_EID           18

/* Device specific command event IDs */
#define NOVATEL_OEM615_CMD_CONFIG_INF_EID        20
#define NOVATEL_OEM615_CONFIG_INF_EID            21
#define NOVATEL_OEM615_CONFIG_ERR_EID            22

/* Standard telemetry event IDs */
#define NOVATEL_OEM615_DEVICE_TLM_ERR_EID        30
#define NOVATEL_OEM615_REQ_HK_ERR_EID            31

/* Device specific telemetry event IDs */
#define NOVATEL_OEM615_REQ_DATA_ERR_EID          32

/* Hardware protocol event IDs */
#define NOVATEL_OEM615_UART_INIT_ERR_EID         40
#define NOVATEL_OEM615_UART_CLOSE_ERR_EID        41
#define NOVATEL_OEM615_UART_READ_ERR_EID         42
#define NOVATEL_OEM615_UART_WRITE_ERR_EID        43
#define NOVATEL_OEM615_UART_TIMEOUT_ERR_EID      44

/* Child task event IDs */
#define NOVATEL_OEM615_CHILDTASKINIT_ERR_EID     45
#define NOVATEL_OEM615_CHILDTASKINIT_INF_EID     46
#define NOVATEL_OEM615_CHILDTASKEXIT_INF_EID     47
#define NOVATEL_OEM615_CREATE_MUTEX_ERR_EID      48

/* Failure to obtain mutex safely event IDs */
#define NOVATEL_OEM615_MUT_CMD_DEVICE_ERR_EID      49
#define NOVATEL_OEM615_MUT_REQUEST_HK_ERR_EID      50
#define NOVATEL_OEM615_MUT_REQUEST_DATA_ERR_EID    51
#define NOVATEL_OEM615_MUT_INC_CMD_CNT_ERR_EID     52
#define NOVATEL_OEM615_MUT_INC_CMD_ERR_CNT_ERR_EID 53
#define NOVATEL_OEM615_MUT_INC_DEV_CNT_ERR_EID     54
#define NOVATEL_OEM615_MUT_INC_DEV_ERR_CNT_ERR_EID 55
#define NOVATEL_OEM615_MUT_GET_RUN_STATUS_ERR_EID  56
#define NOVATEL_OEM615_MUT_SET_RUN_STATUS_ERR_EID  57
#define NOVATEL_OEM615_MUT_GET_DEV_ENABLE_ERR_EID  58
#define NOVATEL_OEM615_MUT_SET_DEV_ENABLE_ERR_EID  59

/* Novatel specific command event IDs */
#define NOVATEL_OEM615_CMD_LOG_INF_EID             60
#define NOVATEL_OEM615_CMD_UNLOG_INF_EID           61
#define NOVATEL_OEM615_CMD_UNLOGALL_INF_EID        62
#define NOVATEL_OEM615_CMD_SERIALCONFIG_INF_EID    63

#endif /* _NOVATEL_OEM615_EVENTS_H_ */
