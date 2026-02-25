/*******************************************************************************
** File: 
**  generic_reaction_wheel_app.c
**
** Purpose:
**   This file contains the source code for the Generic Reaction Wheel App.
**
*******************************************************************************/

/*
** Include Files:
*/
#include "generic_reaction_wheel_events.h"
#include "generic_reaction_wheel_version.h"
#include "generic_reaction_wheel_app.h"
#include "hwlib.h"

#include <string.h>

/*
** global data
*/
GENERIC_RW_AppData_t GENERIC_RW_AppData;

static char deviceName0[] = "/dev/tty2";
static char deviceName1[] = "/dev/tty3";
static char deviceName2[] = "/dev/tty4";
static uart_info_t RW_UART[3] = {
    {.deviceString = &deviceName0[0], .handle = 2, .isOpen = PORT_CLOSED, .baud = 115200},
    {.deviceString = &deviceName1[0], .handle = 3, .isOpen = PORT_CLOSED, .baud = 115200},
    {.deviceString = &deviceName2[0], .handle = 4, .isOpen = PORT_CLOSED, .baud = 115200},
};

/* Forward declarations */
static int32_t GetCurrentMomentum(int wheel_number, double *momentum);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/* GENERIC_RW_AppMain() -- Application entry point and main process loop      */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void RW_AppMain( void )
{
    int32  status;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(GENERIC_RW_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = GENERIC_RW_AppInit();
    if (status != CFE_SUCCESS)
    {
        GENERIC_RW_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** GENERIC_RW Runloop
    */
    while (CFE_ES_RunLoop(&GENERIC_RW_AppData.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(GENERIC_RW_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&GENERIC_RW_AppData.MsgPtr,
                               GENERIC_RW_AppData.CommandPipe,
                               CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(GENERIC_RW_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            GENERIC_RW_ProcessCommandPacket(GENERIC_RW_AppData.MsgPtr);
        }
        else
        {
            CFE_EVS_SendEvent(GENERIC_RW_PIPE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "GENERIC_RW APP: SB Pipe Read Error, App Will Exit");

            GENERIC_RW_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }

    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(GENERIC_RW_APP_PERF_ID);

    CFE_ES_ExitApp(GENERIC_RW_AppData.RunStatus);

} /* End of GENERIC_RW_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* GENERIC_RW_AppInit() --  initialization                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 GENERIC_RW_AppInit( void )
{
    int32    status;

    GENERIC_RW_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    GENERIC_RW_AppData.HkTlm.Payload.CommandCounter = 0;
    GENERIC_RW_AppData.HkTlm.Payload.CommandErrorCounter = 0;

    /*
    ** Initialize app configuration data
    */
    GENERIC_RW_AppData.PipeDepth = GENERIC_RW_PIPE_DEPTH;

    strcpy(GENERIC_RW_AppData.PipeName, "GEN_RW_CMD_PIPE");

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_RW App: Error Registering Events, RC = 0x%08lX\n",
                             (unsigned long)status);
        return ( status );
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(CFE_MSG_PTR(GENERIC_RW_AppData.HkTlm.TlmHeader),
                   CFE_SB_ValueToMsgId(GENERIC_RW_APP_HK_TLM_MID),
                   sizeof(GENERIC_RW_AppData.HkTlm));

    /* Connect to the UART */
    status = uart_init_port(&RW_UART[0]);
    if(status != CFE_SUCCESS)
    {
    	CFE_ES_WriteToSysLog("GENERIC_RW App: UART 0 port initialization error!\n");
    }    
    status = uart_init_port(&RW_UART[1]);
    if(status != CFE_SUCCESS)
    {
    	CFE_ES_WriteToSysLog("GENERIC_RW App: UART 1 port initialization error!\n");
    }    
    status = uart_init_port(&RW_UART[2]);
    if(status != CFE_SUCCESS)
    {
    	CFE_ES_WriteToSysLog("GENERIC_RW App: UART 2 port initialization error!\n");
    }    
    

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&GENERIC_RW_AppData.CommandPipe,
                               GENERIC_RW_AppData.PipeDepth,
                               GENERIC_RW_AppData.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_RW App: Error creating pipe, RC = 0x%08lX\n",
                             (unsigned long)status);
        return ( status );
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_RW_APP_SEND_HK_MID),
                              GENERIC_RW_AppData.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GENERIC_RW App: Error Subscribing to HK request, RC = 0x%08lX\n",
                             (unsigned long)status);
        return ( status );
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(GENERIC_RW_APP_CMD_MID),
                              GENERIC_RW_AppData.CommandPipe);
    if (status != CFE_SUCCESS )
    {
        CFE_ES_WriteToSysLog("GENERIC_RW App: Error Subscribing to Command, RC = 0x%08lX\n",
                             (unsigned long)status);

        return ( status );
    }

    CFE_EVS_SendEvent (GENERIC_RW_STARTUP_INF_EID,
                       CFE_EVS_EventType_INFORMATION,
                       "GENERIC_RW App Initialized. Version %d.%d.%d.%d",
                       GENERIC_RW_APP_MAJOR_VERSION,
                       GENERIC_RW_APP_MINOR_VERSION,
                       GENERIC_RW_APP_REVISION,
                       GENERIC_RW_APP_MISSION_REV);

    return ( CFE_SUCCESS );

} /* End of GENERIC_RW_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  GENERIC_RW_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the GENERIC_RW    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void GENERIC_RW_ProcessCommandPacket( CFE_MSG_Message_t * Msg )
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(Msg, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case GENERIC_RW_APP_CMD_MID:
            GENERIC_RW_ProcessGroundCommand(Msg);
            break;

        case GENERIC_RW_APP_SEND_HK_MID:
            GENERIC_RW_ReportHousekeeping();
            break;

        default:
            CFE_EVS_SendEvent(GENERIC_RW_INVALID_MSGID_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "GENERIC_RW: invalid command packet,MID = 0x%x",
                              CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;

} /* End GENERIC_RW_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* GENERIC_RW_ProcessGroundCommand() -- GENERIC_RW ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void GENERIC_RW_ProcessGroundCommand( CFE_MSG_Message_t * Msg )
{
    CFE_MSG_FcnCode_t CommandCode = 0;
    CFE_MSG_GetFcnCode(Msg, &CommandCode);

    /*
    ** Process "known" GENERIC_RW app ground commands
    */
    switch (CommandCode)
    {
        case GENERIC_RW_APP_NOOP_CC:
            if (GENERIC_RW_VerifyCmdLength(Msg, sizeof(GENERIC_RW_Noop_t)))
            {
                GENERIC_RW_Noop((GENERIC_RW_Noop_t *)Msg);
            }

            break;

        case GENERIC_RW_APP_RESET_COUNTERS_CC:
            if (GENERIC_RW_VerifyCmdLength(Msg, sizeof(GENERIC_RW_ResetCounters_t)))
            {
                GENERIC_RW_ResetCounters((GENERIC_RW_ResetCounters_t *)Msg);
            }

            break;

		/* Request Generic Reaction Wheel data 		*/
		case GENERIC_RW_APP_REQ_DATA_CC:
            if (GENERIC_RW_VerifyCmdLength(Msg, sizeof(GENERIC_RW_ResetCounters_t)))
            {
                GENERIC_RW_Current_Momentum((GENERIC_RW_Noop_t *)Msg);
            }
            
			break;

        /* Set Generic Reaction Wheel torque */
        case GENERIC_RW_APP_SET_TORQUE_CC:
            if (GENERIC_RW_VerifyCmdLength(Msg, sizeof(GENERIC_RW_Cmd_t)))
            {
                GENERIC_RW_Set_Torque((GENERIC_RW_Cmd_t *)Msg );
            }

            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(GENERIC_RW_COMMAND_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d",
                              CommandCode);
            break;
    }

    return;

} /* End of GENERIC_RW_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  GENERIC_RW_ReportHousekeeping                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 GENERIC_RW_ReportHousekeeping(void)
{
    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &GENERIC_RW_AppData.HkTlm.TlmHeader);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &GENERIC_RW_AppData.HkTlm.TlmHeader, true);

    return CFE_SUCCESS;

} /* End of GENERIC_RW_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* GENERIC_RW_Noop -- GENERIC_RW NOOP commands                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 GENERIC_RW_Noop( const GENERIC_RW_Noop_t *Msg )
{

    GENERIC_RW_AppData.HkTlm.Payload.CommandCounter++;

    CFE_EVS_SendEvent(GENERIC_RW_COMMANDNOP_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "GENERIC_RW: NOOP command  Version %d.%d.%d.%d",
                      GENERIC_RW_APP_MAJOR_VERSION,
                      GENERIC_RW_APP_MINOR_VERSION,
                      GENERIC_RW_APP_REVISION,
                      GENERIC_RW_APP_MISSION_REV);

    return CFE_SUCCESS;

} /* End of GENERIC_RW_Noop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  GENERIC_RW_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 GENERIC_RW_ResetCounters( const GENERIC_RW_ResetCounters_t *Msg )
{

    GENERIC_RW_AppData.HkTlm.Payload.CommandCounter = 0;
    GENERIC_RW_AppData.HkTlm.Payload.CommandErrorCounter = 0;

    CFE_EVS_SendEvent(GENERIC_RW_COMMANDRST_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "GENERIC_RW: RESET command");

    return CFE_SUCCESS;

} /* End of GENERIC_RW_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* GENERIC_RW_Noop -- GENERIC_RW Current Momentum command                         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 GENERIC_RW_Current_Momentum( const GENERIC_RW_Noop_t *Msg )
{
    int32_t status;
    double momentum;

    GENERIC_RW_AppData.HkTlm.Payload.CommandCounter++;
    CFE_EVS_SendEvent(GENERIC_RW_CMD_REQ_DATA_EID, CFE_EVS_EventType_DEBUG,"Request Generic Reaction Wheel Data");

    /* Read data from the UARTs for all 3 wheels */
    for (int i = 0; i < 3; i++) {
        status = GetCurrentMomentum(i, &momentum);
        //OS_printf("GENERIC_RW: GetCurrentMomentum:  status=%d, momentum=%f\n", status, momentum);
        if (status > 0) {
            GENERIC_RW_AppData.HkTlm.Payload.data.momentum[i] = momentum;
        } else {
            CFE_EVS_SendEvent(GENERIC_RW_CMD_REQ_DATA_EID, CFE_EVS_EventType_ERROR,"Request Generic Reaction Wheel Data - Error reading momentum");
        }
    }

    /* publish the HK message which includes reaction wheel data */
    GENERIC_RW_ReportHousekeeping();

    return CFE_SUCCESS;
} /* End of GENERIC_RW_Current_Momentum */

