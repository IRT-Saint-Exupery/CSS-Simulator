/*******************************************************************************
** File: generic_css_app.c
**
** Purpose:
**   This file contains the source code for the GENERIC_CSS application.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_css_app.h"


/*
** Global Data
*/
GENERIC_CSS_AppData_t GENERIC_CSS_AppData;

/*
** Application entry point and main process loop
*/
void CSS_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(GENERIC_CSS_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = GENERIC_CSS_AppInit();
    if (status != CFE_SUCCESS)
    {
        GENERIC_CSS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&GENERIC_CSS_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(GENERIC_CSS_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&GENERIC_CSS_AppData.MsgPtr,  GENERIC_CSS_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(GENERIC_CSS_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            GENERIC_CSS_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(GENERIC_CSS_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_CSS: SB Pipe Read Error = %d", (int) status);
            GENERIC_CSS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Disable component, which cleans up the interface, upon exit
    */
    GENERIC_CSS_Disable();

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(GENERIC_CSS_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(GENERIC_CSS_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 GENERIC_CSS_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    GENERIC_CSS_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);    /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_CSS: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&GENERIC_CSS_AppData.CmdPipe, GENERIC_CSS_PIPE_DEPTH, "CSS_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_CSS_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_CSS_CMD_MID), GENERIC_CSS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_CSS_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            GENERIC_CSS_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_CSS_REQ_HK_MID), GENERIC_CSS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_CSS_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            GENERIC_CSS_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the GENERIC_CSS_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_CSS_AppData.HkTelemetryPkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_CSS_HK_TLM_MID),
                   GENERIC_CSS_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_CSS_AppData.DevicePkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_CSS_DEVICE_TLM_MID),
                   GENERIC_CSS_DEVICE_TLM_LNGTH);

    /* 
    ** Always reset all counters during application initialization 
    */
    GENERIC_CSS_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    GENERIC_CSS_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_CSS_DEVICE_DISABLED;
    GENERIC_CSS_AppData.Generic_cssI2c.handle = GENERIC_CSS_CFG_HANDLE;
    GENERIC_CSS_AppData.Generic_cssI2c.isOpen = PORT_CLOSED;
    GENERIC_CSS_AppData.Generic_cssI2c.speed = GENERIC_CSS_CFG_BAUDRATE_HZ;
    GENERIC_CSS_AppData.Generic_cssI2c.addr = GENERIC_CSS_I2C_ADDRESS;

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(GENERIC_CSS_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "GENERIC_CSS App Initialized. Version %d.%d.%d.%d",
                GENERIC_CSS_MAJOR_VERSION,
                GENERIC_CSS_MINOR_VERSION, 
                GENERIC_CSS_REVISION, 
                GENERIC_CSS_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_CSS: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the GENERIC_CSS command pipe
*/
void GENERIC_CSS_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(GENERIC_CSS_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the GENERIC_CSS_CMD_MID (Message ID)
        */
        case GENERIC_CSS_CMD_MID:
            GENERIC_CSS_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case GENERIC_CSS_REQ_HK_MID:
            GENERIC_CSS_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            GENERIC_CSS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_CSS_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "GENERIC_CSS: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
} 


/*
** Process ground commands
*/
void GENERIC_CSS_ProcessGroundCommand(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(GENERIC_CSS_AppData.MsgPtr, &MsgId);   

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(GENERIC_CSS_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case GENERIC_CSS_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (GENERIC_CSS_VerifyCmdLength(GENERIC_CSS_AppData.MsgPtr, sizeof(GENERIC_CSS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(GENERIC_CSS_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_CSS: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case GENERIC_CSS_RESET_COUNTERS_CC:
            if (GENERIC_CSS_VerifyCmdLength(GENERIC_CSS_AppData.MsgPtr, sizeof(GENERIC_CSS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_CSS_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_CSS: RESET counters command received");
                GENERIC_CSS_ResetCounters();
            }
            break;

        /*
        ** Enable Command
        */
        case GENERIC_CSS_ENABLE_CC:
            if (GENERIC_CSS_VerifyCmdLength(GENERIC_CSS_AppData.MsgPtr, sizeof(GENERIC_CSS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_CSS_CMD_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_CSS: Enable command received");
                GENERIC_CSS_Enable();
            }
            break;

        /*
        ** Disable Command
        */
        case GENERIC_CSS_DISABLE_CC:
            if (GENERIC_CSS_VerifyCmdLength(GENERIC_CSS_AppData.MsgPtr, sizeof(GENERIC_CSS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_CSS_CMD_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_CSS: Disable command received");
                GENERIC_CSS_Disable();
            }
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_CSS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_CSS_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_CSS: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 


/*
** Process Telemetry Request - Triggered in response to a telemetery request
*/
void GENERIC_CSS_ProcessTelemetryRequest(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(GENERIC_CSS_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(GENERIC_CSS_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case GENERIC_CSS_REQ_HK_TLM:
            GENERIC_CSS_ReportHousekeeping();
            break;

        case GENERIC_CSS_REQ_DATA_TLM:
            GENERIC_CSS_ReportDeviceTelemetry();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_CSS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_CSS_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_CSS: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
void GENERIC_CSS_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    /* No HK data to request from device */

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_CSS_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_CSS_AppData.HkTelemetryPkt, true);
    return;
}


/*
** Collect and Report Device Telemetry
*/
void GENERIC_CSS_ReportDeviceTelemetry(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_CSS_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_CSS_DEVICE_ENABLED)
    {
        status = GENERIC_CSS_RequestData(&GENERIC_CSS_AppData.Generic_cssI2c, &GENERIC_CSS_AppData.DevicePkt.Generic_css);
        if (status == OS_SUCCESS)
        {
            GENERIC_CSS_AppData.HkTelemetryPkt.DeviceCount++;
            /* Time stamp and publish data telemetry */
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_CSS_AppData.DevicePkt);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_CSS_AppData.DevicePkt, true);
        }
        else
        {
            GENERIC_CSS_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_CSS_REQ_DATA_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "GENERIC_CSS: Request device data reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */
    return;
}


/*
** Reset all global counter variables
*/
void GENERIC_CSS_ResetCounters(void)
{
    GENERIC_CSS_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    GENERIC_CSS_AppData.HkTelemetryPkt.CommandCount = 0;
    GENERIC_CSS_AppData.HkTelemetryPkt.DeviceErrorCount = 0;
    GENERIC_CSS_AppData.HkTelemetryPkt.DeviceCount = 0;
    return;
} 


/*
** Enable Component
*/
void GENERIC_CSS_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is disabled */
    if (GENERIC_CSS_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_CSS_DEVICE_DISABLED)
    {
        /* Open device specific protocols */
        status = i2c_master_init(&GENERIC_CSS_AppData.Generic_cssI2c);
        if (status == OS_SUCCESS)
        {
            GENERIC_CSS_AppData.HkTelemetryPkt.DeviceCount++;
            GENERIC_CSS_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_CSS_DEVICE_ENABLED;
            CFE_EVS_SendEvent(GENERIC_CSS_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_CSS: Device enabled");
        }
        else
        {
            GENERIC_CSS_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_CSS_I2C_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_CSS: I2C port initialization error %d", status);
        }
    }
    else
    {
        GENERIC_CSS_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_CSS_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_CSS: Device enable failed, already enabled");
    }
    return;
}


/*
** Disable Component
*/
void GENERIC_CSS_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_CSS_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_CSS_DEVICE_ENABLED)
    {
        GENERIC_CSS_AppData.HkTelemetryPkt.DeviceCount++;
        GENERIC_CSS_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_CSS_DEVICE_DISABLED;
        CFE_EVS_SendEvent(GENERIC_CSS_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_CSS: Device disabled");
    }
    else
    {
        GENERIC_CSS_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_CSS_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_CSS: Device disable failed, already disabled");
    }
    return;
}


/*
** Verify command packet length matches expected
*/
int32 GENERIC_CSS_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;
    
    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        GENERIC_CSS_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(GENERIC_CSS_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        GENERIC_CSS_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
} 
