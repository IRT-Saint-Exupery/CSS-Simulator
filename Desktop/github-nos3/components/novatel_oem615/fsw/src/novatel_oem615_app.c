/*******************************************************************************
** File: novatel_oem615_app.c
**
** Purpose:
**   This file contains the source code for the NOVATEL_OEM615 application.
**
*******************************************************************************/

/*
** Include Files
*/
#include "novatel_oem615_app.h"


/*
** Global Data
*/
NOVATEL_OEM615_AppData_t NOVATEL_OEM615_AppData;

/*
** Application entry point and main process loop
*/
void NOVATEL_AppMain(void)
{
    int32 status = OS_SUCCESS;
    uint32 local_run = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(NOVATEL_OEM615_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = NOVATEL_OEM615_AppInit();
    if (status != CFE_SUCCESS)
    {
        NOVATEL_OEM615_SetRunStatus(CFE_ES_RunStatus_APP_ERROR);
        local_run = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&local_run) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(NOVATEL_OEM615_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&NOVATEL_OEM615_AppData.MsgPtr,  NOVATEL_OEM615_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(NOVATEL_OEM615_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            NOVATEL_OEM615_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(NOVATEL_OEM615_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "NOVATEL_OEM615: SB Pipe Read Error = %d", (int) status);
            NOVATEL_OEM615_SetRunStatus(CFE_ES_RunStatus_APP_ERROR);
        }

        /*
        ** Check run status
        */
        local_run = NOVATEL_OEM615_GetRunStatus();
    }

    /*
    ** Disable component, which cleans up the interface, upon exit
    */
    NOVATEL_OEM615_Disable();

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(NOVATEL_OEM615_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(NOVATEL_OEM615_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 NOVATEL_OEM615_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    NOVATEL_OEM615_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&NOVATEL_OEM615_AppData.CmdPipe, NOVATEL_OEM615_PIPE_DEPTH, "NAV_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(NOVATEL_OEM615_CMD_MID), NOVATEL_OEM615_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            NOVATEL_OEM615_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(NOVATEL_OEM615_REQ_HK_MID), NOVATEL_OEM615_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            NOVATEL_OEM615_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the NOVATEL_OEM615_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(NOVATEL_OEM615_AppData.HkTelemetryPkt.TlmHeader),
                   CFE_SB_ValueToMsgId(NOVATEL_OEM615_HK_TLM_MID),
                   NOVATEL_OEM615_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(NOVATEL_OEM615_AppData.DevicePkt.TlmHeader),
                   CFE_SB_ValueToMsgId(NOVATEL_OEM615_DEVICE_TLM_MID),
                   NOVATEL_OEM615_DEVICE_TLM_LNGTH);

    /*
    ** Initialize hardware interface data
    ** Note that other components provide examples for the different protocols available
    */ 
    NOVATEL_OEM615_AppData.Novatel_oem615Uart.deviceString = NOVATEL_OEM615_CFG_STRING;
    NOVATEL_OEM615_AppData.Novatel_oem615Uart.handle = NOVATEL_OEM615_CFG_HANDLE;
    NOVATEL_OEM615_AppData.Novatel_oem615Uart.isOpen = PORT_CLOSED;
    NOVATEL_OEM615_AppData.Novatel_oem615Uart.baud = NOVATEL_OEM615_CFG_BAUDRATE_HZ;
    NOVATEL_OEM615_AppData.Novatel_oem615Uart.access_option = uart_access_flag_RDWR;

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceEnabled = NOVATEL_OEM615_DEVICE_DISABLED;
    NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceHK.DeviceCounter = 0;
    NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceHK.DeviceConfig = 0;
    NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceHK.DeviceStatus = 0;
    
    /* Create hk data mutex */
    status = OS_MutSemCreate(&NOVATEL_OEM615_AppData.HkDataMutex, NOVATEL_OEM615_HK_MUTEX_NAME, 0);
    if (status != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_CREATE_MUTEX_ERR_EID, CFE_EVS_EventType_ERROR, "NOVATEL_OEM615: Create hk mutex error %d", status);
        return status;
    }

    /* 
    ** Create device task
    */
    status = CFE_ES_CreateChildTask(&NOVATEL_OEM615_AppData.DeviceID,
                                    NOVATEL_OEM615_CHILD_TASK_NAME,
                                    NOVATEL_OEM615_ChildTask, 0,
                                    NOVATEL_OEM615_CHILD_TASK_STACK_SIZE,
                                    NOVATEL_OEM615_CHILD_TASK_PRIORITY, 0);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_CHILDTASKINIT_ERR_EID, CFE_EVS_EventType_ERROR, "NOVATEL_OEM615: Create device task error %d", status);
        return status;
    }

    /* 
    ** Always reset all counters during application initialization 
    */
    NOVATEL_OEM615_ResetCounters();

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(NOVATEL_OEM615_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "NOVATEL_OEM615 App Initialized. Version %d.%d.%d.%d",
                NOVATEL_OEM615_MAJOR_VERSION,
                NOVATEL_OEM615_MINOR_VERSION, 
                NOVATEL_OEM615_REVISION, 
                NOVATEL_OEM615_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("NOVATEL_OEM615: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the NOVATEL_OEM615 command pipe
*/
void NOVATEL_OEM615_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(NOVATEL_OEM615_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the NOVATEL_OEM615_CMD_MID (Message ID)
        */
        case NOVATEL_OEM615_CMD_MID:
            NOVATEL_OEM615_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case NOVATEL_OEM615_REQ_HK_MID:
            NOVATEL_OEM615_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            NOVATEL_OEM615_IncrementCommandErrorCount();
            CFE_EVS_SendEvent(NOVATEL_OEM615_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "NOVATEL_OEM615: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
} 


/*
** Process ground commands
*/
void NOVATEL_OEM615_ProcessGroundCommand(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(NOVATEL_OEM615_AppData.MsgPtr, &MsgId);   

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(NOVATEL_OEM615_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case NOVATEL_OEM615_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (NOVATEL_OEM615_VerifyCmdLength(NOVATEL_OEM615_AppData.MsgPtr, sizeof(NOVATEL_OEM615_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(NOVATEL_OEM615_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "NOVATEL_OEM615: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case NOVATEL_OEM615_RESET_COUNTERS_CC:
            if (NOVATEL_OEM615_VerifyCmdLength(NOVATEL_OEM615_AppData.MsgPtr, sizeof(NOVATEL_OEM615_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                NOVATEL_OEM615_ResetCounters();
            }
            break;

        /*
        ** Enable Command
        */
        case NOVATEL_OEM615_ENABLE_CC:
            if (NOVATEL_OEM615_VerifyCmdLength(NOVATEL_OEM615_AppData.MsgPtr, sizeof(NOVATEL_OEM615_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(NOVATEL_OEM615_CMD_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "NOVATEL_OEM615: Enable command received");
                NOVATEL_OEM615_Enable();
            }
            break;

        /*
        ** Disable Command
        */
        case NOVATEL_OEM615_DISABLE_CC:
            if (NOVATEL_OEM615_VerifyCmdLength(NOVATEL_OEM615_AppData.MsgPtr, sizeof(NOVATEL_OEM615_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(NOVATEL_OEM615_CMD_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "NOVATEL_OEM615: Disable command received");
                NOVATEL_OEM615_Disable();
            }
            break;

        /*
        ** Log Command
        */
        case NOVATEL_OEM615_LOG_CC:
            if (NOVATEL_OEM615_VerifyCmdLength(NOVATEL_OEM615_AppData.MsgPtr, sizeof(NOVATEL_OEM615_Log_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(NOVATEL_OEM615_CMD_SERIALCONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "NOVATEL_OEM615: Log command received - %d, %d", ((NOVATEL_OEM615_Log_cmd_t*) NOVATEL_OEM615_AppData.MsgPtr)->LogType, ((NOVATEL_OEM615_Log_cmd_t*) NOVATEL_OEM615_AppData.MsgPtr)->PeriodOption);
                NOVATEL_OEM615_SafeCommandDeviceCustom(NOVATEL_OEM615_LOG_CC, ((NOVATEL_OEM615_Log_cmd_t*) NOVATEL_OEM615_AppData.MsgPtr)->LogType, ((NOVATEL_OEM615_Log_cmd_t*) NOVATEL_OEM615_AppData.MsgPtr)->PeriodOption);
            }
            break;

        /*
        ** Unlog Command
        */
        case NOVATEL_OEM615_UNLOG_CC:
            if (NOVATEL_OEM615_VerifyCmdLength(NOVATEL_OEM615_AppData.MsgPtr, sizeof(NOVATEL_OEM615_Unlog_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(NOVATEL_OEM615_CMD_SERIALCONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "NOVATEL_OEM615: Unlog command received - %d", ((NOVATEL_OEM615_Unlog_cmd_t*) NOVATEL_OEM615_AppData.MsgPtr)->LogType);
                NOVATEL_OEM615_SafeCommandDeviceCustom(NOVATEL_OEM615_UNLOG_CC, ((NOVATEL_OEM615_Unlog_cmd_t*) NOVATEL_OEM615_AppData.MsgPtr)->LogType, -1);
            }
            break;

        /*
        ** Unlog All Command
        */
        case NOVATEL_OEM615_UNLOGALL_CC:
            if (NOVATEL_OEM615_VerifyCmdLength(NOVATEL_OEM615_AppData.MsgPtr, sizeof(NOVATEL_OEM615_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(NOVATEL_OEM615_CMD_SERIALCONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "NOVATEL_OEM615: Unlog all command received");
                NOVATEL_OEM615_SafeCommandDeviceCustom(NOVATEL_OEM615_UNLOGALL_CC, -1, -1);
            }
            break;

        /*
        ** Serial Config Command
        */
        case NOVATEL_OEM615_SERIALCONFIG_CC:
            if (NOVATEL_OEM615_VerifyCmdLength(NOVATEL_OEM615_AppData.MsgPtr, sizeof(NOVATEL_OEM615_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(NOVATEL_OEM615_CMD_SERIALCONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "NOVATEL_OEM615: Serial config command received");
                NOVATEL_OEM615_SafeCommandDeviceCustom(NOVATEL_OEM615_SERIALCONFIG_CC, -1, -1);
            }
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            NOVATEL_OEM615_IncrementCommandErrorCount();
            CFE_EVS_SendEvent(NOVATEL_OEM615_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 


/*
** Process Telemetry Request - Triggered in response to a telemetery request
** TODO: Add additional telemetry required by the specific component
*/
void NOVATEL_OEM615_ProcessTelemetryRequest(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(NOVATEL_OEM615_AppData.MsgPtr, &MsgId);   

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(NOVATEL_OEM615_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case NOVATEL_OEM615_REQ_HK_TLM:
            NOVATEL_OEM615_ReportHousekeeping();
            break;

        case NOVATEL_OEM615_REQ_DATA_TLM:
            NOVATEL_OEM615_ReportDeviceTelemetry();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            NOVATEL_OEM615_IncrementCommandErrorCount();
            CFE_EVS_SendEvent(NOVATEL_OEM615_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
void NOVATEL_OEM615_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (NOVATEL_OEM615_GetDeviceEnabledStatus() == NOVATEL_OEM615_DEVICE_ENABLED)
    {
        status = NOVATEL_OEM615_SafeRequestHK((NOVATEL_OEM615_Device_HK_tlm_t*) &NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceHK);
        if (status == OS_SUCCESS)
        {
            NOVATEL_OEM615_IncrementDeviceCount();
        }
        else
        {
            NOVATEL_OEM615_IncrementDeviceErrorCount();
            // This spams the NOS3 FSW window when uncommented because there usually isn't any data available to be read, and that returns an error.
            /*
            CFE_EVS_SendEvent(NOVATEL_OEM615_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "NOVATEL_OEM615: Request device HK reported error %d", status);
            */
        }
    }
    /* Intentionally do not report errors if disabled */

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &NOVATEL_OEM615_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &NOVATEL_OEM615_AppData.HkTelemetryPkt, true);
    return;
}


/*
** Collect and Report Device Telemetry
*/
void NOVATEL_OEM615_ReportDeviceTelemetry(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (NOVATEL_OEM615_GetDeviceEnabledStatus() == NOVATEL_OEM615_DEVICE_ENABLED)
    {
        status = NOVATEL_OEM615_SafeRequestData((NOVATEL_OEM615_Device_Data_tlm_t*) &NOVATEL_OEM615_AppData.DevicePkt.Novatel_oem615);
        if (status == OS_SUCCESS)
        {
            NOVATEL_OEM615_IncrementDeviceCount();
            /* Time stamp and publish data telemetry */
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &NOVATEL_OEM615_AppData.DevicePkt);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &NOVATEL_OEM615_AppData.DevicePkt, true);
        }
        else
        {
            NOVATEL_OEM615_IncrementDeviceErrorCount();
            CFE_EVS_SendEvent(NOVATEL_OEM615_REQ_DATA_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "NOVATEL_OEM615: Request device data reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */
    return;
}


/*
** Enable Component
** TODO: Edit for your specific component implementation
*/
void NOVATEL_OEM615_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is disabled */
    if (NOVATEL_OEM615_GetDeviceEnabledStatus() == NOVATEL_OEM615_DEVICE_DISABLED)
    {
        /* Open device specific protocols */
        status = uart_init_port(&NOVATEL_OEM615_AppData.Novatel_oem615Uart);
        if (status == OS_SUCCESS)
        {
            NOVATEL_OEM615_IncrementDeviceCount();
            NOVATEL_OEM615_SetDeviceEnabledStatus(NOVATEL_OEM615_DEVICE_ENABLED);
            CFE_EVS_SendEvent(NOVATEL_OEM615_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "NOVATEL_OEM615: Device enabled");
        }
        else
        {
            NOVATEL_OEM615_IncrementDeviceErrorCount();
            CFE_EVS_SendEvent(NOVATEL_OEM615_UART_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "NOVATEL_OEM615: UART port initialization error %d", status);
        }
    }
    else
    {
        NOVATEL_OEM615_IncrementDeviceErrorCount();
        CFE_EVS_SendEvent(NOVATEL_OEM615_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, "NOVATEL_OEM615: Device enable failed, already enabled");
    }
    return;
}


/*
** Disable Component
** TODO: Edit for your specific component implementation
*/
void NOVATEL_OEM615_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (NOVATEL_OEM615_GetDeviceEnabledStatus() == NOVATEL_OEM615_DEVICE_ENABLED)
    {
        /* Open device specific protocols */
        status = uart_close_port(&NOVATEL_OEM615_AppData.Novatel_oem615Uart);
        if (status == OS_SUCCESS)
        {
            NOVATEL_OEM615_IncrementDeviceCount();
            NOVATEL_OEM615_SetDeviceEnabledStatus(NOVATEL_OEM615_DEVICE_DISABLED);
            CFE_EVS_SendEvent(NOVATEL_OEM615_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "NOVATEL_OEM615: Device disabled");
        }
        else
        {
            NOVATEL_OEM615_IncrementDeviceErrorCount();
            CFE_EVS_SendEvent(NOVATEL_OEM615_UART_CLOSE_ERR_EID, CFE_EVS_EventType_ERROR, "NOVATEL_OEM615: UART port close error %d", status);
        }
    }
    else
    {
        NOVATEL_OEM615_IncrementDeviceErrorCount();
        CFE_EVS_SendEvent(NOVATEL_OEM615_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR, "NOVATEL_OEM615: Device disable failed, already disabled");
    }
    return;
}


/*
** Verify command packet length matches expected
*/
int32 NOVATEL_OEM615_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;
    
    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        NOVATEL_OEM615_IncrementCommandCount();
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(NOVATEL_OEM615_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        NOVATEL_OEM615_IncrementCommandErrorCount();
    }
    return status;
} 

/*
** Safely command device using NOVATEL-specific commands (LOG, UNLOG, UNLOGALL, SERIALCONFIG)
*/
int32 NOVATEL_OEM615_SafeCommandDeviceCustom(uint8_t cmd_code, int8_t log_type, int8_t period_option)
{
    uint32 status;
    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        status = NOVATEL_OEM615_CommandDeviceCustom(&NOVATEL_OEM615_AppData.Novatel_oem615Uart, cmd_code, log_type, period_option);

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        status = CFE_ES_RunStatus_APP_ERROR;
    }
    return status;
}

/*
** Safely request HK
*/
int32 NOVATEL_OEM615_SafeRequestHK(NOVATEL_OEM615_Device_HK_tlm_t* data)
{
    uint32 status;

    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        status = NOVATEL_OEM615_RequestHK(&NOVATEL_OEM615_AppData.Novatel_oem615Uart, data);

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_REQUEST_HK_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Request device HK reported error obtaining mutex");
        status = CFE_ES_RunStatus_APP_ERROR;
    }
    return status;
}

/*
** Safely request data
*/
int32 NOVATEL_OEM615_SafeRequestData(NOVATEL_OEM615_Device_Data_tlm_t* data)
{
    uint32 status;

    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        status = NOVATEL_OEM615_RequestData(&NOVATEL_OEM615_AppData.Novatel_oem615Uart, data);

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_REQUEST_DATA_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Request device data reported error obtaining mutex");
        status = CFE_ES_RunStatus_APP_ERROR;
    }
    return status;
}

int32 NOVATEL_OEM615_ChildProcessRequestData(NOVATEL_OEM615_Device_Data_tlm_t* data)
{
    uint32 status;

    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        //status = NOVATEL_OEM615_ChildProcessReadData(&NOVATEL_OEM615_AppData.Novatel_oem615Uart, data);
        status = NOVATEL_OEM615_RequestData(&NOVATEL_OEM615_AppData.Novatel_oem615Uart, data); //IRT CSS

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_REQUEST_DATA_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Request device data for child task reported error obtaining mutex");
        status = CFE_ES_RunStatus_APP_ERROR;
    }
    return status;
}



/*
** Safely increment command counter
*/
void NOVATEL_OEM615_IncrementCommandCount(void)
{
    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        NOVATEL_OEM615_AppData.HkTelemetryPkt.CommandCount++;

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_INC_CMD_CNT_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Increment command count reported error obtaining mutex");
    }
    return;
}

/*
** Safely increment command error counter
*/
void NOVATEL_OEM615_IncrementCommandErrorCount(void)
{
    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        NOVATEL_OEM615_AppData.HkTelemetryPkt.CommandErrorCount++;

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_INC_CMD_ERR_CNT_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Increment command error count reported error obtaining mutex");
    }
    return;
}

/*
** Safely increment device counter
*/
void NOVATEL_OEM615_IncrementDeviceCount(void)
{
    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceCount++;

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_INC_DEV_CNT_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Increment device count reported error obtaining mutex");
    }
    return;
}

/*
** Safely increment device error counter
*/
void NOVATEL_OEM615_IncrementDeviceErrorCount(void)
{
    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceErrorCount++;

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_INC_DEV_ERR_CNT_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Increment device error count reported error obtaining mutex");
    }
    return;
}

/*
** Reset all global counter variables
*/
void NOVATEL_OEM615_ResetCounters(void)
{
    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        NOVATEL_OEM615_AppData.HkTelemetryPkt.CommandErrorCount = 0;
        NOVATEL_OEM615_AppData.HkTelemetryPkt.CommandCount = 0;
        NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceErrorCount = 0;
        NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceCount = 0;

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
        CFE_EVS_SendEvent(NOVATEL_OEM615_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "NOVATEL_OEM615: RESET counters command received");
    }
    return;
} 

/*
** Safe Get Run Status
*/
int32 NOVATEL_OEM615_GetRunStatus(void)
{
    uint32 status;

    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        status = NOVATEL_OEM615_AppData.RunStatus;

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_GET_RUN_STATUS_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Get run status reported error obtaining mutex");
        status = CFE_ES_RunStatus_APP_ERROR;
    }
    return status;
}

/*
** Safe Set Run Status
*/
void NOVATEL_OEM615_SetRunStatus(uint32 status)
{
    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        NOVATEL_OEM615_AppData.RunStatus = status;

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_SET_RUN_STATUS_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Set run status reported error obtaining mutex");
    }
    return;
}

/*
** Safe Get Device Enable Status
*/
int32 NOVATEL_OEM615_GetDeviceEnabledStatus(void)
{
    uint32 status;

    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        status = NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceEnabled;

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_GET_DEV_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Get device enabled status reported error obtaining mutex");
        status = CFE_ES_RunStatus_APP_ERROR;
    }
    return status;
}

/*
** Safe Set Device Enable Status
*/
void NOVATEL_OEM615_SetDeviceEnabledStatus(uint32 status)
{
    if (OS_MutSemTake(NOVATEL_OEM615_AppData.HkDataMutex) == OS_SUCCESS)
    {
        NOVATEL_OEM615_AppData.HkTelemetryPkt.DeviceEnabled = status;

        OS_MutSemGive(NOVATEL_OEM615_AppData.HkDataMutex);
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_MUT_SET_DEV_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, 
                "NOVATEL_OEM615: Set device enabled status reported error obtaining mutex");
    }
    return;
}
