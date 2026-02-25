/*******************************************************************************
** Purpose:
**   This file contains the source code for the Generic ADCS application.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_adcs_perfids.h"
#include "generic_adcs_events.h"
#include "generic_adcs_msgids.h"
#include "generic_adcs_version.h"
#include "generic_adcs_ingest.h"
#include "generic_adcs_adac.h"
#include "generic_adcs_output.h"
#include "generic_adcs_app.h"

// ADCS sensor/actuator messages
#include "generic_mag_msgids.h"
#include "generic_fss_msgids.h"
#include "generic_css_msgids.h"
#include "generic_imu_msgids.h"
#include "generic_torquer_msgids.h"
#include "generic_reaction_wheel_msgids.h"
#include "novatel_oem615_msgids.h" // IRT CSS
#include "generic_star_tracker_msgids.h" // IRT CSS

/*
** Global Data
*/
Generic_ADCS_AppData_t Generic_ADCS_AppData;

/*
** Forward Declarations
*/
static int32 Generic_ADCS_AppInit(void);
static void  Generic_ADCS_ProcessCommandPacket(void);
static void  Generic_ADCS_ProcessGroundCommand(void);
static void  Generic_ADCS_ProcessTelemetryRequest(void);
static void  Generic_ADCS_ReportHousekeeping(void);
static void  Generic_ADCS_ResetCounters(void);
static int32 Generic_ADCS_SendDICommand(void);
static int32 Generic_ADCS_SendADCommand(void);
static int32 Generic_ADCS_SendGNCCommand(void);
static int32 Generic_ADCS_SendACCommand(void);
static int32 Generic_ADCS_SendDOCommand(void);
static int32 Generic_ADCS_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);

