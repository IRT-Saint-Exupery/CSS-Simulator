/************************************************************************
** File:
**   generic_fss_msgids.h
**
** Purpose:
**  Define GENERIC_FSS Message IDs
**
*************************************************************************/
#ifndef _GENERIC_FSS_MSGIDS_H_
#define _GENERIC_FSS_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define GENERIC_FSS_CMD_MID              0x1920

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define GENERIC_FSS_REQ_HK_MID           0x1921

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define GENERIC_FSS_HK_TLM_MID           0x0920
#define GENERIC_FSS_DEVICE_TLM_MID       0x0921

#endif /* _GENERIC_FSS_MSGIDS_H_ */
