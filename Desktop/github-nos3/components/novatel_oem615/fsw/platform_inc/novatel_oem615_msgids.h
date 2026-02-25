/************************************************************************
** File:
**   $Id: novatel_oem615_msgids.h  $
**
** Purpose:
**  Define NOVATEL_OEM615 Message IDs
**
*************************************************************************/
#ifndef _NOVATEL_OEM615_MSGIDS_H_
#define _NOVATEL_OEM615_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define NOVATEL_OEM615_CMD_MID              0x1870

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define NOVATEL_OEM615_REQ_HK_MID           0x1871

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define NOVATEL_OEM615_HK_TLM_MID           0x0870
#define NOVATEL_OEM615_DEVICE_TLM_MID       0x0871

#endif /* _NOVATEL_OEM615_MSGIDS_H_ */