/*
** Application entry point and main process loop
*/
void ADCS_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(GENERIC_ADCS_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = Generic_ADCS_AppInit();
    if (status != CFE_SUCCESS)
    {
        Generic_ADCS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&Generic_ADCS_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(GENERIC_ADCS_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&Generic_ADCS_AppData.MsgPtr,  Generic_ADCS_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(GENERIC_ADCS_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            Generic_ADCS_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(GENERIC_ADCS_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "Generic_ADCS: SB Pipe Read Error = %d", (int) status);
            Generic_ADCS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(GENERIC_ADCS_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(Generic_ADCS_AppData.RunStatus);
} 


/* 
** Initialize application
*/
static int32 Generic_ADCS_AppInit(void)
{
    int32 status = OS_SUCCESS;

    Generic_ADCS_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);    /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_ADCS: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&Generic_ADCS_AppData.CmdPipe, GENERIC_ADCS_PIPE_DEPTH, "ADCS_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_ADCS_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_ADCS_CMD_MID), Generic_ADCS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_ADCS_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            GENERIC_ADCS_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_ADCS_REQ_HK_MID), Generic_ADCS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_ADCS_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            GENERIC_ADCS_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /*
    ** TODO: Subscribe to any other messages here
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_ADCS_ADAC_UPDATE_MID), Generic_ADCS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_ADCS_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to ADAC Update, MID=0x%04X, RC=0x%08X",
            GENERIC_ADCS_ADAC_UPDATE_MID, (unsigned int) status);
        return status;
    }

    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the Generic_ADCS_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(Generic_ADCS_AppData.HkTelemetryPkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_ADCS_HK_TLM_MID),
                   GENERIC_ADCS_HK_TLM_LNGTH);

    /*
    ** Initialize any other messages that this app will publish
    */
    CFE_MSG_Init(CFE_MSG_PTR(Generic_ADCS_AppData.DIPacket.TlmHeader), CFE_SB_ValueToMsgId(GENERIC_ADCS_DI_MID), GENERIC_ADCS_DI_LNGTH);
    CFE_MSG_Init(CFE_MSG_PTR(Generic_ADCS_AppData.ADPacket.TlmHeader), CFE_SB_ValueToMsgId(GENERIC_ADCS_AD_MID), GENERIC_ADCS_AD_LNGTH);
    CFE_MSG_Init(CFE_MSG_PTR(Generic_ADCS_AppData.GNCPacket.TlmHeader), CFE_SB_ValueToMsgId(GENERIC_ADCS_GNC_MID), GENERIC_ADCS_GNC_LNGTH);
    CFE_MSG_Init(CFE_MSG_PTR(Generic_ADCS_AppData.ACSPacket.TlmHeader), CFE_SB_ValueToMsgId(GENERIC_ADCS_AC_MID), GENERIC_ADCS_AC_LNGTH);
    CFE_MSG_Init(CFE_MSG_PTR(Generic_ADCS_AppData.DOPacket.TlmHeader), CFE_SB_ValueToMsgId(GENERIC_ADCS_DO_MID), GENERIC_ADCS_DO_LNGTH);

    CFE_MSG_Init(CFE_MSG_PTR(Generic_ADCS_AppData.MtbPctOnCmd.CmdHeader), CFE_SB_ValueToMsgId(GENERIC_TORQUER_CMD_MID), GENERIC_TORQUER_ALL_PERCENT_ON_CMD_LEN);
    CFE_MSG_SetFcnCode((CFE_MSG_Message_t *)&Generic_ADCS_AppData.MtbPctOnCmd,  GENERIC_TORQUER_CONFIG_ALL_CC);
    CFE_MSG_Init(CFE_MSG_PTR(Generic_ADCS_AppData.RwCmd.CmdHeader), CFE_SB_ValueToMsgId(GENERIC_RW_APP_CMD_MID), GENERIC_RW_CMD_LEN);
    CFE_MSG_SetFcnCode((CFE_MSG_Message_t *)&Generic_ADCS_AppData.RwCmd,  GENERIC_RW_APP_SET_TORQUE_CC);

    /* 
    ** Always reset all counters during application initialization 
    */
    Generic_ADCS_ResetCounters();

    /* ADCS initializations */
    FILE *adcs_in = fopen("cf/Inp_DI.txt", "r");
    if (adcs_in == NULL) {
        CFE_EVS_SendEvent(GENERIC_ADCS_FOPEN_ERR_EID, CFE_EVS_EventType_ERROR, "Error opening cf/Inp_DI.txt");
        return OS_ERROR;
    }
    Generic_ADCS_ingest_init(adcs_in, &Generic_ADCS_AppData.DIPacket.Payload);
    fclose(adcs_in);

    adcs_in = fopen("cf/Inp_ADAC.txt", "r");
    if (adcs_in == NULL) {
        CFE_EVS_SendEvent(GENERIC_ADCS_FOPEN_ERR_EID, CFE_EVS_EventType_ERROR, "Error opening cf/Inp_ADAC.txt");
        return OS_ERROR;
    }
    Generic_ADCS_init_attitude_determination_and_attitude_control(adcs_in, &Generic_ADCS_AppData.ADPacket.Payload, &Generic_ADCS_AppData.GNCPacket.Payload, &Generic_ADCS_AppData.ACSPacket.Payload);
    fclose(adcs_in);

    adcs_in = fopen("cf/Inp_DO.txt", "r");
    if (adcs_in == NULL) {
        CFE_EVS_SendEvent(GENERIC_ADCS_FOPEN_ERR_EID, CFE_EVS_EventType_ERROR, "Error opening cf/Inp_DO.txt");
        return OS_ERROR;
    }
    Generic_ADCS_output_init(adcs_in, &Generic_ADCS_AppData.DOPacket.Payload);
    fclose(adcs_in);

    /*
    ** Subscribe to ADCS packets from the sensors
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_MAG_DEVICE_TLM_MID), Generic_ADCS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_ADCS App: Error Subscribing to GENERIC_MAG_DEVICE_TLM_MID, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_FSS_DEVICE_TLM_MID), Generic_ADCS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_ADCS App: Error Subscribing to GENERIC_FSS_DEVICE_TLM_MID, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_CSS_DEVICE_TLM_MID), Generic_ADCS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_ADCS App: Error Subscribing to GENERIC_CSS_DEVICE_TLM_MID, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_IMU_DEVICE_TLM_MID), Generic_ADCS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_ADCS App: Error Subscribing to GENERIC_IMU_DEVICE_TLM_MID, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_RW_APP_HK_TLM_MID), Generic_ADCS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_ADCS App: Error Subscribing to GENERIC_RW_APP_HK_TLM_MID, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(NOVATEL_OEM615_DEVICE_TLM_MID), Generic_ADCS_AppData.CmdPipe); // IRT CSS
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_ADCS App: Error Subscribing to NOVATEL_OEM615_DEVICE_TLM_MID, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_STAR_TRACKER_DEVICE_TLM_MID), Generic_ADCS_AppData.CmdPipe); // IRT CSS
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_ADCS App: Error Subscribing to GENERIC_STAR_TRACKER_DEVICE_TLM_MID, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(GENERIC_ADCS_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "Generic_ADCS App Initialized. Version %d.%d.%d.%d",
                GENERIC_ADCS_MAJOR_VERSION,
                GENERIC_ADCS_MINOR_VERSION, 
                GENERIC_ADCS_REVISION, 
                GENERIC_ADCS_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_ADCS: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
}

/* 
** Process packets received on the Generic_ADCS command pipe
*/
static void  Generic_ADCS_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(Generic_ADCS_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the GENERIC_ADCS_CMD_MID (Message ID)
        */
        case GENERIC_ADCS_CMD_MID:
            Generic_ADCS_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case GENERIC_ADCS_REQ_HK_MID:
            Generic_ADCS_ProcessTelemetryRequest();
            break;

        case GENERIC_MAG_DEVICE_TLM_MID:
            Generic_ADCS_ingest_generic_mag(Generic_ADCS_AppData.MsgPtr, &Generic_ADCS_AppData.DIPacket.Payload.Mag);
            break;

        case GENERIC_FSS_DEVICE_TLM_MID:
            Generic_ADCS_ingest_generic_fss(Generic_ADCS_AppData.MsgPtr, &Generic_ADCS_AppData.DIPacket.Payload.Fss);
            break;

        case GENERIC_CSS_DEVICE_TLM_MID:
            Generic_ADCS_ingest_generic_css(Generic_ADCS_AppData.MsgPtr, &Generic_ADCS_AppData.DIPacket.Payload.Css);
            break;

        case GENERIC_IMU_DEVICE_TLM_MID:
            Generic_ADCS_ingest_generic_imu(Generic_ADCS_AppData.MsgPtr, &Generic_ADCS_AppData.DIPacket.Payload.Imu);
            break;
        
        case GENERIC_RW_APP_HK_TLM_MID:
            Generic_ADCS_ingest_generic_rw(Generic_ADCS_AppData.MsgPtr, &Generic_ADCS_AppData.DIPacket.Payload.Rw);
            break;
            
        case NOVATEL_OEM615_DEVICE_TLM_MID:
            Generic_ADCS_ingest_generic_Gps(Generic_ADCS_AppData.MsgPtr, &Generic_ADCS_AppData.DIPacket.Payload.Gps); // IRT CSS
            break; 
            
        case GENERIC_STAR_TRACKER_DEVICE_TLM_MID:
            Generic_ADCS_ingest_generic_Star_Tracker(Generic_ADCS_AppData.MsgPtr, &Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker); // IRT CSS
            break;    

        case GENERIC_ADCS_ADAC_UPDATE_MID:
            Generic_ADCS_execute_attitude_determination_and_attitude_control(&Generic_ADCS_AppData.DIPacket.Payload, &Generic_ADCS_AppData.ADPacket.Payload, &Generic_ADCS_AppData.GNCPacket.Payload, &Generic_ADCS_AppData.ACSPacket.Payload);
            Generic_ADCS_output_to_actuators(&Generic_ADCS_AppData.GNCPacket.Payload, &Generic_ADCS_AppData.DOPacket.Payload, &Generic_ADCS_AppData.MtbPctOnCmd, &Generic_ADCS_AppData.RwCmd);
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_ADCS_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "Generic_ADCS: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
}

