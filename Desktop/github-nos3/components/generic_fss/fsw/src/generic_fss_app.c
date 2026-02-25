/*******************************************************************************
** File: generic_fss_app.c
**
** Purpose:
**   This file contains the source code for the GENERIC_FSS application.
**
*******************************************************************************/

/*
** Include Files
*/
#include "generic_fss_app.h"
#include "generic_fss_device.h"
#include "generic_fss_events.h"
#include "generic_fss_platform_cfg.h"
#include "generic_fss_perfids.h"
#include "generic_fss_msgids.h"
#include "generic_fss_version.h"


/*
** Global Data
*/
GENERIC_FSS_AppData_t GENERIC_FSS_AppData;

// Forward declarations
static int32 GENERIC_FSS_AppInit(void);
static void  GENERIC_FSS_ProcessCommandPacket(void);
static void  GENERIC_FSS_ProcessGroundCommand(void);
static void  GENERIC_FSS_ProcessTelemetryRequest(void);
static void  GENERIC_FSS_ReportHousekeeping(void);
static void  GENERIC_FSS_ReportDeviceTelemetry(void);
static void  GENERIC_FSS_ResetCounters(void);
static int32 GENERIC_FSS_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);
static void  GENERIC_FSS_Enable(void);
static void  GENERIC_FSS_Disable(void);


