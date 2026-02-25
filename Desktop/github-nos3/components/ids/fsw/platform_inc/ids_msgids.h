/************************************************************************
** File:
**   $Id: ids_msgids.h  $
**
** Purpose:
**  Define IDS Message IDs
**
*************************************************************************/
#ifndef _IDS_MSGIDS_H_
#define _IDS_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define IDS_CMD_MID              0x199A /* TODO: Change this for your app */

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define IDS_REQ_HK_MID           0x199B /* TODO: Change this for your app */

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define IDS_HK_TLM_MID           0x09FA /* TODO: Change this for your app */
#define IDS_DEVICE_TLM_MID       0x09FB /* TODO: Change this for your app */

#endif /* _IDS_MSGIDS_H_ */
