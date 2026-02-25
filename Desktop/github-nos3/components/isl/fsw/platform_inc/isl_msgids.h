/************************************************************************
** File:
**   $Id: isl_msgids.h  $
**
** Purpose:
**  Define ISL Message IDs
**
*************************************************************************/
#ifndef _ISL_MSGIDS_H_
#define _ISL_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define ISL_CMD_MID              0x1872 /* TODO: Change this for your app */

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define ISL_REQ_HK_MID           0x1873 /* TODO: Change this for your app */


/* 
** This MID is for watchdog messages
*/
#define ISL_REQ_WD_MID           0x1874 /* TODO: Change this for your app */

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define ISL_HK_TLM_MID           0x0872 /* TODO: Change this for your app */
#define ISL_DEVICE_TLM_MID       0x0873 /* TODO: Change this for your app */

#endif /* _ISL_MSGIDS_H_ */
