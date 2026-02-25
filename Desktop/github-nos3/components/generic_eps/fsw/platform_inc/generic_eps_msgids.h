/************************************************************************
** File:
**   $Id: generic_eps_msgids.h  $
**
** Purpose:
**  Define GENERIC_EPS Message IDs
**
*************************************************************************/
#ifndef _GENERIC_EPS_MSGIDS_H_
#define _GENERIC_EPS_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define GENERIC_EPS_CMD_MID              0x191A

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define GENERIC_EPS_REQ_HK_MID           0x191B

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define GENERIC_EPS_HK_TLM_MID           0x091A

#endif /* _GENERIC_EPS_MSGIDS_H_ */
