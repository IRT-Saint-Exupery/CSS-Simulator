/*******************************************************************************
** File: generic_imu_app.h
**
** Purpose:
**   This is the main header file for the GENERIC_IMU application.
**
*******************************************************************************/
#ifndef _GENERIC_IMU_APP_H_
#define _GENERIC_IMU_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "generic_imu_device.h"
#include "generic_imu_events.h"
#include "generic_imu_platform_cfg.h"
#include "generic_imu_perfids.h"
#include "generic_imu_msg.h"
#include "generic_imu_msgids.h"
#include "generic_imu_version.h"
#include "hwlib.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define GENERIC_IMU_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define GENERIC_IMU_DEVICE_DISABLED       0
#define GENERIC_IMU_DEVICE_ENABLED        1


/*
** GENERIC_IMU global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `generic_imu_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    GENERIC_IMU_Hk_tlm_t   HkTelemetryPkt;   /* GENERIC_IMU Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    /*
	** Device data 
	*/
	uint32 DeviceID;		            /* Device ID provided by CFS on initialization */
    GENERIC_IMU_Device_tlm_t DevicePkt;      /* Device specific data packet */

    /* 
    ** Device protocol: CAN
    */ 
    can_info_t Generic_imuCan;             /* Hardware protocol definition */

} GENERIC_IMU_AppData_t;


/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern GENERIC_IMU_AppData_t GENERIC_IMU_AppData; /* GENERIC_IMU App Data */


/*
**
** Local function prototypes.
**
** Note: Except for the entry point (IMU_AppMain), these
**       functions are not called from any other source module.
*/
void  IMU_AppMain(void);
int32 GENERIC_IMU_AppInit(void);
void  GENERIC_IMU_ProcessCommandPacket(void);
void  GENERIC_IMU_ProcessGroundCommand(void);
void  GENERIC_IMU_ProcessTelemetryRequest(void);
void  GENERIC_IMU_ReportHousekeeping(void);
void  GENERIC_IMU_ReportDeviceTelemetry(void);
void  GENERIC_IMU_ResetCounters(void);
void  GENERIC_IMU_Enable(void);
void  GENERIC_IMU_Disable(void);
int32 GENERIC_IMU_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

#endif /* _GENERIC_IMU_APP_H_ */
