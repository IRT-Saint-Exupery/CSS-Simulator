/*******************************************************************************
** File: generic_imu_app.c
**
** Purpose:
**   This file contains the source code for the GENERIC_IMU application.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_imu_app.h"


/*
** Global Data
*/
GENERIC_IMU_AppData_t GENERIC_IMU_AppData;


/*
** Application entry point and main process loop
*/
void IMU_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(GENERIC_IMU_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = GENERIC_IMU_AppInit();
    if (status != CFE_SUCCESS)
    {
        GENERIC_IMU_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&GENERIC_IMU_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(GENERIC_IMU_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&GENERIC_IMU_AppData.MsgPtr,  GENERIC_IMU_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(GENERIC_IMU_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            GENERIC_IMU_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(GENERIC_IMU_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_IMU: SB Pipe Read Error = %d", (int) status);
            GENERIC_IMU_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Disable component, and clean up the interface
    */
    GENERIC_IMU_Disable();
    can_close_device(&GENERIC_IMU_AppData.Generic_imuCan);

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(GENERIC_IMU_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(GENERIC_IMU_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 GENERIC_IMU_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    GENERIC_IMU_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);    /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_IMU: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&GENERIC_IMU_AppData.CmdPipe, GENERIC_IMU_PIPE_DEPTH, "IMU_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_IMU_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_IMU_CMD_MID), GENERIC_IMU_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_IMU_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            GENERIC_IMU_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_IMU_REQ_HK_MID), GENERIC_IMU_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_IMU_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            GENERIC_IMU_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the GENERIC_IMU_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_IMU_AppData.HkTelemetryPkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_IMU_HK_TLM_MID),
                   GENERIC_IMU_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_IMU_AppData.DevicePkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_IMU_DEVICE_TLM_MID),
                   GENERIC_IMU_DEVICE_TLM_LNGTH);

    /* 
    ** Always reset all counters during application initialization 
    */
    GENERIC_IMU_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    GENERIC_IMU_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_IMU_DEVICE_DISABLED;
    GENERIC_IMU_AppData.HkTelemetryPkt.DeviceHK.DeviceCounter = 0;
    GENERIC_IMU_AppData.HkTelemetryPkt.DeviceHK.DeviceStatus = 0;

    /*
    ** Initialize hardware interface data
    */ 
    
    GENERIC_IMU_AppData.Generic_imuCan.handle = GENERIC_IMU_CFG_HANDLE;
    GENERIC_IMU_AppData.Generic_imuCan.isUp = CAN_INTERFACE_DOWN;
    GENERIC_IMU_AppData.Generic_imuCan.loopback = false;
    GENERIC_IMU_AppData.Generic_imuCan.listenOnly = false;
    GENERIC_IMU_AppData.Generic_imuCan.tripleSampling = false;
    GENERIC_IMU_AppData.Generic_imuCan.oneShot = false;
    GENERIC_IMU_AppData.Generic_imuCan.berrReporting = false;
    GENERIC_IMU_AppData.Generic_imuCan.fd = false;
    GENERIC_IMU_AppData.Generic_imuCan.presumeAck = false;
    GENERIC_IMU_AppData.Generic_imuCan.bitrate = GENERIC_IMU_CFG_CAN_BITRATE;
    GENERIC_IMU_AppData.Generic_imuCan.second_timeout = GENERIC_IMU_CFG_CAN_TIMEOUT;
    GENERIC_IMU_AppData.Generic_imuCan.microsecond_timeout = GENERIC_IMU_CFG_CAN_MS_TIMEOUT;
    GENERIC_IMU_AppData.Generic_imuCan.xfer_us_delay = GENERIC_IMU_CFG_CAN_XFER_US;

    status = can_init_dev(&GENERIC_IMU_AppData.Generic_imuCan);
    if (status != OS_SUCCESS)
    {
        GENERIC_IMU_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_IMU_CAN_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_IMU: CAN port initialization error %d", status);
    }

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(GENERIC_IMU_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "GENERIC_IMU App Initialized. Version %d.%d.%d.%d",
                GENERIC_IMU_MAJOR_VERSION,
                GENERIC_IMU_MINOR_VERSION, 
                GENERIC_IMU_REVISION, 
                GENERIC_IMU_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_IMU: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the GENERIC_IMU command pipe
*/
void GENERIC_IMU_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(GENERIC_IMU_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the GENERIC_IMU_CMD_MID (Message ID)
        */
        case GENERIC_IMU_CMD_MID:
            GENERIC_IMU_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case GENERIC_IMU_REQ_HK_MID:
            GENERIC_IMU_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            GENERIC_IMU_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_IMU_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "GENERIC_IMU: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
} 


/*
** Process ground commands
*/
void GENERIC_IMU_ProcessGroundCommand(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(GENERIC_IMU_AppData.MsgPtr, &MsgId);

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(GENERIC_IMU_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case GENERIC_IMU_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (GENERIC_IMU_VerifyCmdLength(GENERIC_IMU_AppData.MsgPtr, sizeof(GENERIC_IMU_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(GENERIC_IMU_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_IMU: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case GENERIC_IMU_RESET_COUNTERS_CC:
            if (GENERIC_IMU_VerifyCmdLength(GENERIC_IMU_AppData.MsgPtr, sizeof(GENERIC_IMU_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_IMU_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_IMU: RESET counters command received");
                GENERIC_IMU_ResetCounters();
            }
            break;

        /*
        ** Enable Command
        */
        case GENERIC_IMU_ENABLE_CC:
            if (GENERIC_IMU_VerifyCmdLength(GENERIC_IMU_AppData.MsgPtr, sizeof(GENERIC_IMU_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_IMU_CMD_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_IMU: Enable command received");
                GENERIC_IMU_Enable();
            }
            break;

        /*
        ** Disable Command
        */
        case GENERIC_IMU_DISABLE_CC:
            if (GENERIC_IMU_VerifyCmdLength(GENERIC_IMU_AppData.MsgPtr, sizeof(GENERIC_IMU_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_IMU_CMD_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_IMU: Disable command received");
                GENERIC_IMU_Disable();
            }
            break;
            
        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_IMU_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_IMU_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_IMU: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 


/*
** Process Telemetry Request - Triggered in response to a telemetry request
*/
void GENERIC_IMU_ProcessTelemetryRequest(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(GENERIC_IMU_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(GENERIC_IMU_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case GENERIC_IMU_REQ_HK_TLM:
            GENERIC_IMU_ReportHousekeeping();
            break;

        case GENERIC_IMU_REQ_DATA_TLM:
            GENERIC_IMU_ReportDeviceTelemetry();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_IMU_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_IMU_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_IMU: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
void GENERIC_IMU_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_IMU_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_IMU_DEVICE_ENABLED)
    {
        status = GENERIC_IMU_RequestHK(&GENERIC_IMU_AppData.Generic_imuCan, (GENERIC_IMU_Device_HK_tlm_t*) &GENERIC_IMU_AppData.HkTelemetryPkt.DeviceHK);
        if (status == OS_SUCCESS)
        {
            GENERIC_IMU_AppData.HkTelemetryPkt.DeviceCount++;
        }
        else
        {
            GENERIC_IMU_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_IMU_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "GENERIC_IMU: Request device HK reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_IMU_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_IMU_AppData.HkTelemetryPkt, true);
    return;
}


/*
** Collect and Report Device Telemetry
*/
void GENERIC_IMU_ReportDeviceTelemetry(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_IMU_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_IMU_DEVICE_ENABLED)
    {
        status = GENERIC_IMU_RequestData(&GENERIC_IMU_AppData.Generic_imuCan, (GENERIC_IMU_Device_Data_tlm_t*) &GENERIC_IMU_AppData.DevicePkt.Generic_imu);
        if (status == OS_SUCCESS)
        {
            GENERIC_IMU_AppData.HkTelemetryPkt.DeviceCount++;
            /* Time stamp and publish data telemetry */
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_IMU_AppData.DevicePkt);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_IMU_AppData.DevicePkt, true);
        }
        else
        {
            GENERIC_IMU_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_IMU_REQ_DATA_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "GENERIC_IMU: Request device data reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */
    return;
}


/*
** Reset all global counter variables
*/
void GENERIC_IMU_ResetCounters(void)
{
    GENERIC_IMU_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    GENERIC_IMU_AppData.HkTelemetryPkt.CommandCount = 0;
    GENERIC_IMU_AppData.HkTelemetryPkt.DeviceErrorCount = 0;
    GENERIC_IMU_AppData.HkTelemetryPkt.DeviceCount = 0;
    return;
} 


/*
** Enable Component
*/
void GENERIC_IMU_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is disabled */
    if (GENERIC_IMU_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_IMU_DEVICE_DISABLED)
    {
        GENERIC_IMU_AppData.HkTelemetryPkt.DeviceCount++;
        GENERIC_IMU_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_IMU_DEVICE_ENABLED;
        CFE_EVS_SendEvent(GENERIC_IMU_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_IMU: Device enabled");
    }
    else
    {
        GENERIC_IMU_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_IMU_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_IMU: Device enable failed, already enabled");
    }
    return;
}


/*
** Disable Component
*/
void GENERIC_IMU_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_IMU_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_IMU_DEVICE_ENABLED)
    {
        GENERIC_IMU_AppData.HkTelemetryPkt.DeviceCount++;
        GENERIC_IMU_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_IMU_DEVICE_DISABLED;
        CFE_EVS_SendEvent(GENERIC_IMU_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_IMU: Device disabled");
    }
    else
    {
        GENERIC_IMU_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_IMU_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_IMU: Device disable failed, already disabled");
    }
    return;
}


/*
** Verify command packet length matches expected
*/
int32 GENERIC_IMU_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;

    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        GENERIC_IMU_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(GENERIC_IMU_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        GENERIC_IMU_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
} 