/************************************************************************
** Get current momentum data from the UART
*************************************************************************/
static int32_t GetCurrentMomentum(int wheel_number, double *momentum)
{
    uint8_t DataBuffer[1024];
    int32 DataLen;
    char *reply;

    char *request = "CURRENT_MOMENTUM";
    int32_t status = uart_write_port(&RW_UART[wheel_number], (uint8_t*)request, strlen(request));
    if (status < 0) {
        CFE_EVS_SendEvent(GENERIC_RW_CMD_REQ_DATA_EID, CFE_EVS_EventType_ERROR, "GetCurrentMomentum: Error writing to UART=%d\n", status);
    }
    /* check how many bytes are waiting on the uart */
    DataLen = uart_bytes_available(&RW_UART[wheel_number]);
    if (DataLen > 0)
    {
        /* grab the bytes */
        status = uart_read_port(&RW_UART[wheel_number], DataBuffer, DataLen);
        if (status < 0) {
            CFE_EVS_SendEvent(GENERIC_RW_CMD_REQ_DATA_EID, CFE_EVS_EventType_ERROR, "GetCurrentMomentum: Error reading from UART=%d\n", status);
        } else {
            DataBuffer[DataLen] = 0; // Ensure null termination
            reply = (char *)DataBuffer;
            if (strncmp(reply, "CURRENT_MOMENTUM=", 17) == 0) {
                *momentum = atof(&reply[17]);
            } else {
                status = 0;
            }
        }
    }

    return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* GENERIC_RW_Noop -- GENERIC_RW Set Torque command                         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 GENERIC_RW_Set_Torque( const GENERIC_RW_Cmd_t *Msg )
{
    int32_t status;
    double torque;
    char request[22];

    GENERIC_RW_AppData.HkTlm.Payload.CommandCounter++;
    GENERIC_RW_Cmd_t *cmd;
    cmd = (GENERIC_RW_Cmd_t*)Msg;
    CFE_EVS_SendEvent(GENERIC_RW_CMD_SET_TORQUE_EID, CFE_EVS_EventType_DEBUG, 
                    "Generic Reaction Wheel %d: Info, Set Torque Command received (%d * 10^-4 N-m)", cmd->wheel_number, cmd->data);
    torque = cmd->data;
    torque /= 10000.0; // units are 10^-4 Newton-meters (so we don't have to send floats in the command)

    sprintf(request, "SET_TORQUE=%10.4f", torque);
    status = uart_write_port(&RW_UART[cmd->wheel_number], (uint8_t*)request, strlen(request));
    //OS_printf("Generic Reaction Wheel: Sending command:%s\n", request);
    if (status < 0) {
        CFE_EVS_SendEvent(GENERIC_RW_CMD_SET_TORQUE_EID, CFE_EVS_EventType_ERROR, "Generic Reaction Wheel: Error writing to UART=%d\n", status);
    } else {
        /* Read the reply */
        uint8_t DataBuffer[1024];
        int32 DataLen;
        /* check how many bytes are waiting on the uart */
        DataLen = uart_bytes_available(&RW_UART[cmd->wheel_number]);
        if (DataLen > 0)
        {
            uart_read_port(&RW_UART[cmd->wheel_number], DataBuffer, DataLen);
            DataBuffer[DataLen] = 0; // Ensure null termination
            //OS_printf("Generic Reaction Wheel: Response on UART=%s\n", (char *)DataBuffer);
        }
    }

    return CFE_SUCCESS;
} /* End of GENERIC_RW_Set_Torque */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* GENERIC_RW_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool GENERIC_RW_VerifyCmdLength( CFE_MSG_Message_t * Msg, uint16 ExpectedLength )
{
    bool result = true;
    CFE_SB_MsgId_t MessageID = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;
    size_t ActualLength = 0;

    CFE_MSG_GetSize(Msg, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(Msg, &MessageID);
        CFE_MSG_GetFcnCode(Msg, &CommandCode);

        CFE_EVS_SendEvent(GENERIC_RW_LEN_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
                          CFE_SB_MsgIdToValue(MessageID),
                          CommandCode,
                          ActualLength,
                          ExpectedLength);

        result = false;

        GENERIC_RW_AppData.HkTlm.Payload.CommandErrorCounter++;
    }
    return( result );
} /* End of GENERIC_RW_VerifyCmdLength() */
