/************************************************************************
** File:
**   $Id: generic_imu_msgids.h  $
**
** Purpose:
**  Define GENERIC_IMU Message IDs
**
*************************************************************************/
#ifndef _GENERIC_IMU_MSGIDS_H_
#define _GENERIC_IMU_MSGIDS_H_

/* 
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define GENERIC_IMU_CMD_MID              0x1925

/* 
** This MID is for commands telling the app to publish its telemetry message
*/
#define GENERIC_IMU_REQ_HK_MID           0x1926

/* 
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define GENERIC_IMU_HK_TLM_MID           0x0925
#define GENERIC_IMU_DEVICE_TLM_MID       0x0926

#endif /* _GENERIC_IMU_MSGIDS_H_ */
