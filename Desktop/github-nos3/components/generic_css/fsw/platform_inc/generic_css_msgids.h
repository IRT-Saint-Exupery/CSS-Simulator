/************************************************************************
** File:
**   $Id: generic_css_msgids.h  $
**
** Purpose:
**  Define GENERIC_CSS Message IDs
**
*************************************************************************/
#ifndef _GENERIC_CSS_MSGIDS_H_
#define _GENERIC_CSS_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define GENERIC_CSS_CMD_MID              0x1910

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define GENERIC_CSS_REQ_HK_MID           0x1911

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define GENERIC_CSS_HK_TLM_MID           0x0910
#define GENERIC_CSS_DEVICE_TLM_MID       0x0911

#endif /* _GENERIC_CSS_MSGIDS_H_ */
