/************************************************************************
** File:
**   $Id: generic_thruster_msgids.h  $
**
** Purpose:
**  Define GENERIC_THRUSTER Message IDs
**
*************************************************************************/
#ifndef _GENERIC_THRUSTER_MSGIDS_H_
#define _GENERIC_THRUSTER_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define GENERIC_THRUSTER_CMD_MID              0x18EA 

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define GENERIC_THRUSTER_REQ_HK_MID           0x18EB

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define GENERIC_THRUSTER_HK_TLM_MID           0x08EA

#endif /* _GENERIC_THRUSTER_MSGIDS_H_ */