/*
** Application entry point and main process loop
*/
void FSS_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(GENERIC_FSS_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = GENERIC_FSS_AppInit();
    if (status != CFE_SUCCESS)
    {
        GENERIC_FSS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&GENERIC_FSS_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(GENERIC_FSS_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&GENERIC_FSS_AppData.MsgPtr,  GENERIC_FSS_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(GENERIC_FSS_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            GENERIC_FSS_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(GENERIC_FSS_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_FSS: SB Pipe Read Error = %d", (int) status);
            GENERIC_FSS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Disable component, which cleans up the interface, upon exit
    */
    GENERIC_FSS_Disable();

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(GENERIC_FSS_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(GENERIC_FSS_AppData.RunStatus);
} 


/* 
** Initialize application
*/
static int32 GENERIC_FSS_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    GENERIC_FSS_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);    /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_FSS: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&GENERIC_FSS_AppData.CmdPipe, GENERIC_FSS_PIPE_DEPTH, "FSS_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_FSS_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_FSS_CMD_MID), GENERIC_FSS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_FSS_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            GENERIC_FSS_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_FSS_REQ_HK_MID), GENERIC_FSS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GENERIC_FSS_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            GENERIC_FSS_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the GENERIC_FSS_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_FSS_AppData.HkTelemetryPkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_FSS_HK_TLM_MID),
                   GENERIC_FSS_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_FSS_AppData.DevicePkt.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_FSS_DEVICE_TLM_MID),
                   GENERIC_FSS_DEVICE_TLM_LNGTH);

    /* 
    ** Always reset all counters during application initialization 
    */
    GENERIC_FSS_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    GENERIC_FSS_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_FSS_DEVICE_DISABLED;

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(GENERIC_FSS_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "GENERIC_FSS App Initialized. Version %d.%d.%d.%d",
                GENERIC_FSS_MAJOR_VERSION,
                GENERIC_FSS_MINOR_VERSION, 
                GENERIC_FSS_REVISION, 
                GENERIC_FSS_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_FSS: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the GENERIC_FSS command pipe
*/
static void GENERIC_FSS_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(GENERIC_FSS_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the GENERIC_FSS_CMD_MID (Message ID)
        */
        case GENERIC_FSS_CMD_MID:
            GENERIC_FSS_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case GENERIC_FSS_REQ_HK_MID:
            GENERIC_FSS_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            GENERIC_FSS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_FSS_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "GENERIC_FSS: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
} 


/*
** Process ground commands
*/
static void GENERIC_FSS_ProcessGroundCommand(void)
{
    //int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(GENERIC_FSS_AppData.MsgPtr, &MsgId);

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(GENERIC_FSS_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case GENERIC_FSS_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (GENERIC_FSS_VerifyCmdLength(GENERIC_FSS_AppData.MsgPtr, sizeof(GENERIC_FSS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(GENERIC_FSS_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_FSS: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case GENERIC_FSS_RESET_COUNTERS_CC:
            if (GENERIC_FSS_VerifyCmdLength(GENERIC_FSS_AppData.MsgPtr, sizeof(GENERIC_FSS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_FSS_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_FSS: RESET counters command received");
                GENERIC_FSS_ResetCounters();
            }
            break;

        /*
        ** Enable Command
        */
        case GENERIC_FSS_ENABLE_CC:
            if (GENERIC_FSS_VerifyCmdLength(GENERIC_FSS_AppData.MsgPtr, sizeof(GENERIC_FSS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_FSS_CMD_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_FSS: Enable command received");
                GENERIC_FSS_Enable();
            }
            break;

        /*
        ** Disable Command
        */
        case GENERIC_FSS_DISABLE_CC:
            if (GENERIC_FSS_VerifyCmdLength(GENERIC_FSS_AppData.MsgPtr, sizeof(GENERIC_FSS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(GENERIC_FSS_CMD_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_FSS: Disable command received");
                GENERIC_FSS_Disable();
            }
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_FSS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_FSS_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_FSS: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 


/*
** Process Telemetry Request - Triggered in response to a telemetery request
*/
static void GENERIC_FSS_ProcessTelemetryRequest(void)
{
    //int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(GENERIC_FSS_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(GENERIC_FSS_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case GENERIC_FSS_REQ_HK_TLM:
            GENERIC_FSS_ReportHousekeeping();
            break;

        case GENERIC_FSS_REQ_DATA_TLM:
            GENERIC_FSS_ReportDeviceTelemetry();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            GENERIC_FSS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(GENERIC_FSS_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "GENERIC_FSS: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
static void GENERIC_FSS_ReportHousekeeping(void)
{
    //int32 status = OS_SUCCESS;

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_FSS_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_FSS_AppData.HkTelemetryPkt, true);
    return;
}


/*
** Collect and Report Device Telemetry
*/
static void GENERIC_FSS_ReportDeviceTelemetry(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_FSS_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_FSS_DEVICE_ENABLED)
    {
        status = GENERIC_FSS_RequestData(&GENERIC_FSS_AppData.Generic_fssSpi, (GENERIC_FSS_Device_Data_tlm_t*) &GENERIC_FSS_AppData.DevicePkt.Generic_fss);
        if (status == OS_SUCCESS)
        {
            GENERIC_FSS_AppData.HkTelemetryPkt.DeviceCount++;
            /* Time stamp and publish data telemetry */
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_FSS_AppData.DevicePkt);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_FSS_AppData.DevicePkt, true);
        }
        else
        {
            GENERIC_FSS_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_FSS_REQ_DATA_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "GENERIC_FSS: Request device data reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */
    return;
}


/*
** Reset all global counter variables
*/
static void GENERIC_FSS_ResetCounters(void)
{
    GENERIC_FSS_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    GENERIC_FSS_AppData.HkTelemetryPkt.CommandCount = 0;
    GENERIC_FSS_AppData.HkTelemetryPkt.DeviceErrorCount = 0;
    GENERIC_FSS_AppData.HkTelemetryPkt.DeviceCount = 0;
    return;
} 

/*
** Verify command packet length matches expected
*/
static int32 GENERIC_FSS_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;

    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of a valid command */
        GENERIC_FSS_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(GENERIC_FSS_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        GENERIC_FSS_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
} 

/*
** Enable Component
*/
static void GENERIC_FSS_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is disabled */
    if (GENERIC_FSS_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_FSS_DEVICE_DISABLED)
    {
        /*
        ** Initialize hardware interface data
        */ 
        GENERIC_FSS_AppData.Generic_fssSpi.deviceString = GENERIC_FSS_CFG_STRING;
        GENERIC_FSS_AppData.Generic_fssSpi.handle = GENERIC_FSS_CFG_HANDLE;
        GENERIC_FSS_AppData.Generic_fssSpi.baudrate = GENERIC_FSS_CFG_BAUD;
        GENERIC_FSS_AppData.Generic_fssSpi.spi_mode = GENERIC_FSS_CFG_SPI_MODE;
        GENERIC_FSS_AppData.Generic_fssSpi.bits_per_word = GENERIC_FSS_CFG_BITS_PER_WORD;
        GENERIC_FSS_AppData.Generic_fssSpi.bus = GENERIC_FSS_CFG_BUS;
        GENERIC_FSS_AppData.Generic_fssSpi.cs = GENERIC_FSS_CFG_CS;

        /* Open device specific protocols */
        status = spi_init_dev(&GENERIC_FSS_AppData.Generic_fssSpi);
        if (status == OS_SUCCESS)
        {
            GENERIC_FSS_AppData.HkTelemetryPkt.DeviceCount++;
            GENERIC_FSS_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_FSS_DEVICE_ENABLED;
            CFE_EVS_SendEvent(GENERIC_FSS_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_FSS: Device enabled");
        }
        else
        {
            GENERIC_FSS_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_FSS_SPI_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_FSS: SPI device initialization error %d", status);
        }
    }
    else
    {
        GENERIC_FSS_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_FSS_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_FSS: Device enable failed, already enabled");
    }
    return;
}


/*
** Disable Component
*/
static void GENERIC_FSS_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (GENERIC_FSS_AppData.HkTelemetryPkt.DeviceEnabled == GENERIC_FSS_DEVICE_ENABLED)
    {
        /* Close device specific protocols */
        status = spi_close_device(&GENERIC_FSS_AppData.Generic_fssSpi);
        if (status == OS_SUCCESS)
        {
            GENERIC_FSS_AppData.HkTelemetryPkt.DeviceCount++;
            GENERIC_FSS_AppData.HkTelemetryPkt.DeviceEnabled = GENERIC_FSS_DEVICE_DISABLED;
            CFE_EVS_SendEvent(GENERIC_FSS_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "GENERIC_FSS: Device disabled");
        }
        else
        {
            GENERIC_FSS_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(GENERIC_FSS_SPI_CLOSE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_FSS: SPI device close error %d", status);
        }
    }
    else
    {
        GENERIC_FSS_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(GENERIC_FSS_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR, "GENERIC_FSS: Device disable failed, already disabled");
    }
    return;
}