/*
** Process ground commands
*/
static void  Generic_ADCS_ProcessGroundCommand(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(Generic_ADCS_AppData.MsgPtr, &MsgId);

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(Generic_ADCS_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case GENERIC_ADCS_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (Generic_ADCS_VerifyCmdLength(Generic_ADCS_AppData.MsgPtr, sizeof(Generic_ADCS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(GENERIC_ADCS_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "Generic_ADCS: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            } else {
                Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            }

            break;

        /*
        ** Reset Counters Command
        */
        case GENERIC_ADCS_RESET_COUNTERS_CC:
            if (Generic_ADCS_VerifyCmdLength(Generic_ADCS_AppData.MsgPtr, sizeof(Generic_ADCS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_ADCS_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "Generic_ADCS: RESET counters command received");
                Generic_ADCS_ResetCounters();
            } else {
                Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            }
            break;

        case GENERIC_ADCS_SET_MODE_CC:
            if (Generic_ADCS_VerifyCmdLength(Generic_ADCS_AppData.MsgPtr, sizeof(Generic_ADCS_Mode_cmd_t)) == OS_SUCCESS)
            {
                Generic_ADCS_Mode_cmd_t *cmd;
                cmd = (Generic_ADCS_Mode_cmd_t *)Generic_ADCS_AppData.MsgPtr; 
                Generic_ADCS_AppData.GNCPacket.Payload.Mode = cmd->Mode; // Keep the current value in **one** place
                CFE_EVS_SendEvent(GENERIC_ADCS_SET_MODE_INF_EID, CFE_EVS_EventType_INFORMATION, "***ADCS*** Changed mode to: %u", cmd->Mode);
                CFE_EVS_SendEvent(GENERIC_ADCS_SET_MODE_INF_EID, CFE_EVS_EventType_INFORMATION, "***ADCS*** Critical TC with MAC = %.4s", cmd->MAC); //IRT CSS
                //printf("Critical TC with MAC = %.4s \n",cmd->MAC ); //IRT CSS
            } else {
                Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            }
            break;

        case GENERIC_ADCS_SET_MOMENTUM_MANAGEMENT_CC:
            if (Generic_ADCS_VerifyCmdLength(Generic_ADCS_AppData.MsgPtr, sizeof(Generic_ADCS_MomentumManagement_cmd_t)) == OS_SUCCESS)
            {
                Generic_ADCS_MomentumManagement_cmd_t *cmd;
                cmd = (Generic_ADCS_MomentumManagement_cmd_t *)Generic_ADCS_AppData.MsgPtr; 
                Generic_ADCS_AppData.GNCPacket.Payload.HmgmtOn = cmd->MomentumManagement; // Keep the current value in **one** place
                CFE_EVS_SendEvent(GENERIC_ADCS_SET_MOMENTUM_MANAGEMENT_INF_EID, CFE_EVS_EventType_INFORMATION, "***ADCS*** Changed momentum management to: %u", cmd->MomentumManagement);
            } else {
                Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            }
            break;

        case GENERIC_ADCS_SEND_DI_CMD_CC:
            if (Generic_ADCS_VerifyCmdLength(Generic_ADCS_AppData.MsgPtr, sizeof(Generic_ADCS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                int32 status = Generic_ADCS_SendDICommand();
                if (status != CFE_SUCCESS) {
                    CFE_EVS_SendEvent(GENERIC_ADCS_CMD_ERR_EID, CFE_EVS_EventType_ERROR, "Unable to send DI telemetry: status = %d", status);
                }
            } else {
                Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            }
            break;

        case GENERIC_ADCS_SEND_AD_CMD_CC:
            if (Generic_ADCS_VerifyCmdLength(Generic_ADCS_AppData.MsgPtr, sizeof(Generic_ADCS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                int32 status = Generic_ADCS_SendADCommand();
                if (status != CFE_SUCCESS) {
                    CFE_EVS_SendEvent(GENERIC_ADCS_CMD_ERR_EID, CFE_EVS_EventType_ERROR, "Unable to send AD telemetry: status = %d", status);
                }
            } else {
                Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            }
            break;

        case GENERIC_ADCS_SEND_GNC_CMD_CC:
            if (Generic_ADCS_VerifyCmdLength(Generic_ADCS_AppData.MsgPtr, sizeof(Generic_ADCS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                int32 status = Generic_ADCS_SendGNCCommand();
                if (status != CFE_SUCCESS) {
                    CFE_EVS_SendEvent(GENERIC_ADCS_CMD_ERR_EID, CFE_EVS_EventType_ERROR, "Unable to send GNC telemetry: status = %d", status);
                }
            } else {
                Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            }
            break;

        case GENERIC_ADCS_SEND_AC_CMD_CC:
            if (Generic_ADCS_VerifyCmdLength(Generic_ADCS_AppData.MsgPtr, sizeof(Generic_ADCS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                int32 status = Generic_ADCS_SendACCommand();
                if (status != CFE_SUCCESS) {
                    CFE_EVS_SendEvent(GENERIC_ADCS_CMD_ERR_EID, CFE_EVS_EventType_ERROR, "Unable to send AC telemetry: status = %d", status);
                }
            } else {
                Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            }
            break;

        case GENERIC_ADCS_SEND_DO_CMD_CC:
            if (Generic_ADCS_VerifyCmdLength(Generic_ADCS_AppData.MsgPtr, sizeof(Generic_ADCS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                int32 status = Generic_ADCS_SendDOCommand();
                if (status != CFE_SUCCESS) {
                    CFE_EVS_SendEvent(GENERIC_ADCS_CMD_ERR_EID, CFE_EVS_EventType_ERROR, "Unable to send DO telemetry: status = %d", status);
                }
            } else {
                Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            }
            break;    
            
        case GENERIC_ADCS_SET_TARGET_CC:
            
            // plot the received command
            /*{char* pt = (char*) Generic_ADCS_AppData.MsgPtr;
            for ( int i = 0; i < 16; i++) {
		printf ("%02X ",(unsigned char) pt[i]);
				}
		printf ("\n");} */
						
            if (Generic_ADCS_VerifyCmdLength(Generic_ADCS_AppData.MsgPtr, sizeof(Generic_ADCS_SetTarget_cmd_t)) == OS_SUCCESS)
            {
            	Generic_ADCS_SetTarget_cmd_t *cmd;
                cmd = (Generic_ADCS_SetTarget_cmd_t *)Generic_ADCS_AppData.MsgPtr; 
                Generic_ADCS_AppData.GNCPacket.Payload.latitude = cmd->latitude; // Keep the current value in **one** place
                Generic_ADCS_AppData.GNCPacket.Payload.longitude = cmd->longitude; // Keep the current value in **one** place
                int32 status = Generic_ADCS_SendDOCommand();
                CFE_EVS_SendEvent(GENERIC_ADCS_SET_TARGET_INF_EID, CFE_EVS_EventType_INFORMATION, "***ADCS*** Target set to LAT / LONG : %f %f", cmd->latitude, cmd->longitude);
            } else {
                Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            }
            break;   

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_ADCS_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "Generic_ADCS: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 

/*
** Process Telemetry Request - Triggered in response to a telemetery request
*/
static void  Generic_ADCS_ProcessTelemetryRequest(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(Generic_ADCS_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(Generic_ADCS_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case GENERIC_ADCS_REQ_HK_TLM:
            Generic_ADCS_ReportHousekeeping();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_ADCS_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "Generic_ADCS: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}

/* 
** Report Application Housekeeping
*/
static void  Generic_ADCS_ReportHousekeeping(void)
{
    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.HkTelemetryPkt, true);
}

/*
** Reset all global counter variables
*/
static void  Generic_ADCS_ResetCounters(void)
{
    Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    Generic_ADCS_AppData.HkTelemetryPkt.CommandCount = 0;
}

static int32 Generic_ADCS_SendDICommand(void)
{
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.DIPacket);
    return CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.DIPacket, true);
}

static int32 Generic_ADCS_SendADCommand(void)
{
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.ADPacket);
    return CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.ADPacket, true);
}

static int32 Generic_ADCS_SendGNCCommand(void)
{
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.GNCPacket);
    return CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.GNCPacket, true);
}

static int32 Generic_ADCS_SendACCommand(void)
{
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.ACSPacket);
    return CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.ACSPacket, true);
}

static int32 Generic_ADCS_SendDOCommand(void)
{
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.DOPacket);
    return CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &Generic_ADCS_AppData.DOPacket, true);
}

/*
** Verify command packet length matches expected
*/
static int32 Generic_ADCS_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;
    
    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        Generic_ADCS_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(GENERIC_ADCS_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        Generic_ADCS_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
}
