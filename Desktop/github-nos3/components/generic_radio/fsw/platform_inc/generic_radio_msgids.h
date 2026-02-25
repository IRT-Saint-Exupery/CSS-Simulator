/************************************************************************
** File:
**   $Id: generic_radio_msgids.h  $
**
** Purpose:
**  Define GENERIC_RADIO Message IDs
**
*************************************************************************/
#ifndef _GENERIC_RADIO_MSGIDS_H_
#define _GENERIC_RADIO_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define GENERIC_RADIO_CMD_MID              0x1930

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define GENERIC_RADIO_REQ_HK_MID           0x1931

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define GENERIC_RADIO_HK_TLM_MID           0x0930
#define GENERIC_RADIO_DEVICE_TLM_MID       0x0931

#endif /* _GENERIC_RADIO_MSGIDS_H_ */
