/*******************************************************************************
** File: ids_app.h
**
** Purpose:
**   This is the main header file for the IDS application.
**
*******************************************************************************/
#ifndef _IDS_APP_H_
#define _IDS_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "ids_device.h"
#include "ids_events.h"
#include "ids_platform_cfg.h"
#include "ids_perfids.h"
#include "ids_msg.h"
#include "ids_msgids.h"
#include "ids_version.h"
#include "hwlib.h"
#include <time.h>
#include <semaphore.h>

/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define IDS_PIPE_DEPTH            32


/*
** Enabled and Disabled Definitions
*/
#define IDS_DEVICE_DISABLED       0
#define IDS_DEVICE_ENABLED        1


/*
** IDS global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `ids_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    IDS_Hk_tlm_t   HkTelemetryPkt;   /* IDS Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    /*
	** Device data 
    ** TODO: Make specific to your application
	*/
    IDS_Device_tlm_t DevicePkt;      /* Device specific data packet */

    //IDS Child data
    uint32   ChildTaskID;

    /* 
    ** Device protocol
    ** TODO: Make specific to your application
    */ 
    uart_info_t IdsUart;             /* Hardware protocol definition */

} IDS_AppData_t;

struct DetectionReporting {
	sem_t is_reporting;
    sem_t sem_detect_count;
	int pipe_alert[2];
};

struct AlertStruct {
	uint8_t module;
	uint8_t rule;
	//uint16_t data_len;
	uint8_t TName_len;
	//uint8_t packet[IDS_PDATA_SIZE];
	char Taskname[IDS_TSKNAME_SIZE];
};

union DetectionData {
	uint8_t buf[sizeof(struct AlertStruct)]; // TODO - replace sizeof by constant like a #define ?
	struct AlertStruct AlertData;
};

/*
 * Table size for the "to-subscribe" table
 */
#define MSG_IDS_TABLE_SIZE            0x2000
#define IDS_TABLE_SIZE            135
/*
 * Table for Message IDs info (command string)
 */

/*typedef struct _MsgIDstring {
	char CmdName[30];
}MsgIDstring;*/

/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern IDS_AppData_t IDS_AppData; /* IDS App Data */

/*
**
** Local function prototypes.
**
** Note: Except for the entry point (IDS_AppMain), these
**       functions are not called from any other source module.
*/
void  IDS_AppMain(void);
int32 IDS_AppInit(void);
void  IDS_ProcessCommandPacket(void);
void  IDS_ProcessGroundCommand(void);
void  IDS_ProcessTelemetryRequest(void);
void  IDS_ReportHousekeeping(void);
void  IDS_ReportDeviceTelemetry(uint8_t module, uint8_t rule, /*uint8_t * buf, uint16_t data_len,*/ char * TaskName);
void  IDS_ResetCounters(void);
void  IDS_Enable(void);
void  IDS_Disable(void);
int32 IDS_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

#ifdef IDS_TELEM_ENABLED
void  IDS_ChildTask(void);
void IDS_RaiseAlert(/*void*/uint8_t module, uint8_t rule, uint16_t data_len, uint8_t TName_len, uint8_t *packet, char *Taskname);
#endif

#endif /* _IDS_APP_H_ */
