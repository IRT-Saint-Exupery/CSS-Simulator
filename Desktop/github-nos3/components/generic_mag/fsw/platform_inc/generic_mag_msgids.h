/************************************************************************
** File:
**   $Id: generic_mag_msgids.h  $
**
** Purpose:
**  Define GENERIC_MAG Message IDs
**
*************************************************************************/
#ifndef _GENERIC_MAG_MSGIDS_H_
#define _GENERIC_MAG_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define GENERIC_MAG_CMD_MID              0x192A

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define GENERIC_MAG_REQ_HK_MID           0x192B

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define GENERIC_MAG_HK_TLM_MID           0x092A
#define GENERIC_MAG_DEVICE_TLM_MID       0x092B

#endif /* _GENERIC_MAG_MSGIDS_H_ */
