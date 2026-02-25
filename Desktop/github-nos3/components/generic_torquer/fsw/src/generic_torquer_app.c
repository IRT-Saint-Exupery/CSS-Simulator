/*******************************************************************************
** File: generic_torquer_app.c
**
** Purpose:
**   This file contains the source code for the GENERIC_TORQUER application.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_torquer_app.h"


/*
** Global Data
*/
GENERIC_TORQUER_AppData_t GENERIC_TORQUER_AppData;

/*
** Application entry point and main process loop
*/
void TORQUER_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(GENERIC_TORQUER_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = GENERIC_TORQUER_AppInit();
    if (status != CFE_SUCCESS)
    {
        GENERIC_TORQUER_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&GENERIC_TORQUER_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(GENERIC_TORQUER_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&GENERIC_TORQUER_AppData.MsgPtr,  GENERIC_TORQUER_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(GENERIC_TORQUER_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            GENERIC_TORQUER_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(GENERIC_TORQUER_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_TORQUER: SB Pipe Read Error = %d", (int) status);
            GENERIC_TORQUER_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Disable component, which cleans up the interface, upon exit
    */
    GENERIC_TORQUER_Disable();
    trq_close(&GENERIC_TORQUER_AppData.trqDevice[0]); /* TODO: Move this into disable when close issue resolved in HWLIB */

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(GENERIC_TORQUER_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(GENERIC_TORQUER_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 GENERIC_TORQUER_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    GENERIC_TORQUER_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);    /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_TORQUER: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&GENERIC_TORQUER_AppData.CmdPipe, GENERIC_TORQUER_PIPE_DEPTH, "GENERIC_TRQ_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_TORQUER_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_TORQUER_CMD_MID), GENERIC_TORQUER_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_TORQUER_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            GENERIC_TORQUER_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_TORQUER_REQ_HK_MID), GENERIC_TORQUER_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_TORQUER_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            GENERIC_TORQUER_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the GENERIC_TORQUER_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_TORQUER_AppData.HkTelemetryPkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_TORQUER_HK_TLM_MID),
                   GENERIC_TORQUER_HK_TLM_LNGTH);

    /* 
    ** Always reset all counters during application initialization 
    */
    GENERIC_TORQUER_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_TORQUER_DEVICE_DISABLED;

    /*
    ** Initialize torquer interfaces
    */ 
    GENERIC_TORQUER_AppData.HkTelemetryPkt.TorquerPeriod = GENERIC_TORQUER_CFG_PERIOD;
    for(uint8_t i = 0; i < 3; i++)
    {
        GENERIC_TORQUER_AppData.HkTelemetryPkt.TrqInfo[i].Direction = 0;
        GENERIC_TORQUER_AppData.HkTelemetryPkt.TrqInfo[i].PercentOn = 0;

        GENERIC_TORQUER_AppData.trqDevice[i].trq_num = i;
        GENERIC_TORQUER_AppData.trqDevice[i].timer_period_ns = GENERIC_TORQUER_CFG_PERIOD;
        GENERIC_TORQUER_AppData.trqDevice[i].timerfd = 0;
        GENERIC_TORQUER_AppData.trqDevice[i].direction_pin_fd = 0;
        GENERIC_TORQUER_AppData.trqDevice[i].timer_high_ns = 0;
        GENERIC_TORQUER_AppData.trqDevice[i].positive_direction = false;
        GENERIC_TORQUER_AppData.trqDevice[i].enabled = false;

        trq_init(&GENERIC_TORQUER_AppData.trqDevice[i]);  /* TODO: Move this into enable once HWLIB issue resolved */
    }

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(GENERIC_TORQUER_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "GENERIC_TORQUER App Initialized. Version %d.%d.%d.%d",
                GENERIC_TORQUER_MAJOR_VERSION,
                GENERIC_TORQUER_MINOR_VERSION, 
                GENERIC_TORQUER_REVISION, 
                GENERIC_TORQUER_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_TORQUER: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the GENERIC_TORQUER command pipe
*/
void GENERIC_TORQUER_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(GENERIC_TORQUER_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the GENERIC_TORQUER_CMD_MID (Message ID)
        */
        case GENERIC_TORQUER_CMD_MID:
            GENERIC_TORQUER_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case GENERIC_TORQUER_REQ_HK_MID:
            GENERIC_TORQUER_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            GENERIC_TORQUER_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_TORQUER_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "GENERIC_TORQUER: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
} 


/*
** Process ground commands
*/
void GENERIC_TORQUER_ProcessGroundCommand(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(GENERIC_TORQUER_AppData.MsgPtr, &MsgId);

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(GENERIC_TORQUER_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case GENERIC_TORQUER_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (GENERIC_TORQUER_VerifyCmdLength(GENERIC_TORQUER_AppData.MsgPtr, sizeof(GENERIC_TORQUER_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(GENERIC_TORQUER_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_TORQUER: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case GENERIC_TORQUER_RESET_COUNTERS_CC:
            if (GENERIC_TORQUER_VerifyCmdLength(GENERIC_TORQUER_AppData.MsgPtr, sizeof(GENERIC_TORQUER_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_TORQUER_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_TORQUER: RESET counters command received");
                GENERIC_TORQUER_ResetCounters();
            }
            break;

        /*
        ** Enable Command
        */
        case GENERIC_TORQUER_ENABLE_CC:
            if (GENERIC_TORQUER_VerifyCmdLength(GENERIC_TORQUER_AppData.MsgPtr, sizeof(GENERIC_TORQUER_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_TORQUER_CMD_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_TORQUER: Enable command received");
                GENERIC_TORQUER_Enable();
            }
            break;

        /*
        ** Disable Command
        */
        case GENERIC_TORQUER_DISABLE_CC:
            if (GENERIC_TORQUER_VerifyCmdLength(GENERIC_TORQUER_AppData.MsgPtr, sizeof(GENERIC_TORQUER_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_TORQUER_CMD_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_TORQUER: Disable command received");
                GENERIC_TORQUER_Disable();
            }
            break;

        /*
        ** Set Configuration Command
        */
        case GENERIC_TORQUER_CONFIG_CC:
            if (GENERIC_TORQUER_VerifyCmdLength(GENERIC_TORQUER_AppData.MsgPtr, sizeof(GENERIC_TORQUER_Percent_On_cmd_t)) == OS_SUCCESS)
            {
                //CFE_EVS_SendEvent(GENERIC_TORQUER_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_TORQUER: Configuration command received");
                status = GENERIC_TORQUER_PercentOn(GENERIC_TORQUER_AppData.MsgPtr);
                if (status == OS_SUCCESS)
                {
                    GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceCount++;
                }
                else
                {
                    GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceErrorCount++;
                }
            }
            break;

        /*
        ** Set All Configuration Command
        */
        case GENERIC_TORQUER_CONFIG_ALL_CC:
            if (GENERIC_TORQUER_VerifyCmdLength(GENERIC_TORQUER_AppData.MsgPtr, sizeof(GENERIC_TORQUER_All_Percent_On_cmd_t)) == OS_SUCCESS)
            {
                //CFE_EVS_SendEvent(GENERIC_TORQUER_CMD_CONFIG_ALL_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_TORQUER: Configuration command received");
                status = GENERIC_TORQUER_AllPercentOn(GENERIC_TORQUER_AppData.MsgPtr);
                if (status == OS_SUCCESS)
                {
                    GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceCount++;
                }
                else
                {
                    GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceErrorCount++;
                }
            }
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_TORQUER_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_TORQUER_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_TORQUER: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 


/*
** Process Telemetry Request - Triggered in response to a telemetery request
*/
void GENERIC_TORQUER_ProcessTelemetryRequest(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(GENERIC_TORQUER_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(GENERIC_TORQUER_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case GENERIC_TORQUER_REQ_HK_TLM:
            GENERIC_TORQUER_ReportHousekeeping();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_TORQUER_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_TORQUER_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_TORQUER: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
void GENERIC_TORQUER_ReportHousekeeping(void)
{
    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_TORQUER_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_TORQUER_AppData.HkTelemetryPkt, true);
    return;
}


/*
** Reset all global counter variables
*/
void GENERIC_TORQUER_ResetCounters(void)
{
    GENERIC_TORQUER_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    GENERIC_TORQUER_AppData.HkTelemetryPkt.CommandCount = 0;
    GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceErrorCount = 0;
    GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceCount = 0;
    return;
} 


/*
** Enable Component
*/
void GENERIC_TORQUER_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is disabled */
    if (GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_TORQUER_DEVICE_DISABLED)
    {
        /*
        ** Initialize hardware interface data
        */
        for(uint8_t i = 0; i < 3; i++)
        {
            status += trq_command(&GENERIC_TORQUER_AppData.trqDevice[i], 0, 0);
            GENERIC_TORQUER_AppData.HkTelemetryPkt.TrqInfo[i].Direction = 0;
            GENERIC_TORQUER_AppData.HkTelemetryPkt.TrqInfo[i].PercentOn = 0;
        }

        if (status == OS_SUCCESS)
        {
            GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceCount++;
            GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_TORQUER_DEVICE_ENABLED;
            CFE_EVS_SendEvent(GENERIC_TORQUER_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_TORQUER: Device enabled");
        }
        else
        {
            GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_TORQUER_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_TORQUER: Torquer initialization error %d", status);
        }
    }
    else
    {
        GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_TORQUER_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_TORQUER: Device enable failed, already enabled");
    }
    return;
}


/*
** Disable Component
*/
void GENERIC_TORQUER_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_TORQUER_DEVICE_ENABLED)
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            /* Set to zero  */
            trq_command(&GENERIC_TORQUER_AppData.trqDevice[i], 0, 0);
            GENERIC_TORQUER_AppData.HkTelemetryPkt.TrqInfo[i].Direction = 0;
            GENERIC_TORQUER_AppData.HkTelemetryPkt.TrqInfo[i].PercentOn = 0;
        }        
        GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceCount++;
        GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_TORQUER_DEVICE_DISABLED;
        CFE_EVS_SendEvent(GENERIC_TORQUER_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_TORQUER: Device disabled");
    }
    else
    {
        GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_TORQUER_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_TORQUER: Device disable failed, already disabled");
    }
    return;
}

/*
** Configure torquer using hwlib
*/
int32 GENERIC_TORQUER_Config(uint8 num, uint8 percent, uint8 dir)
{
    int32 status = OS_SUCCESS;

    status = trq_command(&GENERIC_TORQUER_AppData.trqDevice[num], percent, dir);        
    if (status == OS_SUCCESS)
    {
        GENERIC_TORQUER_AppData.HkTelemetryPkt.TrqInfo[num].PercentOn = percent;
        GENERIC_TORQUER_AppData.HkTelemetryPkt.TrqInfo[num].Direction = dir;
        GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceCount++;
    }
    else
    {
        GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_TORQUER_CONFIG_ERR_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_TORQUER: Failed to configure device %d", num);    
    }
    return status;
}


/*
** Configure a specific torquer
*/
int32 GENERIC_TORQUER_PercentOn(CFE_MSG_Message_t * msg)
{
    int32 status = OS_SUCCESS;
    GENERIC_TORQUER_Percent_On_cmd_t* percent_cmd_ptr = (GENERIC_TORQUER_Percent_On_cmd_t*) msg;

    /* Check that device is enabled */
    if (GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_TORQUER_DEVICE_ENABLED)
    {
        GENERIC_TORQUER_Config(percent_cmd_ptr->TrqNum, percent_cmd_ptr->PercentOn, percent_cmd_ptr->Direction);
    }
    return status;
}


/*
** Configure all torquers at once
*/
int32 GENERIC_TORQUER_AllPercentOn(CFE_MSG_Message_t * msg)
{
    int32 status = OS_SUCCESS;
    GENERIC_TORQUER_All_Percent_On_cmd_t* all_percent_cmd_ptr = (GENERIC_TORQUER_All_Percent_On_cmd_t*) msg;

    /* Check that device is enabled */
    if (GENERIC_TORQUER_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_TORQUER_DEVICE_ENABLED)
    {
        GENERIC_TORQUER_Config(0, all_percent_cmd_ptr->PercentOn_0, all_percent_cmd_ptr->Direction_0);
        GENERIC_TORQUER_Config(1, all_percent_cmd_ptr->PercentOn_1, all_percent_cmd_ptr->Direction_1);
        GENERIC_TORQUER_Config(2, all_percent_cmd_ptr->PercentOn_2, all_percent_cmd_ptr->Direction_2);
    }
    return status;
}


/*
** Verify command packet length matches expected
*/
int32 GENERIC_TORQUER_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;
    
    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        GENERIC_TORQUER_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(GENERIC_TORQUER_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        GENERIC_TORQUER_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
} 
