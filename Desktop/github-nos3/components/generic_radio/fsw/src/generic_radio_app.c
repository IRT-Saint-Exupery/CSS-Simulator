/*******************************************************************************
** File: generic_radio_app.c
**
** Purpose:
**   This file contains the source code for the GENERIC_RADIO application.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_radio_app.h"


/*
** Global Data
*/
GENERIC_RADIO_AppData_t GENERIC_RADIO_AppData;


/*
** Application entry point and main process loop
*/
void RADIO_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(GENERIC_RADIO_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = GENERIC_RADIO_AppInit();
    if (status != CFE_SUCCESS)
    {
        GENERIC_RADIO_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&GENERIC_RADIO_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(GENERIC_RADIO_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&GENERIC_RADIO_AppData.MsgPtr,  GENERIC_RADIO_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(GENERIC_RADIO_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            GENERIC_RADIO_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(GENERIC_RADIO_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_RADIO: SB Pipe Read Error = %d", (int) status);
            GENERIC_RADIO_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Clean up interface upon exit
    */
    socket_close(&GENERIC_RADIO_AppData.RadioSocket);

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(GENERIC_RADIO_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(GENERIC_RADIO_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 GENERIC_RADIO_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    GENERIC_RADIO_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);    /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_RADIO: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&GENERIC_RADIO_AppData.CmdPipe, GENERIC_RADIO_PIPE_DEPTH, "RADIO_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_RADIO_CMD_MID), GENERIC_RADIO_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            GENERIC_RADIO_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_RADIO_REQ_HK_MID), GENERIC_RADIO_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            GENERIC_RADIO_REQ_HK_MID, (unsigned int) status);
        return status;
    }


    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the GENERIC_RADIO_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_RADIO_AppData.HkTelemetryPkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_RADIO_HK_TLM_MID),
                   GENERIC_RADIO_HK_TLM_LNGTH);


    /* 
    ** Always reset all counters during application initialization 
    */
    GENERIC_RADIO_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceHK.DeviceCounter = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceHK.DeviceConfig = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceHK.ProxSignal = 0;

    /*
    ** Initialize sockets
    */
    GENERIC_RADIO_AppData.RadioSocket.sockfd = -1;
    GENERIC_RADIO_AppData.RadioSocket.port_num = GENERIC_RADIO_CFG_UDP_RADIO_TO_FSW;
    GENERIC_RADIO_AppData.RadioSocket.ip_address = GENERIC_RADIO_CFG_FSW_IP;
    GENERIC_RADIO_AppData.RadioSocket.address_family = ip_ver_4;
    GENERIC_RADIO_AppData.RadioSocket.type = dgram;
    GENERIC_RADIO_AppData.RadioSocket.category = client;
    GENERIC_RADIO_AppData.RadioSocket.block = false;
    GENERIC_RADIO_AppData.RadioSocket.keep_alive = false;
    GENERIC_RADIO_AppData.RadioSocket.created = false;
    GENERIC_RADIO_AppData.RadioSocket.bound = false;
    GENERIC_RADIO_AppData.RadioSocket.listening = false;
    GENERIC_RADIO_AppData.RadioSocket.connected = false;

    status = socket_create(&GENERIC_RADIO_AppData.RadioSocket);
    if (status != SOCKET_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_SOCK_OPEN_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_RADIO: Radio interface create error %d", status);
        return status;
    }

    GENERIC_RADIO_AppData.ProxySocket.sockfd = -1;
    GENERIC_RADIO_AppData.ProxySocket.port_num = GENERIC_RADIO_CFG_UDP_PROX_TO_FSW;
    GENERIC_RADIO_AppData.ProxySocket.ip_address = GENERIC_RADIO_CFG_FSW_IP;
    GENERIC_RADIO_AppData.ProxySocket.address_family = ip_ver_4;
    GENERIC_RADIO_AppData.ProxySocket.type = dgram;
    GENERIC_RADIO_AppData.ProxySocket.category = client;
    GENERIC_RADIO_AppData.ProxySocket.block = false;
    GENERIC_RADIO_AppData.ProxySocket.keep_alive = false;
    GENERIC_RADIO_AppData.ProxySocket.created = false;
    GENERIC_RADIO_AppData.ProxySocket.bound = false;
    GENERIC_RADIO_AppData.ProxySocket.listening = false;
    GENERIC_RADIO_AppData.ProxySocket.connected = false;

    status = socket_create(&GENERIC_RADIO_AppData.ProxySocket);
    if (status != SOCKET_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_RADIO_PROX_OPEN_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_RADIO: Proximity interface create error %d", status);
        return status;
    }


    /* 
    ** Start device task
    */
    status = CFE_ES_CreateChildTask(&GENERIC_RADIO_AppData.DeviceID,
                                    GENERIC_RADIO_DEVICE_NAME,
                                    GENERIC_RADIO_ProxyTask, 0,
                                    GENERIC_RADIO_DEVICE_STACK_SIZE,
                                    GENERIC_RADIO_DEVICE_PRIORITY, 0);
    if (status != CFE_SUCCESS)
    {
        return status;
    }

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(GENERIC_RADIO_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "GENERIC_RADIO App Initialized. Version %d.%d.%d.%d",
                GENERIC_RADIO_MAJOR_VERSION,
                GENERIC_RADIO_MINOR_VERSION, 
                GENERIC_RADIO_REVISION, 
                GENERIC_RADIO_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_RADIO: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the GENERIC_RADIO command pipe
*/
void GENERIC_RADIO_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(GENERIC_RADIO_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the GENERIC_RADIO_CMD_MID (Message ID)
        */
        case GENERIC_RADIO_CMD_MID:
            GENERIC_RADIO_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case GENERIC_RADIO_REQ_HK_MID:
            GENERIC_RADIO_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            GENERIC_RADIO_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_RADIO_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "GENERIC_RADIO: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
} 


/*
** Process ground commands
*/
void GENERIC_RADIO_ProcessGroundCommand(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;
    size_t prox_size = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(GENERIC_RADIO_AppData.MsgPtr, &MsgId);

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(GENERIC_RADIO_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case GENERIC_RADIO_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (GENERIC_RADIO_VerifyCmdLength(GENERIC_RADIO_AppData.MsgPtr, sizeof(GENERIC_RADIO_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(GENERIC_RADIO_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_RADIO: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case GENERIC_RADIO_RESET_COUNTERS_CC:
            if (GENERIC_RADIO_VerifyCmdLength(GENERIC_RADIO_AppData.MsgPtr, sizeof(GENERIC_RADIO_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_RADIO_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_RADIO: RESET counters command received");
                GENERIC_RADIO_ResetCounters();
            }
            break;

        /*
        ** Set Configuration Command
        ** Note that this is an example of a command that has additional arguments
        */
        case GENERIC_RADIO_CONFIG_CC:
            if (GENERIC_RADIO_VerifyCmdLength(GENERIC_RADIO_AppData.MsgPtr, sizeof(GENERIC_RADIO_Config_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_RADIO_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_RADIO: Configuration command received");
                /* Command device to send HK */
                status = GENERIC_RADIO_SetConfiguration(&GENERIC_RADIO_AppData.RadioSocket,
                     ((GENERIC_RADIO_Config_cmd_t*) GENERIC_RADIO_AppData.MsgPtr)->DeviceCfg);
                if (status == OS_SUCCESS)
                {
                    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceCount++;
                }
                else
                {
                    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceErrorCount++;
                }
            }
            break;

        /*
        ** Radio Proximity Forward Command
        ** Note that no verifications are performed prior to forwarding
        */
        case GENERIC_RADIO_PROXIMITY_CC:
            CFE_MSG_GetSize((CFE_MSG_Message_t*) ((GENERIC_RADIO_Proximity_cmd_t*) GENERIC_RADIO_AppData.MsgPtr)->Payload, &prox_size);
            status = GENERIC_RADIO_ProximityForward(&GENERIC_RADIO_AppData.ProxySocket, 
                ((GENERIC_RADIO_Proximity_cmd_t*) GENERIC_RADIO_AppData.MsgPtr)->SCID,
                ((GENERIC_RADIO_Proximity_cmd_t*) GENERIC_RADIO_AppData.MsgPtr)->Payload,
                prox_size);
            if (status == OS_SUCCESS)
            {
                GENERIC_RADIO_AppData.HkTelemetryPkt.ForwardCount++;
            }
            else
            {
                GENERIC_RADIO_AppData.HkTelemetryPkt.ForwardErrorCount++;
            }
            break;


        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_RADIO_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_RADIO_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_RADIO: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 


/*
** Process Telemetry Request - Triggered in response to a telemetery request
*/
void GENERIC_RADIO_ProcessTelemetryRequest(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(GENERIC_RADIO_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(GENERIC_RADIO_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case GENERIC_RADIO_REQ_HK_TLM:
            GENERIC_RADIO_ReportHousekeeping();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_RADIO_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_RADIO_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_RADIO: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
void GENERIC_RADIO_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    status = GENERIC_RADIO_RequestHK(&GENERIC_RADIO_AppData.RadioSocket, (GENERIC_RADIO_Device_HK_tlm_t*) &GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceHK);
    if (status == OS_SUCCESS)
    {
        GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceCount++;
    }
    else
    {
        GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_RADIO_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_RADIO: Request device HK reported error %d", status);
    }

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_RADIO_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_RADIO_AppData.HkTelemetryPkt, true);
    return;
}


/*
** Reset all global counter variables
*/
void GENERIC_RADIO_ResetCounters(void)
{
    GENERIC_RADIO_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.CommandCount = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceErrorCount = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.DeviceCount = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.ForwardErrorCount = 0;
    GENERIC_RADIO_AppData.HkTelemetryPkt.ForwardCount = 0;
    return;
} 


/*
** Verify command packet length matches expected
*/
int32 GENERIC_RADIO_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;

    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        GENERIC_RADIO_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(GENERIC_RADIO_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        GENERIC_RADIO_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
} 

/*
** Proxy Read Task
*/
void GENERIC_RADIO_ProxyTask(void)
{
    int32 status = OS_SUCCESS;
    uint8 read_data[GENERIC_RADIO_CFG_PROX_DATA_SIZE] = {0};
    size_t bytes = 0;

    CFE_EVS_SendEvent(GENERIC_RADIO_TASK_REG_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_RADIO: Device task registration complete");

    /*
    ** Device Run Loop
    */
    while (CFE_ES_RunLoop(&GENERIC_RADIO_AppData.RunStatus) == true)
    {
        /* Zero read data */
        CFE_PSP_MemSet(read_data, 0x00, GENERIC_RADIO_CFG_PROX_DATA_SIZE);
        bytes = 0;

        /* Read */
        status = socket_recv(&GENERIC_RADIO_AppData.ProxySocket, read_data, sizeof(read_data), &bytes);
        
        if (status != SOCKET_TRY_AGAIN)
        {
            #ifdef GENERIC_RADIO_CFG_DEBUG
                OS_printf("GENERIC_RADIO_ProxyTask reported status %d and received[%d]: ", status, bytes);
                for(int i = 0; i < (int) bytes; i++)
                {
                    OS_printf("0x%02x ", read_data[i]);
                }
                OS_printf("\n");
            #endif
        
            /* Publish on software bus assuming all received data is correctly formatted */
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) read_data, true);
        }

        /* Delay between loops */
        OS_TaskDelay(GENERIC_RADIO_DEVICE_MS_LOOP_DELAY);
    }

    socket_close(&GENERIC_RADIO_AppData.ProxySocket);
}
