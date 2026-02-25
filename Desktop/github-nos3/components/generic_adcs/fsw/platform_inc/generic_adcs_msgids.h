/************************************************************************
** Purpose:
**  Define Generic ADCS Message IDs
**
*************************************************************************/
#ifndef _GENERIC_ADCS_MSGIDS_H_
#define _GENERIC_ADCS_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define GENERIC_ADCS_CMD_MID              0x1940

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define GENERIC_ADCS_REQ_HK_MID           0x1941

/*
** This MID is for requesting the Generic ADCS app to perform AD and AC
*/
#define GENERIC_ADCS_ADAC_UPDATE_MID      0x1942

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define GENERIC_ADCS_HK_TLM_MID           0x0940
#define GENERIC_ADCS_DI_MID               0x0941
#define GENERIC_ADCS_AD_MID               0x0942
#define GENERIC_ADCS_GNC_MID              0x0943
#define GENERIC_ADCS_AC_MID               0x0944
#define GENERIC_ADCS_DO_MID               0x0945

#endif