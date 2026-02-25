/*******************************************************************************
** File: ids_app.c
**
** Purpose:
**   This file contains the source code for the IDS application.
**
*******************************************************************************/

/*
** Include Files
*/
#include <arpa/inet.h>
#include "ids_app.h"
#include "ids_p1.h"

#ifdef IDS_TELEM_ENABLED
#include "ids_platform_cfg.h"
#include <errno.h>
#endif

/*
** Global Data
*/
#if (!defined(IDS_P1_ENABLED) && !defined(IDS_P2_ENABLED) && !defined(IDS_P3_ENABLED)) || !defined(IDS_TELEM_ENABLED)
IDS_AppData_t IDS_AppData;
#endif
FILE *logs = NULL;
int batch=0;


//extern struct P1_struct P1_data;
extern struct P1_struct * P1_struct_ptr; // used for the bad crc count in HK packet
#ifdef IDS_TELEM_ENABLED
extern struct DetectionReporting DetectionAlert;
#endif

/*
** Application entry point and main process loop
*/
void IDS_AppMain(void)
{

	/*FILE *fichier = NULL;
	fichier = fopen("/tmp/debugids.txt", "w");


	logs = fopen("/tmp/logsids.txt", "w");*/

    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(IDS_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = IDS_AppInit();
    if (status != CFE_SUCCESS)
    {
        IDS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&IDS_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(IDS_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        //struct timeval begin_pend;
        //struct timeval end_pend;
        //measure time we spent waiting pending on arrival of a bus message
        //gettimeofday(&begin_pend, 0);

        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&IDS_AppData.MsgPtr,  IDS_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        //gettimeofday(&begin_pend, 0);
        //gettimeofday(&end_pend, 0);

        //float diff = (end_pend.tv_sec - begin_pend.tv_sec)*1000.0f + (end_pend.tv_usec - begin_pend.tv_usec)/ 1000.0f;
        //printf("time to be called : %f",diff);
        //fprintf(fichier, "%f\n",diff);
        //fflush(fichier); - flush every batch w/ if ?
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(IDS_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            IDS_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(IDS_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "IDS: SB Pipe Read Error = %d", (int) status);
            IDS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
        //gettimeofday(&end_pend, 0);
        //float diff = (end_pend.tv_sec - begin_pend.tv_sec)*1000.0f + (end_pend.tv_usec - begin_pend.tv_usec)/ 1000.0f;
        //fprintf(fichier, "%f\n",diff);
    }

    /*fclose(fichier);
    fclose(logs);*/

    /*
    ** Disable component, which cleans up the interface, upon exit
    */
    IDS_Disable();

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(IDS_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(IDS_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 IDS_AppInit(void)
{

    int32 status = OS_SUCCESS;
    
    IDS_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);    /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("IDS: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&IDS_AppData.CmdPipe, IDS_PIPE_DEPTH, "IDS_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(IDS_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(IDS_CMD_MID), IDS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(IDS_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            IDS_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(IDS_REQ_HK_MID), IDS_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(IDS_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            IDS_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /* TODO:
	** Subscribe to other (many...) messages from other components (make a big table with all IDs and iterate on it ??)
	**  + subscribe to things that may not come from the ground ? */
    /*for (int i=0;i<IDS_TABLE_SIZE;i++)
    {
    	status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(tab[i]), IDS_AppData.CmdPipe);
    	if (status != CFE_SUCCESS)
    	{
    		CFE_EVS_SendEvent(IDS_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
    				"Error Subscribing to ID Request, MID=0x%04X, RC=0x%08X",
					IDS_REQ_HK_MID, (unsigned int) status);
    		return status;
    	}
    }*/

    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the IDS_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(IDS_AppData.HkTelemetryPkt.TlmHeader),
                   CFE_SB_ValueToMsgId(IDS_HK_TLM_MID),
                   IDS_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(IDS_AppData.DevicePkt.TlmHeader),
                   CFE_SB_ValueToMsgId(IDS_DEVICE_TLM_MID),
                   IDS_DEVICE_TLM_LNGTH);

    /*
    ** TODO: Initialize any other messages that this app will publish
    */


    /* 
    ** Always reset all counters during application initialization 
    */
    IDS_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    IDS_AppData.HkTelemetryPkt.DeviceEnabled = IDS_DEVICE_DISABLED;
    IDS_AppData.HkTelemetryPkt.DeviceHK.DeviceCounter = 0;
    IDS_AppData.HkTelemetryPkt.DeviceHK.DeviceConfig = 0;
    IDS_AppData.HkTelemetryPkt.DeviceHK.DeviceStatus = 0;
    IDS_AppData.HkTelemetryPkt.DeviceHK.IdsStatus = 0;
    IDS_AppData.HkTelemetryPkt.DeviceHK.PacketsDetected = 0;
    IDS_AppData.HkTelemetryPkt.DeviceHK.BadCrc = 0;
    IDS_AppData.DevicePkt.Ids.Module = 0;
    IDS_AppData.DevicePkt.Ids.RuleNb = 0;

    #ifdef IDS_TELEM_ENABLED
    /* Create child task (high priority alert handler) */
    status = CFE_ES_CreateChildTask(&IDS_AppData.ChildTaskID,
                                    IDS_CHILD_TASK_NAME,
                                    IDS_ChildTask, 0,
                                    IDS_CHILD_TASK_STACK_SIZE,
                                    IDS_CHILD_TASK_PRIORITY, 0);

    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(IDS_CHILD_INIT_ERR_EID, CFE_EVS_EventType_ERROR,
           "IDS child task initialization error: create task failed: status = %d", status);
    }
    #endif

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(IDS_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "IDS App Initialized. Version %d.%d.%d.%d",
                IDS_MAJOR_VERSION,
                IDS_MINOR_VERSION, 
                IDS_REVISION, 
                IDS_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("IDS: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 


/* 
** Process packets received on the IDS command pipe
*/
void IDS_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(IDS_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the IDS_CMD_MID (Message ID)
        */
        case IDS_CMD_MID:
            IDS_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case IDS_REQ_HK_MID:
            IDS_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            IDS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(IDS_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "IDS: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
} 


/*
** Process ground commands
** TODO: Add additional commands required by the specific component
*/
void IDS_ProcessGroundCommand(void)
{  
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(IDS_AppData.MsgPtr, &MsgId);

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(IDS_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case IDS_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (IDS_VerifyCmdLength(IDS_AppData.MsgPtr, sizeof(IDS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(IDS_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "IDS: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case IDS_RESET_COUNTERS_CC:
            if (IDS_VerifyCmdLength(IDS_AppData.MsgPtr, sizeof(IDS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(IDS_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "IDS: RESET counters command received");
                IDS_ResetCounters();
            }
            break;

        /*
        ** Enable Command
        */
        case IDS_ENABLE_CC:
            if (IDS_VerifyCmdLength(IDS_AppData.MsgPtr, sizeof(IDS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(IDS_CMD_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "IDS: Enable command received");
                IDS_Enable();
            }
            break;

        /*
        ** Disable Command
        */
        case IDS_DISABLE_CC:
            if (IDS_VerifyCmdLength(IDS_AppData.MsgPtr, sizeof(IDS_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(IDS_CMD_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "IDS: Disable command received");
                IDS_Disable();
            }
            break;

        /*
        ** TODO: Edit and add more command codes as appropriate for the application
        ** Set Configuration Command
        ** Note that this is an example of a command that has additional arguments
        */
        case IDS_CONFIG_CC:
            if (IDS_VerifyCmdLength(IDS_AppData.MsgPtr, sizeof(IDS_Config_cmd_t)) == OS_SUCCESS)
            {
                uint32_t config = ntohl(((IDS_Config_cmd_t*) IDS_AppData.MsgPtr)->DeviceCfg); // command is defined as big-endian... need to convert to host representation
                CFE_EVS_SendEvent(IDS_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "IDS: Configuration command received: %u", config);
                /* Command device to send HK */
                status = IDS_CommandDevice(&IDS_AppData.IdsUart, IDS_DEVICE_CFG_CMD, config);
                if (status == OS_SUCCESS)
                {
                    IDS_AppData.HkTelemetryPkt.DeviceCount++;
                }
                else
                {
                    IDS_AppData.HkTelemetryPkt.DeviceErrorCount++;
                }
            }
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            IDS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(IDS_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "IDS: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 


/*
** Process Telemetry Request - Triggered in response to a telemetery request
** TODO: Add additional telemetry required by the specific component
*/
void IDS_ProcessTelemetryRequest(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(IDS_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(IDS_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case IDS_REQ_HK_TLM:
	    #ifdef IDS_TELEM_ENABLED
            IDS_ReportHousekeeping();
	    #endif
            break;

        case IDS_REQ_DATA_TLM:
	    #if ((defined IDS_TELEM_ENABLED)&&(defined IDS_TELEM_DEBUG))
            IDS_ReportDeviceTelemetry(0, 0, NULL, 0, NULL);
	    #endif
	    break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            IDS_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(IDS_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "IDS: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}

/* 
** Report Application Housekeeping
*/
void IDS_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (IDS_AppData.HkTelemetryPkt.DeviceEnabled == IDS_DEVICE_ENABLED)
    {
        status = IDS_RequestHK(&IDS_AppData.IdsUart, (IDS_Device_HK_tlm_t*) &IDS_AppData.HkTelemetryPkt.DeviceHK);
        if (status == OS_SUCCESS)
        {
            IDS_AppData.HkTelemetryPkt.DeviceCount++;
	    //TODO - ADD UPDATE OF IDS HK PACKET - DATA GATHERING AND UPDATE INTO HK STRUCT
	    //TODO - MUTEX ?
	    #ifdef IDS_P1_ENABLED
        //TODO - move to ReportDeviceTelemetry
	    IDS_AppData.HkTelemetryPkt.DeviceHK.BadCrc = P1_struct_ptr->bad_crc_count;
	    //IDS_AppData.HkTelemetryPkt.DeviceHK.BadCrc = get_bad_crc_count();
	    #endif
	    //printf("bad crc count : %d\n", P1_struct_ptr->bad_crc_count);
        }
        else
        {
            IDS_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(IDS_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "IDS: Request device HK reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &IDS_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &IDS_AppData.HkTelemetryPkt, true);
    return;
}


/*
** Collect and Report Device Telemetry
*/
void IDS_ReportDeviceTelemetry(uint8_t module, uint8_t rule, /*uint8_t * packet, uint16_t data_len,*/ char * TaskName)
{
    #if ((defined IDS_TELEM_ENABLED)&&(defined IDS_P1_ENABLED || defined IDS_P2_ENABLED || defined IDS_P3_ENABLED))
    int32 status = OS_SUCCESS;
    //printf("DeviceEnabled = %d vs %d\n", IDS_AppData.HkTelemetryPkt.DeviceEnabled, IDS_DEVICE_ENABLED);
    
    /* Check that device is enabled */
    if (IDS_AppData.HkTelemetryPkt.DeviceEnabled == IDS_DEVICE_ENABLED)
    {
        status = IDS_RequestData(&IDS_AppData.IdsUart, (IDS_Device_Data_tlm_t*) &IDS_AppData.DevicePkt.Ids);
        if (status == OS_SUCCESS)
        {
        IDS_AppData.HkTelemetryPkt.DeviceCount++;
        
        sem_wait(&(DetectionAlert.sem_detect_count));
        IDS_AppData.HkTelemetryPkt.DeviceHK.PacketsDetected++;
        //TODO - FILL VALUE HERE OF NUMBER OF BAD CRC + SUCCESSIVE BAD CRC
        sem_post(&(DetectionAlert.sem_detect_count));
	    
	    //TODO - ADD UPDATE OF IDS DATA PACKET - DATA GATHERING AND UPDATE INTO DATA STRUCT
	    IDS_AppData.DevicePkt.Ids.Module = module;
	    IDS_AppData.DevicePkt.Ids.RuleNb = rule;
	    
	    /*if (data_len > 0)
	    {
            //TODO - replace by memcpy ?
	    	for (unsigned int i=0;i<sizeof(IDS_AppData.DevicePkt.Ids.PacketData);i++) //TODO - replace sizeof with a constant (#define ?) that normalizes the size of the 3 packet structures passed
			    IDS_AppData.DevicePkt.Ids.PacketData[i] = packet[i];
        }
	    else
		    memset(IDS_AppData.DevicePkt.Ids.PacketData, 0, IDS_PDATA_SIZE);*/
	    
        /*printf("packet_tlm : ");
        for(unsigned int i = 0; i<sizeof(IDS_AppData.DevicePkt.Ids.PacketData);i++)
            printf("%02x",IDS_AppData.DevicePkt.Ids.PacketData[i]);
        printf("\n");*/

	    //IDS_AppData.DevicePkt.Ids.DataLen = data_len;
	    
	    if (TaskName != NULL)
		    strncpy(IDS_AppData.DevicePkt.Ids.Taskname,TaskName,IDS_TSKNAME_SIZE);
	    else
		    memset(IDS_AppData.DevicePkt.Ids.Taskname, 0, IDS_TSKNAME_SIZE);

            /* Time stamp and publish data telemetry */
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &IDS_AppData.DevicePkt);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &IDS_AppData.DevicePkt, true);
	    //printf("published\n");
        }
        else
        {
            IDS_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(IDS_REQ_DATA_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "IDS: Request device data reported error %d", status);
        }
    }
    #endif
    /* Intentionally do not report errors if disabled */
    return;
}

#ifdef IDS_TELEM_ENABLED
void IDS_RaiseAlert(uint8_t module, uint8_t rule, uint16_t data_len, uint8_t TName_len, uint8_t *packet, char *Taskname)
{
    //TODO - FOR NOW DATA_LEN AND PACKET ARE UNUSED, BUT WOULD NEED TO WRITE THEM TO A FILE AND BE ABLE TO GET LOG FILES FROM IDS USING A TC SENT FROM GROUND

	#if defined IDS_P1_ENABLED || defined IDS_P2_ENABLED || defined IDS_P3_ENABLED
    //take semaphore for alert reporting - ensures 1 alert reporting at a time
	sem_wait(&(DetectionAlert.is_reporting));

	union DetectionData d;
	d.AlertData.module = module;
	d.AlertData.rule = rule;
	//d.AlertData.data_len = data_len;
	d.AlertData.TName_len = TName_len;

	/*if (data_len > 0)
	{
        //TODO - CODE OPTIMIZATION : is this mandatory ? maybe we could write something with a variable length depending 
        //on packet size (but need to read element by element of the struct instead of whole buffer) ?
        memset(d.AlertData.packet, 0, sizeof(d.AlertData.packet));

        //TRUNCATE HERE : if a packet is too large (>175B), it is truncated to 175B. The size of packet "holder" 
        //in telemetry could be enlarged but one could still imagine a packet with an even bigger size
        //either crafted by an attacker or just with a different implementation of cryptolib/cfdp limitations. 
        //TODO - Maybe change later to match that couple's limits even though it would mean being vulnerable to implementation changes ?
        //in the later sent alert packet, the data_len field still equals the original length allowing to know on ground that the packet was truncated
        if (data_len > sizeof(d.AlertData.packet))
            data_len = sizeof(d.AlertData.packet);


		for (int i=0;i<data_len;i++) 
			d.AlertData.packet[i] = packet[i];
    }*/

	if (Taskname != NULL)
		strncpy(d.AlertData.Taskname,Taskname,IDS_TSKNAME_SIZE);
	else
		strncpy(d.AlertData.Taskname,"None",IDS_TSKNAME_SIZE);

    /*printf("data_to_pipe : ");
    for (unsigned int i=0; i<sizeof(d.AlertData.packet); i++)
        printf("%02x",d.AlertData.packet[i]);
    printf("\n");*/

	write(DetectionAlert.pipe_alert[1], d.buf, sizeof(d.buf));
	#endif
}

void IDS_ChildTask(void)
{
	#if defined IDS_P1_ENABLED || defined IDS_P2_ENABLED || defined IDS_P3_ENABLED
	while(true)
	{
		//poll to see if an alert packet needs to be sent
		union DetectionData d;
		int bytesRead = read(DetectionAlert.pipe_alert[0], d.buf, sizeof(struct AlertStruct));
		if (bytesRead == -1) //read error
			if (errno == EAGAIN) //no data available, retry later
				usleep(200000);
			else
			{
				printf("read from pipe failed with errno : %d\n", errno);
				break;
			}
		else
		{
			/*printf("bytesRead = %d, msg = %s\n", bytesRead, d.buf);
			printf("bytesRead = %d, module = %d, rule = %d, data_len = %d, TName_len = %d\n", bytesRead, d.AlertData.module, d.AlertData.rule, d.AlertData.data_len, d.AlertData.TName_len);
			printf("packet : ");
			for (int i = 0; i<(int)sizeof(d.AlertData.packet); i++)
				printf("%02X",d.AlertData.packet[i]);
            printf("\n");
			if (d.AlertData.Taskname != NULL)
				printf("Task name : %s\n",d.AlertData.Taskname);*/
            
            //send alert telemetry packet
			IDS_ReportDeviceTelemetry(d.AlertData.module, d.AlertData.rule, /*d.AlertData.packet, d.AlertData.data_len,*/ d.AlertData.Taskname);
			
            //give back semaphore
			sem_post(&(DetectionAlert.is_reporting));
		}
	}
	#else
	printf("Reporting is disabled : no detection activated\n");
	#endif
    printf("IDS child task exit complete\n");
    CFE_ES_ExitChildTask();
}
#endif

/*
** Reset all global counter variables
*/
void IDS_ResetCounters(void)
{
    IDS_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    IDS_AppData.HkTelemetryPkt.CommandCount = 0;
    IDS_AppData.HkTelemetryPkt.DeviceErrorCount = 0;
    IDS_AppData.HkTelemetryPkt.DeviceCount = 0;
    return;
} 


/*
** Enable Component
** TODO: Edit for your specific component implementation
*/
void IDS_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is disabled */
    if (IDS_AppData.HkTelemetryPkt.DeviceEnabled == IDS_DEVICE_DISABLED)
    {
        /*
        ** Initialize hardware interface data
        ** TODO: Make specific to your application depending on protocol in use
        ** Note that other components provide examples for the different protocols available
        */ 
        IDS_AppData.IdsUart.deviceString = IDS_CFG_STRING;
        IDS_AppData.IdsUart.handle = IDS_CFG_HANDLE;
        IDS_AppData.IdsUart.isOpen = PORT_CLOSED;
        IDS_AppData.IdsUart.baud = IDS_CFG_BAUDRATE_HZ;
        IDS_AppData.IdsUart.access_option = uart_access_flag_RDWR;

        /* Open device specific protocols */
        status = uart_init_port(&IDS_AppData.IdsUart);
        if (status == OS_SUCCESS)
        {
            IDS_AppData.HkTelemetryPkt.DeviceCount++;
            IDS_AppData.HkTelemetryPkt.DeviceEnabled = IDS_DEVICE_ENABLED;
            CFE_EVS_SendEvent(IDS_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "IDS: Device enabled");
        }
        else
        {
            IDS_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(IDS_UART_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "IDS: UART port initialization error %d", status);
        }
    }
    else
    {
        IDS_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(IDS_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, "IDS: Device enable failed, already enabled");
    }
    return;
}


/*
** Disable Component
** TODO: Edit for your specific component implementation
*/
void IDS_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (IDS_AppData.HkTelemetryPkt.DeviceEnabled == IDS_DEVICE_ENABLED)
    {
        /* Open device specific protocols */
        status = uart_close_port(&IDS_AppData.IdsUart);
        if (status == OS_SUCCESS)
        {
            IDS_AppData.HkTelemetryPkt.DeviceCount++;
            IDS_AppData.HkTelemetryPkt.DeviceEnabled = IDS_DEVICE_DISABLED;
            CFE_EVS_SendEvent(IDS_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "IDS: Device disabled");
        }
        else
        {
            IDS_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(IDS_UART_CLOSE_ERR_EID, CFE_EVS_EventType_ERROR, "IDS: UART port close error %d", status);
        }
    }
    else
    {
        IDS_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(IDS_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR, "IDS: Device disable failed, already disabled");
    }
    return;
}


/*
** Verify command packet length matches expected
*/
int32 IDS_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;

    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        IDS_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(IDS_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        IDS_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
} 
