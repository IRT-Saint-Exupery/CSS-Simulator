/*******************************************************************************
** File: isl_app.c
**
** Purpose:
**   This file contains the source code for the ISL application.
**
*******************************************************************************/

/*
** Include Files
*/
#include <arpa/inet.h>
#include "isl_app.h"
#include <time.h>
#include <ifaddrs.h>
//LOUIS
#include "ids_p0.h"
#include "ids_p1.h"
#include "novatel_oem615_msgids.h" // IRT CSS
#include "ids_app.h"

/*
** Global Data
*/
ISL_AppData_t ISL_AppData;

/*
** Local Data
*/
//IRT CSS - IDS
#ifdef LOGGING_P0_ENABLED
static FILE *log_fp_0;
#endif
#if defined LOGGING_P1_ENABLED || defined IDS_LATENCY_BENCHMARK
static FILE *log_fp_1;
#endif
#ifdef IDS_P1_ENABLED
static FILE *log_attack;
unsigned int already_flooding = 0;
    #ifdef IDS_TELEM_ENABLED
    extern struct DetectionReporting DetectionAlert;
    #endif
#endif
#if defined LOGGING_P0_ENABLED || defined LOGGING_P1_ENABLED
extern unsigned long sim_start_time;
#endif
//static int counter = 0;
static double Sat_PosPrev[3] = {0,0,0};


int get_NSat_value_from_script(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open script file");
        return -1; // error code
    }

    char line[256];
    int value = -1; // default if not found

    while (fgets(line, sizeof(line), file)) {
        // Look for a line that starts with "AA="
        if (strncmp(line, "NBSAT=", 6) == 0) {
            // Extract the numeric value after "AA="
            char *ptr = line + 6; // move past "AA="
            value = atoi(ptr);
            break;
        }
    }

    fclose(file);
    return value;
}

unsigned int NB_SATELLITES;

/*
** Application entry point and main process loop
*/
void ISL_AppMain(void)
{
    int32 status = OS_SUCCESS;
    NB_SATELLITES = get_NSat_value_from_script("/home/nos3/Install_tools_nos3/IP_Scenario.sh");
    
    parseGroundStations("/home/nos3/Desktop/github-nos3/sims/cfg/InOut/Inp_Sim.txt"); //fill in stations and nb_stations
      
    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(ISL_PERF_ID);

    /* 
    ** Perform application initialization
    */
    status = ISL_AppInit();
    if (status != CFE_SUCCESS)
    {
        ISL_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }
    
    //LOUIS - Open log files
    #ifdef LOGGING_P0_ENABLED
    log_fp_0 = fopen("/tmp/logids_p0.csv", "w");
    fprintf(log_fp_0,"simtime,realtime,size,AttackTag,IDS_output,packet,realtime_string\n");
    #endif
    //#ifdef LOGGING_P1_ENABLED
    #if defined LOGGING_P1_ENABLED || defined IDS_LATENCY_BENCHMARK
    log_fp_1 = fopen("/tmp/logids_p1.csv", "w");
    fprintf(log_fp_1,"simtime,realtime,size,AttackTag,IDS_output,packet,realtime_string\n");
    #endif
    #ifdef IDS_P1_ENABLED
    log_attack = fopen("/tmp/logattack_p1.txt", "w");
    #endif

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&ISL_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(ISL_PERF_ID);

        /* 
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&ISL_AppData.MsgPtr,  ISL_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(ISL_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            ISL_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(ISL_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: SB Pipe Read Error = %d", (int) status);
            ISL_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Disable component, which cleans up the interface, upon exit
    */
    ISL_Disable();

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(ISL_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(ISL_AppData.RunStatus);
} 


/* 
** Initialize application
*/
int32 ISL_AppInit(void)
{
    int32 status = OS_SUCCESS;
    
    ISL_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;
    ISL_AppData.prec_visible = -1; // force the first visi to be transmitted to ground

    //ISL_AppData.ISL_TmDestinationSet = false;
    
    for(unsigned int i = 0; i < NB_SATELLITES + 1; i++) {  // init watchdog a zero 
    	ISL_AppData.LastCom[i] = 0;
    	ISL_AppData.upMessageSent[i] = false;
    }
    /*
    ** Init the simulation(or app) start time
    */
    //LOUIS
    #ifdef IDS_P1_ENABLED 
    init_probe1(); // MOVE TO IDS APP
    #endif
       
    /*
    ** Register the events
    */ 
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);    /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("ISL: Error registering for event services: 0x%08X\n", (unsigned int) status);
       return status;
    }

    /*
    ** Create the Software Bus command pipe 
    */
    status = CFE_SB_CreatePipe(&ISL_AppData.CmdPipe, ISL_PIPE_DEPTH, "ISL_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(ISL_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }
    
    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ISL_CMD_MID), ISL_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(ISL_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            ISL_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ISL_REQ_HK_MID), ISL_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(ISL_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            ISL_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /*
    ** TODO: Subscribe to any other messages here
    */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ISL_REQ_WD_MID), ISL_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(ISL_SUB_REQ_WD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to Watchdog Request, MID=0x%04X, RC=0x%08X",
            ISL_REQ_WD_MID, (unsigned int) status);
        return status;
    }
    
    
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(NOVATEL_OEM615_DEVICE_TLM_MID), ISL_AppData.CmdPipe); // IRT CSS
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Generic_ISL App: Error Subscribing to NOVATEL_OEM615_DEVICE_TLM_MID, RC = 0x%08lX\n", (unsigned long)status);
        return (status);
    }


    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the ISL_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(ISL_AppData.HkTelemetryPkt.TlmHeader),
                   CFE_SB_ValueToMsgId(ISL_HK_TLM_MID),
                   ISL_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(ISL_AppData.DevicePkt.TlmHeader),
                   CFE_SB_ValueToMsgId(ISL_DEVICE_TLM_MID),
                   ISL_DEVICE_TLM_LNGTH);

    /*
    ** TODO: Initialize any other messages that this app will publish
    */


    /* 
    ** Always reset all counters during application initialization 
    */
    ISL_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    ISL_AppData.HkTelemetryPkt.DeviceEnabled = ISL_DEVICE_DISABLED;
    ISL_AppData.HkTelemetryPkt.DeviceHK.DeviceCounter = 0;
    ISL_AppData.HkTelemetryPkt.DeviceHK.DeviceConfig = 0;
    ISL_AppData.HkTelemetryPkt.DeviceHK.DeviceStatus = 0;
    ISL_AppData.HkTelemetryPkt.DeviceHK.DropPktCounter = 0;

    /*
    ** Initialize sockets
    */

    /* store all the local ip once for all for comparison on receiving TM */

    status = cacheLocalIps();

	if (status < 0)
	{
		 CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot cache local IPs (err %d)", status);
		 return status;
	}

    /* Local reveive sockets */
    if (
		(status = udpInit (&ISL_AppData.TM_Rcv, ISL_CFG_TM_RCV_PORT)) < 0 ||
		(status = udpInit (&ISL_AppData.TC_Rcv, ISL_CFG_TC_RCV_PORT)) < 0
		)
    {
        CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Rcv sockets initialisation error %d", status);
        return status;
    }

    /* local send socket */

	status = udpInit (&ISL_AppData.Udp_TCSnd, ISL_CFG_UDP_SND_PORT );
	if (status < 0)
    {
        CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Snd socket initialisation error on TC %d", status);
        return status;
    }

	status = udpInit (&ISL_AppData.Udp_TMSnd, ISL_CFG_UDP_SND_PORT );
	if (status < 0)
    {
        CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Snd socket initialisation error on TM %d", status);
        return status;
    }

    /* Initialize select */
    status = IO_TransSelectClear(&ISL_AppData.rcv_selector);
    if (status < 0)
    {
        CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Error clearing the receive selector structure %d", status);
        return status;
    }

    /* Add rcv sockets to receive selector */
    if (
    	(status = IO_TransSelectAddFd(&ISL_AppData.rcv_selector, ISL_AppData.TC_Rcv.sockId )) < 0 ||
    	(status = IO_TransSelectAddFd(&ISL_AppData.rcv_selector, ISL_AppData.TM_Rcv.sockId )) < 0
    	)
     {
        CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Error adding Rcv_Sockets to selector %d", status);
        return status;
    }

    /* Configure the route table */
    status = ISL_RouteConfigInit (CFE_PSP_GetSpacecraftId(), ISL_routeConfigFile);

    if (status < 0)
    {
        CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Error configuring route table %d", status);
        return status;
    }

    /*
    ** Start device task
    */
    status = CFE_ES_CreateChildTask(&ISL_AppData.DeviceID,
                                    ISL_DEVICE_NAME,
                                    ISL_ISL_Task, 0,
                                    ISL_DEVICE_STACK_SIZE,
                                    ISL_DEVICE_PRIORITY, 0);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(ISL_TASK_CREATE_EID, CFE_EVS_EventType_ERROR, "ISL: Error ISL child task %d", status);
        return status;
    }



    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(ISL_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "ISL App Initialized. Version %d.%d.%d.%d",
                ISL_MAJOR_VERSION,
                ISL_MINOR_VERSION, 
                ISL_REVISION, 
                ISL_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("ISL: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }
    return status;
} 

int32 cacheLocalIps (void)
{
	int32 status = OS_SUCCESS;

	struct ifaddrs *ifaddr, *ifa;
	void *addr;

	ISL_AppData.ipv4Count = 0;

	status = getifaddrs(&ifaddr);
	if (status == -1) {
		return status;
	}

	for (ifa = ifaddr; ifa != NULL && ISL_AppData.ipv4Count < MAX_LOCAL_IP; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET) {
			continue;
		}

		addr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
		ISL_AppData.ipv4[ISL_AppData.ipv4Count++] = *(struct in_addr *)addr;
	}

	freeifaddrs (ifaddr);

	return status;
}

int isLocalIp (struct in_addr *ip)
{
	for (size_t i = 0; i < ISL_AppData.ipv4Count; i++) {
		if (ISL_AppData.ipv4[i].s_addr == ip->s_addr) {
			return 1;
		}
	}
	return 0;
}

int32 ISL_RouteConfigInit (uint16 scid ,const char* routeConfigFile)
{
  int32 status = 0;
  char line[512];
  char stringOfScids[512];   // eg. 2 2 3 6 ...
  int nodeId;
  int n;
  char path[1024];
  
  sprintf(path,"%s/%s",ISL_rootDataDir,routeConfigFile);

  FILE *fp = fopen (path, "r");

  if (fp == NULL)
  {
    CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot read routing configuration file %s", path);
     return -1;
  }

  while (fgets(line, sizeof(line),fp))
  {
    if (sscanf (line, "Node ID %d %n", &nodeId, &n) != 1)
      {
	continue;
      }

    if (nodeId == scid)
      {
	break;
      }
  }
  fclose (fp);
  
  if (nodeId == scid)
    {
      status =  readNextIdsFromStringOfIds (line + n);
    }

#ifdef DEBUG
  if (status >= 0)
  {
    CFE_EVS_SendEvent(ISL_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "Route configuration Init for Sat %d : ", scid);
    //printf ("Route configuration Init for Sat %d : ", scid);
    for (unsigned int i = 0; i <= NB_SATELLITES; i++)
      {
	printf ("%d ", ISL_AppData.routeTo[i]);
      }
	  printf ("\n");
  }
#endif
  
  return status;
}

int32 readNextIdsFromStringOfIds (const char * stringOfScids)
{
  const char *ptr = stringOfScids;
  int n;
  unsigned int index = 0;
  uint16 nextNodeId = 0;
  
  for (unsigned int i = 0; i <= NB_SATELLITES; i++)
    {
	ISL_AppData.routeTo[i] = ISL_NO_ROUTE;
    }

  
  while (sscanf (ptr, "%hu %n", &nextNodeId, &n) == 1)
    {
    if (nextNodeId > NB_SATELLITES) {
    	return -1;
    }
    		
    ISL_AppData.routeTo[index] = nextNodeId;
    
    if (index == NB_SATELLITES)      // Indexes in [0 (ground) , NB_SATELLITES]
	{
	  return 0;
	}
      ptr +=  n;
      index++;
    }

  return -1;
}


int32 udpInit (IO_TransUdp_t *udp, uint16 usPort)
{
	int32 iStatus = -1;
	IO_TransUdpConfig_t configSocket;

    /* Initialize the UDP socket */
    CFE_PSP_MemSet((void *) &configSocket, 0x0, sizeof(IO_TransUdpConfig_t));
    strncpy(configSocket.cAddr, IO_TRANS_UDP_INADDR_ANY, 16);
    configSocket.usPort = usPort;
    configSocket.timeoutRcv = 0;

    iStatus = IO_TransUdpInit(&configSocket, udp);

    return iStatus;
}


/* 
** Process packets received on the ISL command pipe
*/
void ISL_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(ISL_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the ISL_CMD_MID (Message ID)
        */
        case ISL_CMD_MID:
            ISL_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case ISL_REQ_HK_MID:          
	    ISL_ProcessTelemetryRequest();                  
            break;
            
        case ISL_REQ_WD_MID:
    	    ISL_ManageWatchdog();          
            break;
            
        case NOVATEL_OEM615_DEVICE_TLM_MID:
            Generic_ISL_ingest_generic_Gps(ISL_AppData.MsgPtr, &ISL_AppData.Gps); 
            
            // This is to pass the visibility information from 42 to ground station (this is a temporary work around as FrontEnd has no direct access to sat position).          
            if (REAL_VISIBILITY) {
            
            	// Forecasted visibility workaround // this is a workaround to pass info from 42 to frontend.
            	double Sat_PosFor[3] = {0,0,0};
            	satellite_propagation_ecef(Sat_PosPrev[0],Sat_PosPrev[1],Sat_PosPrev[2],ISL_AppData.Gps.ECEFX,ISL_AppData.Gps.ECEFY,ISL_AppData.Gps.ECEFZ,&Sat_PosFor[0],&Sat_PosFor[1],&Sat_PosFor[2]);              
            	Sat_PosPrev[0] = ISL_AppData.Gps.ECEFX;
            	Sat_PosPrev[1] = ISL_AppData.Gps.ECEFY;
            	Sat_PosPrev[2] = ISL_AppData.Gps.ECEFZ;                
                  
                // current visibility for a given ground station        
            	// ISL_AppData.visible = is_satellite_visible(GS_LAT_DEG, GS_LON_DEG, MIN_ELEV_DEG, ISL_AppData.Gps.ECEFX, ISL_AppData.Gps.ECEFY, ISL_AppData.Gps.ECEFZ); // for a specific MCS
            	// for all MCS defined in Inp_Sim.txt
            	// stab visible means that sat is visible and it will be visible for some time in the future! for all MCS defined in Inp_Sim.txt
            	ISL_AppData.stab_visible = is_satellite_visible_allMCS(MIN_ELEV_DEG, ISL_AppData.Gps.ECEFX, ISL_AppData.Gps.ECEFY, ISL_AppData.Gps.ECEFZ,Sat_PosFor[0],Sat_PosFor[1],Sat_PosFor[2]);
            	ISL_AppData.visible = is_satellite_visible_now_allMCS(MIN_ELEV_DEG, ISL_AppData.Gps.ECEFX, ISL_AppData.Gps.ECEFY, ISL_AppData.Gps.ECEFZ);
                     	
            	uint16 localSatId = (uint16)CFE_PSP_GetSpacecraftId();
            	            	            	           	
            	{       // this is a workaround to send pos sat to MCS 
            		static int counterPos = 0;
            		if (!(counterPos++%5)) {           			          		        
		    		char filename[32];
		    		//uint16 localSatId = (uint16)CFE_PSP_GetSpacecraftId();
			    	sprintf(filename, "/tmp/pos%d.txt",localSatId);
			    			    	
			    	FILE* fp = fopen(filename,"w");
			   	fprintf(fp, "%.0f %.0f %.0f\n", ISL_AppData.Gps.ECEFX, ISL_AppData.Gps.ECEFY, ISL_AppData.Gps.ECEFZ);  	
			   	fclose(fp);
			   	
			   	char scp_command[512];
			    	sprintf(scp_command, "/usr/bin/sshpass -p nos3123! scp %s nos3@192.168.100.5:/tmp &",filename);
			   	system(scp_command);		   	
		   	}	
            	}
            	          	
            	if (ISL_AppData.stab_visible != ISL_AppData.prec_visible) {
            	
            		//printf("***** ISL: Change in Ground Station Visibility, now = %d. \n",ISL_AppData.visible);
            		CFE_EVS_SendEvent(ISL_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "Change in Ground Station Visibility, now = %d.",ISL_AppData.stab_visible); //TODO
		    	ISL_AppData.prec_visible = ISL_AppData.stab_visible;
		    	//uint16 localSatId = (uint16)CFE_PSP_GetSpacecraftId();
		    	char filename[32];
		    	sprintf(filename, "/tmp/visi%d.txt",localSatId);
		    			    	
		    	FILE* fp = fopen(filename,"w");
		   	fprintf(fp, "%d\n", ISL_AppData.stab_visible);  	
		   	fclose(fp);
		   	
		   	char scp_command[512];
		    	sprintf(scp_command, "/usr/bin/sshpass -p nos3123! scp %s nos3@192.168.100.5:/tmp &",filename);
		   	system(scp_command);
	   	}
          	            	
            }
            else  {
            	ISL_AppData.stab_visible = true;
            }
            //printf(" Satellite visibility = %d \n",ISL_AppData.stab_visible);
            break;

        /*
        ** All other invalid messages that this app doesn't recognize, 
        ** increment the command error counter and log as an error event.  
        */
        default:
            ISL_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(ISL_PROCESS_CMD_ERR_EID,CFE_EVS_EventType_ERROR, "ISL: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
} 


/*
** Process ground commands
** TODO: Add additional commands required by the specific component
*/
void ISL_ProcessGroundCommand(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(ISL_AppData.MsgPtr, &MsgId);

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(ISL_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case ISL_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification 
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (ISL_VerifyCmdLength(ISL_AppData.MsgPtr, sizeof(ISL_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(ISL_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "ISL: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case ISL_RESET_COUNTERS_CC:
            if (ISL_VerifyCmdLength(ISL_AppData.MsgPtr, sizeof(ISL_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(ISL_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "ISL: RESET counters command received");
                ISL_ResetCounters();
            }
            break;

        /*
        ** Enable Command
        */
        case ISL_ENABLE_CC:
            if (ISL_VerifyCmdLength(ISL_AppData.MsgPtr, sizeof(ISL_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(ISL_CMD_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "ISL: Enable command received");
                ISL_Enable();
            }
            break;

        /*
        ** Disable Command
        */
        case ISL_DISABLE_CC:
            if (ISL_VerifyCmdLength(ISL_AppData.MsgPtr, sizeof(ISL_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(ISL_CMD_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "ISL: Disable command received");
                ISL_Disable();
            }
            break;

        /*
        ** TODO: Edit and add more command codes as appropriate for the application
        ** Set Configuration Command
        ** Note that this is an example of a command that has additional arguments
        */
        case ISL_CONFIG_CC:
            if (ISL_VerifyCmdLength(ISL_AppData.MsgPtr, sizeof(ISL_Config_cmd_t)) == OS_SUCCESS)
            {
                uint32_t config = ntohl(((ISL_Config_cmd_t*) ISL_AppData.MsgPtr)->DeviceCfg); // command is defined as big-endian... need to convert to host representation
                uint16 scid = CFE_PSP_GetSpacecraftId();
                //CFE_EVS_SendEvent(ISL_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "ISL: Configuration command received: %u", config);
                /* Command device to send HK */
                //status = ISL_CommandDevice(&ISL_AppData.IslUart, ISL_DEVICE_CFG_CMD, config);
                CFE_EVS_SendEvent(ISL_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "Configuration command received for file: %s", ((ISL_Config_cmd_t*) ISL_AppData.MsgPtr)->RouteFilename);
                
#ifdef ISL_LOOP_CHECK
    		if(checkAllRoutesForLoops(((ISL_Config_cmd_t*) ISL_AppData.MsgPtr)->RouteFilename)) {
#endif
		        status = ISL_RouteConfigInit (scid, ((ISL_Config_cmd_t*) ISL_AppData.MsgPtr)->RouteFilename);
		        if (status == OS_SUCCESS)
		        {                  
		        	  if (status >= 0) 
		        	  {
		        	  	 CFE_EVS_SendEvent(ISL_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "Route configuration for Sat %d : ", scid);
	   				 //printf ("Route configuration for Sat %d : ", scid);	   				 
	    				 for (unsigned int i = 0; i <= NB_SATELLITES; i++)
	      				 {
						printf ("%d ", ISL_AppData.routeTo[i]);
	      				 }
		  			 printf ("\n");                             
	  			  }             
		            ISL_AppData.HkTelemetryPkt.DeviceCount++;
		        }
		        else
		        {
		            ISL_AppData.HkTelemetryPkt.DeviceErrorCount++;
		        }
		    }
#ifdef ISL_LOOP_CHECK
		else 
		printf("The provided routing table contains loops. Refused \n");
            }
#endif
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            ISL_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(ISL_CMD_ERR_EID, CFE_EVS_EventType_ERROR, 
                "ISL: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 

/*
** This function send a watchdog message to all neighbour ISL. 
**
*/

void ISL_ManageWatchdog(void)
{
	ISL_SendWatchdog();  // send wathcdog to neighbour
	ISL_CheckWatchdog(); // check that neighbour sent a watchdog
}

void ISL_SendWatchdog(void)
{
	uint16 localSatId = (uint16)CFE_PSP_GetSpacecraftId();
	uint16_t crc_computed;
	int32 status;
	uint16 transSatId = localSatId + MAX_WD_NBSAT;   // TODO this limit the watchdog to MAX_WD_NBSAT sat

	//static uint8 command[] = {0x02,0x20,0x01,0x10,0x11,0x00,0x00,0x00}; // with TTL // TC like
	static uint8 command[]   = {0x02,0x1A,0xCF,0xFC,0x1D,0x06,0x00,0xDE,0xAD,0xBE,0xEF,0x00,0x00}; // with TTL // with syncro marker 1ACFFC1D // TM like 
	//command[2] = transSatId & 255; //TC like 
	command[6] = (transSatId & 0x0F) << 4; //TM like 
	crc_computed = Crypto_Calc_CRC16(command+1, sizeof(command)-3); // 2 for CRC and 1 for TTL 
	command[sizeof(command)-1] = (crc_computed >> 8) & 255;
	command[sizeof(command)] = crc_computed & 255;
	
	
	/*// NB: transmit watchdog to ground (0) if possible
	if ((REAL_VISIBILITY && ISL_AppData.visible) || (!REAL_VISIBILITY && ISL_AppData.routeTo[0] == 0 )) {   // 0 is the ground
	    	char *nextAddr = ISL_IpFromSatId (0);
		uint16 nextPort = ISL_CFG_ISL_TM_DST_PORT;		
	    	uint8 *msgAdr = command;
	    	int32 nbBytes = sizeof(command);
	    	//printf(" i = %d \n",i);
	    		// Transmit TF 
		status = IO_TransUdpSetDestAddr (&ISL_AppData.Udp_TMSnd, nextAddr, nextPort);
		if (status < 0 ) {
			CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot configure UDP for watchdog. Err= %d", status);
			return;
		}

		status = IO_TransUdpSnd (&ISL_AppData.Udp_TMSnd, msgAdr, nbBytes);
		if (status < 0 ) {
			CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot send watchdog  Err= %d", status);
			return;
		}
	}	
	// send watchdog to neighbour sat
	for(int i = 1; i < NB_SATELLITES + 1; i++) {
	    if (i != localSatId && ISL_AppData.routeTo[i] == i) {   // i is a neighbour of this sat 
	    	char *nextAddr = ISL_IpFromSatId (i);
		uint16 nextPort = ISL_CFG_ISL_TM_DST_PORT;		
	    	uint8 *msgAdr = command;
	    	int32 nbBytes = sizeof(command);
	    	//printf(" i = %d \n",i);
	    		// Transmit TF 
		status = IO_TransUdpSetDestAddr (&ISL_AppData.Udp_TMSnd, nextAddr, nextPort);
		if (status < 0 ) {
			CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot configure UDP for watchdog. Err= %d", status);
			return;
		}

		status = IO_TransUdpSnd (&ISL_AppData.Udp_TMSnd, msgAdr, nbBytes);
		if (status < 0 ) {
			CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot send watchdog  Err= %d", status);
			return;
		}
	    }
	}*/ 
	
	for(unsigned int i = 0; i < NB_SATELLITES + 1; i++) {
	    if (i != localSatId && ISL_AppData.routeTo[i] == i) {   // i is a neighbour of this sat 
	    	char *nextAddr = ISL_IpFromSatId (i);
		uint16 nextPort = ISL_CFG_ISL_TM_DST_PORT;		
	    	uint8 *msgAdr = command;
	    	int32 nbBytes = sizeof(command);
	    	//printf(" i = %d \n",i);
	    		// Transmit TF 
		status = IO_TransUdpSetDestAddr (&ISL_AppData.Udp_TMSnd, nextAddr, nextPort);
		if (status < 0 ) {
			CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot configure UDP for watchdog. Err= %d", status);
			return;
		}

		status = IO_TransUdpSnd (&ISL_AppData.Udp_TMSnd, msgAdr, nbBytes);
		if (status < 0 ) {
			CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot send watchdog  Err= %d", status);
			return;
		}
	    }
	} 

}

void ISL_CheckWatchdog(void)
{

	OS_time_t currtime;
	CFE_PSP_GetTime(&currtime);
	uint16 localSatId = (uint16)CFE_PSP_GetSpacecraftId();


	for(unsigned int i = 0; i < NB_SATELLITES + 1; i++) {
	    if (i != localSatId && ISL_AppData.routeTo[i] == i) {   // i is a neighbour of this sat 
	    	if(OS_TimeGetTotalSeconds(currtime) - ISL_AppData.LastCom[i] > WATCHDOG_TIMEOUT) {
	    	    //printf("the link between sat %d and sat %d is down \n",localSatId,i); //TODO replace with CFE print
	    	    /*CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "the link between sat %d and sat %d is down",localSatId,i);*/
	    	    ISL_AppData.upMessageSent[i] = false;
	    	   //printf(" now = %lld \n",OS_TimeGetTotalSeconds(currtime));
	    	   //printf(" LastCom = %lld \n",ISL_AppData.LastCom[i]);
	    	}
	    	else if(!ISL_AppData.upMessageSent[i]){
		    ISL_AppData.upMessageSent[i] = true;
		    //printf("the link between sat %d and sat %d is up \n",localSatId,i);
		    CFE_EVS_SendEvent(ISL_WARN_EID, CFE_EVS_EventType_INFORMATION, "the link between sat %d and sat %d is up",localSatId,i);
	    	}
	    }
	}  
}

/*
** Returns 1 if the pattern is found, 0 otherwise
** 
*/

int findPatternInString(const uint8_t *vec, size_t vecLen, const uint8_t *pattern, size_t patternLen) {
    if (patternLen == 0 || vecLen < patternLen) {
    	return 0;
    }
    
    // iterate through vec, up to the point pattern could fit
    for (size_t i = 0; i <= vecLen - patternLen; ++i) {
        size_t j;
        for (j = 0; j < patternLen; ++j) {
            if (vec[i + j] != pattern[j]) break;
        }
        if (j == patternLen) return 1; // pattern matched
    }
    return 0;
}



/*
** Process Telemetry Request - Triggered in response to a telemetery request
** TODO: Add additional telemetry required by the specific component
*/
void ISL_ProcessTelemetryRequest(void)
{
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(ISL_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(ISL_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case ISL_REQ_HK_TLM:
            ISL_ReportHousekeeping();
            break;

        case ISL_REQ_DATA_TLM:
            ISL_ReportDeviceTelemetry();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            ISL_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(ISL_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR, 
                "ISL: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}


/* 
** Report Application Housekeeping
*/
void ISL_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (ISL_AppData.HkTelemetryPkt.DeviceEnabled == ISL_DEVICE_ENABLED)
    {
        status = ISL_RequestHK(&ISL_AppData.IslUart, (ISL_Device_HK_tlm_t*) &ISL_AppData.HkTelemetryPkt.DeviceHK);
        if (status == OS_SUCCESS)
        {
            ISL_AppData.HkTelemetryPkt.DeviceCount++;
        }
        else
        {
            ISL_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(ISL_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "ISL: Request device HK reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &ISL_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &ISL_AppData.HkTelemetryPkt, true);
    return;
}


/*
** Collect and Report Device Telemetry
*/
void ISL_ReportDeviceTelemetry(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (ISL_AppData.HkTelemetryPkt.DeviceEnabled == ISL_DEVICE_ENABLED)
    {
        status = ISL_RequestData(&ISL_AppData.IslUart, (ISL_Device_Data_tlm_t*) &ISL_AppData.DevicePkt.Isl);
        if (status == OS_SUCCESS)
        {
            ISL_AppData.HkTelemetryPkt.DeviceCount++;
            /* Time stamp and publish data telemetry */
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &ISL_AppData.DevicePkt);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &ISL_AppData.DevicePkt, true);
        }
        else
        {
            ISL_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(ISL_REQ_DATA_ERR_EID, CFE_EVS_EventType_ERROR, 
                    "ISL: Request device data reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */
    return;
}


/*
** Reset all global counter variables
*/
void ISL_ResetCounters(void)
{
    ISL_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    ISL_AppData.HkTelemetryPkt.CommandCount = 0;
    ISL_AppData.HkTelemetryPkt.DeviceErrorCount = 0;
    ISL_AppData.HkTelemetryPkt.DeviceCount = 0;
    return;
} 


/*
** Enable Component
** TODO: Edit for your specific component implementation
*/
void ISL_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is disabled */
    if (ISL_AppData.HkTelemetryPkt.DeviceEnabled == ISL_DEVICE_DISABLED)
    {
        /*
        ** Initialize hardware interface data
        ** TODO: Make specific to your application depending on protocol in use
        ** Note that other components provide examples for the different protocols available
        */ 
        ISL_AppData.IslUart.deviceString = ISL_CFG_STRING;
        ISL_AppData.IslUart.handle = ISL_CFG_HANDLE;
        ISL_AppData.IslUart.isOpen = PORT_CLOSED;
        ISL_AppData.IslUart.baud = ISL_CFG_BAUDRATE_HZ;
        ISL_AppData.IslUart.access_option = uart_access_flag_RDWR;

        /* Open device specific protocols */
        status = uart_init_port(&ISL_AppData.IslUart);
        if (status == OS_SUCCESS)
        {
            ISL_AppData.HkTelemetryPkt.DeviceCount++;
            ISL_AppData.HkTelemetryPkt.DeviceEnabled = ISL_DEVICE_ENABLED;
            CFE_EVS_SendEvent(ISL_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "ISL: Device enabled");
        }
        else
        {
            ISL_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(ISL_UART_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: UART port initialization error %d", status);
        }
    }
    else
    {
        ISL_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(ISL_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Device enable failed, already enabled");
    }
    return;
}


/*
** Disable Component
** TODO: Edit for your specific component implementation
*/
void ISL_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (ISL_AppData.HkTelemetryPkt.DeviceEnabled == ISL_DEVICE_ENABLED)
    {
        /* Open device specific protocols */
        status = uart_close_port(&ISL_AppData.IslUart);
        if (status == OS_SUCCESS)
        {
            ISL_AppData.HkTelemetryPkt.DeviceCount++;
            ISL_AppData.HkTelemetryPkt.DeviceEnabled = ISL_DEVICE_DISABLED;
            CFE_EVS_SendEvent(ISL_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "ISL: Device disabled");
        }
        else
        {
            ISL_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(ISL_UART_CLOSE_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: UART port close error %d", status);
        }
    }
    else
    {
        ISL_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(ISL_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Device disable failed, already disabled");
    }
    return;
}


/*
** Verify command packet length matches expected
*/
int32 ISL_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length)
{     
    int32 status = OS_SUCCESS;
    CFE_SB_MsgId_t msg_id = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code = 0;
    size_t actual_length = 0;

    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        ISL_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(ISL_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        ISL_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
} 

void ISL_ISL_Task (void)
{
    int32 status = OS_SUCCESS;
    int32 ndReadyFd = 0;

   CFE_EVS_SendEvent(ISL_TASK_REG_INF_EID, CFE_EVS_EventType_INFORMATION, "ISL: Child task registration complete");

	/*
	** Device Run Loop
	*/
	while (CFE_ES_RunLoop(&ISL_AppData.RunStatus) == true)
	{

		/* Wait input from any UDP input */
		while(ndReadyFd >= 0)
		{
			ndReadyFd = IO_TransSelectInput(&ISL_AppData.rcv_selector,
									   IO_TRANS_PEND_FOREVER);

			if (ndReadyFd > 0)
			{
				if (IO_TransSelectFdInActive(&ISL_AppData.rcv_selector, ISL_AppData.TC_Rcv.sockId))
				{
					ISL_ManageTcInput(&ISL_AppData.TC_Rcv);
				}
				else if (IO_TransSelectFdInActive(&ISL_AppData.rcv_selector, ISL_AppData.TM_Rcv.sockId))
				{
					ISL_ManageTmInput(&ISL_AppData.TM_Rcv);
				}
				else
				{
					CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR,
									  "ISL: Unexpected Active Device. "
									  "Quitting ISL infinite loop.");
					break;
				}
			}
		}
	}
}

void ISL_ManageTcInput(IO_TransUdp_t *udp)
{
	int32 status;
	struct {
		uint8 ttl;
		uint8 frameBuffer[ISL_CUSTOM_CLTU_BUFF_SIZE] /*= {0}*/;
	} s;

    int32 readBytes = 0;
    int32 nbBytes = 0;   // To retransmit
    int16 destSatId = 0;
    uint16 localSatId = (uint16)CFE_PSP_GetSpacecraftId();
    int nextSatId = 0;
    char *nextAddr;
    uint8 *msgAdr;		// To retransmit
    uint16 nextPort = 0;

    readBytes = IO_TransUdpRcv(udp, &s.ttl, sizeof(s));

    if (readBytes <= 0 ) {
    	CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot read TC from Feeder or ISL. Err= %d", readBytes);
    	return;
    }

    if (s.ttl && s.ttl != ISL_NO_TTL) {
    	s.ttl--;
    }
    if (s.ttl == 0) {
    	CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: TC packet dropped due to zero ttl");
    	ISL_AppData.HkTelemetryPkt.DeviceHK.DropPktCounter++;
    	return;
    }

    int32 tc_readBytes = readBytes - 1; //bytes read minus TTL field's size
	//IRT CSS - IDS
	OS_time_t currtime;
	#if defined LOGGING_P0_ENABLED || defined LOGGING_P1_ENABLED || defined IDS_P1_ENABLED		
	CFE_PSP_GetTime(&currtime);
	#endif

    #if defined LOGGING_P0_ENABLED || defined LOGGING_P1_ENABLED
    uint8_t attack_tag = (s.frameBuffer[0] & 0x04) > 0; //0b00000100, 1 if there is an attack tag (4>0) but false if there is none (0), pseudo-conversion to bool
    struct timespec realtime;
    timespec_get(&realtime, TIME_UTC);
    unsigned int packet_seconds = realtime.tv_sec - sim_start_time;
    #endif

	//IDS - P0
	#ifdef LOGGING_P0_ENABLED
	update_Log_P0(log_fp_0, currtime, tc_readBytes, s.frameBuffer, packet_seconds, &realtime, P0_NOT_ACTIVE, attack_tag); //No IDS output for P0 as there is no detection at the moment here
	#endif
    //IDS - P0
	
	/*printf("trame TC : ");
	for (int i = 0; i< readBytes; i++) {
		printf("%02x", *(uint8*)(&ttl+i));
	}
	printf("\n");*/
	
	#ifdef IDS_P1_ENABLED
	char rate_is_allowed = true;
	char CRC_is_correct = true;
	char SPI_valid = true;
	char SPI_safe = true;
    char Block_len_correct = true;
	
    uint8_t *frame_ptr_dup;
    frame_ptr_dup = s.frameBuffer;
    CRC_is_correct = is_CRC_correct(frame_ptr_dup, tc_readBytes);	
    if (!CRC_is_correct) { 
        #ifdef LOGGING_P1_ENABLED
        update_Log_P1(log_fp_1, currtime, tc_readBytes, s.frameBuffer, packet_seconds, &realtime, 1, attack_tag); //1 : packet not allowed due to bad CRC but is not an attack !
        #endif
    	printf("TC bad CRC, packet dropped\n"); //TODO replace with CFE event
        IDS_AppData.HkTelemetryPkt.DeviceHK.SuccessiveBadCrc++;
	    return;  // bad packets are dropped.
	}
    else
    {
        IDS_AppData.HkTelemetryPkt.DeviceHK.SuccessiveBadCrc = 0;
    }
	#endif	

    destSatId = TCTF_RD_SCID((ccsdsTfHeader_t*)s.frameBuffer);	// destination satellite is in TF header
    //printf("***************Sat %d ISL received TC for SAT %d \n",localSatId,destSatId);
         
    // watchdog packets 
    if(destSatId >= MAX_WD_NBSAT) { // this is specific to watchdog messages     	    
        unsigned int origSatId = destSatId - MAX_WD_NBSAT; //TODO this limit watchdog to MAX_WD_NBSAT sat // this is original satId for watchdog
        if(origSatId < NB_SATELLITES+1) { // valid origSatId
        	ISL_AppData.LastCom[origSatId] = OS_TimeGetTotalSeconds(currtime);
        	//printf("watchdog TC origSatId = %d \n", origSatId); 
        }
        return;    
    }
    
    
    if (destSatId == localSatId) {	// TC is arrived at destination forward to CI without ttl
    	nextAddr = ISL_CFG_FSW_IP;
    	nextPort = ISL_CFG_FSW_DST_PORT;
    	
        //IDS - P1
        #ifdef IDS_LATENCY_BENCHMARK
        struct timespec realtime;
        timespec_get(&realtime, TIME_UTC);
        fprintf(log_fp_1,"benchmark : %ld.%09ld\n", realtime.tv_sec, realtime.tv_nsec);
        fflush(log_fp_1);
        #endif
        
        #ifdef IDS_P1_ENABLED
        if (tc_readBytes > 10)
        {
            uint16_t spi;
            spi = ((uint8_t)s.frameBuffer[6] << 8) | (uint8_t)s.frameBuffer[7];
            //printf("spi : %d\n",spi);
            if (spi >= 64)
            {
                SPI_valid = false;
                printf("invalid spi ! bool holder : %d\n", SPI_valid);
            }
            else if (!spi_is_initialized(spi))
            {
                SPI_safe = false;
            }

            Block_len_correct = is_len_correct(tc_readBytes, spi);
            //printf("block len correct : %d\n", Block_len_correct);
        }
        //else we have a watchdog or a packet too short to have SDLS with a security header

        rate_is_allowed = is_rate_allowed(currtime);
        if (rate_is_allowed)
            already_flooding = 0;
        #endif

        #ifdef LOGGING_P1_ENABLED
        uint8_t IDS_output;

        #ifdef IDS_P1_ENABLED
        if ((!SPI_safe) || (!SPI_valid) || (!rate_is_allowed))
            IDS_output = 0;
        else
            IDS_output = 1;
        #else
            IDS_output = P1_NOT_ACTIVE;
        #endif
        update_Log_P1(log_fp_1, currtime, tc_readBytes, s.frameBuffer, packet_seconds, &realtime, IDS_output, attack_tag);
        #endif
        //IDS - P1
            
        msgAdr = s.frameBuffer;
    	nbBytes = readBytes - sizeof(s.ttl);
    }
    else {	// Forward to next sat if a route exists for it
    	nextSatId = route (destSatId);
    	if (nextSatId == ISL_NO_ROUTE) {
    		return;
    	}
    	nextAddr = ISL_IpFromSatId (nextSatId);
    	nextPort = ISL_CFG_ISL_TC_DST_PORT;
    	msgAdr = &s.ttl;
    	nbBytes = readBytes;
    }
    
   

    //IRT CSS - IDS
    /*
    ** Anti-flooding & CRC verification :
    ** If not for current satellite, no flooding verification, rate_is_allowed is set to true. Else value is determined by is_rate_allowed() in P1
    ** CRC verification takes place whether dest is current satellite or not
    */
    #ifdef IDS_P1_ENABLED
    if ((rate_is_allowed)&&(SPI_valid)&&(SPI_safe)&&(Block_len_correct))
    {
    #endif
	// Transmit TF to local CI application
	status = IO_TransUdpSetDestAddr (&ISL_AppData.Udp_TCSnd, nextAddr, nextPort);
	if (status < 0 ) {
		CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot configure UDP for local output. Err= %d", status);
		return;
	}

	status = IO_TransUdpSnd (&ISL_AppData.Udp_TCSnd, msgAdr, nbBytes);
	if (status < 0 ) {
		CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot forward TC.  Err= %d", status);
		return;
	}
    #ifdef IDS_P1_ENABLED
    }
    else if (!SPI_valid)
    {
	#ifdef IDS_TELEM_ENABLED
	IDS_RaiseAlert(1, 0, tc_readBytes, 0, s.frameBuffer, NULL);
	#endif
	printf("[P1] invalid SPI detected !\n");
    fprintf(log_attack,"SPI CRASH ATTACK : ");
	for (int i = 0; i< tc_readBytes; i++) {
		fprintf(log_attack,"%02x", s.frameBuffer[i]);
	}
	time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    fprintf(log_attack," : %s",asctime(timeinfo));
	fflush(log_attack);
    }
    else if (!SPI_safe)
    {
	#ifdef IDS_TELEM_ENABLED
	IDS_RaiseAlert(1, 0, tc_readBytes, 0, s.frameBuffer, NULL);
	#endif
	printf("[P1] unsafe SPI detected !\n");
    fprintf(log_attack,"SPI BYPASS ATTACK : ");
	for (int i = 0; i< tc_readBytes; i++) {
		fprintf(log_attack,"%02x", s.frameBuffer[i]);
	}
	time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    fprintf(log_attack," : %s",asctime(timeinfo));
	fflush(log_attack);
    }
    else if (!Block_len_correct)
    {
	#ifdef IDS_TELEM_ENABLED
	IDS_RaiseAlert(1, 0, tc_readBytes, 0, s.frameBuffer, NULL);
	#endif
	printf("[P1] bad block len detected !\n");
    fprintf(log_attack,"BAD BLOCK LEN ATTACK : ");
	for (int i = 0; i< tc_readBytes; i++) {
		fprintf(log_attack,"%02x", s.frameBuffer[i]);
	}
	time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    fprintf(log_attack," : %s",asctime(timeinfo));
	fflush(log_attack);
    }
    else
    {
	#ifdef IDS_TELEM_ENABLED
    if (!already_flooding)
    {
	    IDS_RaiseAlert(1, 0, tc_readBytes, 0, s.frameBuffer, NULL);
        already_flooding = 1;
    }
    else
    {
        //TODO - increment counter of flooding packets / dropped packets
        sem_wait(&(DetectionAlert.sem_detect_count));
        IDS_AppData.HkTelemetryPkt.DeviceHK.PacketsDetected++;
        sem_post(&(DetectionAlert.sem_detect_count));
    }
    #endif
    printf("[P1] packet rate too high, anti-flood enabled\n");
    fprintf(log_attack,"FLOOD : ");
	for (int i = 0; i< tc_readBytes; i++) {
		fprintf(log_attack,"%02x", s.frameBuffer[i]);
	}
    fprintf(log_attack, ",%d,",s.frameBuffer[17]); //checksum SPP, on triche car est chiffré normalement
	time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    fprintf(log_attack," %s",asctime(timeinfo));
	fflush(log_attack);
    }
    #endif

    //IRT CSS - IDS
}

void ISL_ManageTmInput(IO_TransUdp_t *udp)
{
    int32 status;
    	
    struct {
    	uint8 ttl;
    	uint8 frameBuffer[ISL_CUSTOM_CLTU_BUFF_SIZE+1] /*= {0}*/;
    } s;
              
    int32 readBytes = 0;
    uint16 localSatId = (uint16)CFE_PSP_GetSpacecraftId();
    static uint16 nextSatId = 0;
    uint16 origSatId = 0;
    char *nextAddr;
    static uint16 nextPort = 0;
    char CRC_is_correct = true;
    int nbFrameBytes;
    int iswatchdog = false;
    uint8_t watchdogpattern[] = {0xDE, 0xAD, 0xBE, 0xEF};
   
    // read directly into s.ttl by default 
    readBytes = IO_TransUdpRcv(udp, &s.ttl, ISL_CUSTOM_CLTU_BUFF_SIZE);
    if (readBytes <= 0) {
	CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR,
	    "ISL: Cannot read TM from TO or ISL. Err= %d", readBytes);
	return;
    }
    
    
    int localAddress = isLocalIp(&udp->srcAddr.sin_addr);
    
    iswatchdog = findPatternInString(&s.ttl, 20, watchdogpattern, sizeof(watchdogpattern)); // look in the first 20 octets
    //printf("is watchdog = %d \n", iswatchdog);
    
    if ((localAddress)&&(!iswatchdog)) { // Local packets DO NOT include TTL byte! // watchdog includes TTL 
	//readBytes = IO_TransUdpRcv(udp, (void*) &s.frameBuffer, ISL_CUSTOM_CLTU_BUFF_SIZE+1);
	memmove((void*) &s.frameBuffer,(void*) &s.ttl, readBytes);  // in local we receive a framebuffer without TTL so we need to realign
	s.ttl = ISL_TTL_INIT; // Manually initialize TTL
	nbFrameBytes = readBytes;
    } 
    else {
	// Non-local packets include TTL, read starting at ttl
    	nbFrameBytes = readBytes - 1; // first byte is TTL, rest is frameBuffer
    }
    
    /*          
    //  not working properly - to be modified        
    // recompute the CRC for the TM (if necessary) //TODO  to be checked  
    uint16_t crc_computed = Crypto_Calc_CRC16(s.frameBuffer,nbFrameBytes-2);
    s.frameBuffer[nbFrameBytes-2] = (crc_computed >> 8) & 255;
    s.frameBuffer[nbFrameBytes-1] = crc_computed & 255;
    

    CRC_is_correct = is_CRC_correct(s.frameBuffer,nbFrameBytes); 
	
    if(!CRC_is_correct) {
	printf("ISL: TM bad CRC, packet dropped\n");  //TODO replace with CFE event 
	return;  // bad packets are dropped.
    } //TODO a lot of bad packets when telemetry is activated, why ? (ok only if recomputed before)
    */
            	
    OS_time_t currtime;	    
    origSatId = TMTF_RD_SCID((ccsdsTmTfHeader_t*)s.frameBuffer); // destination satellite is in TF header (4 octets of Synchro marker) 
    //printf("TM SatId = %d \n", origSatId); 
    
    if(origSatId > MAX_WD_NBSAT+NB_SATELLITES+1 ) { 
	printf("ISL: TM SCID ERROR, SatId = %d is too big, packet dropped \n", origSatId);
	//for ( int i = 0; i < 15; i++) {
	//	printf ("%02X ",(unsigned char) s.frameBuffer[i]);
	//}
	//printf (" ... \n");
	return;
    }
    
    // watchdog packets 
    else if(origSatId >= MAX_WD_NBSAT) { // this is specific to watchdog messages 
	unsigned int orSatId = origSatId - MAX_WD_NBSAT; //TODO this limit watchdog to MAX_WD_NBSAT sat 
	if(orSatId < NB_SATELLITES+1) { // valid origSatId
	        CFE_PSP_GetTime(&currtime);
		ISL_AppData.LastCom[orSatId] = OS_TimeGetTotalSeconds(currtime); 
		//printf("watchdog TM origSatId = %d \n", orSatId);
	}	  
	return;    
    } 
    
    else if(origSatId < MAX_WD_NBSAT) {
    
	    // other TM packets     
	   // if(ISL_AppData.ISL_TmDestinationSet == false) {  // pre-computation (valid only for static routing)
	        nextSatId = route (GROUND_ID);
	        nextAddr = ISL_IpFromSatId (nextSatId);

	   if (nextSatId == GROUND_ID) {	// The satellite is connected to ground
		nextPort = ISL_CFG_FEEDER_DST_PORT;
	   }
	   else {
		nextPort = ISL_CFG_ISL_TM_DST_PORT;
	   }

	   // Forward TM
	   status = IO_TransUdpSetDestAddr (&ISL_AppData.Udp_TMSnd, nextAddr, nextPort);
	   if (status < 0 ) {
		CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot configure UDP for local output. Err= %d", status);
		return;
	   }
			//ISL_AppData.ISL_TmDestinationSet = true; //TODO do not work in dynamic routing to be commented
	    //}
	    
	   if (s.ttl && s.ttl != ISL_NO_TTL) { 
		    s.ttl--;
	   }
	   if (s.ttl == 0) {
	            static int num = 0;
	   	    if(num++%10==0) {
		    CFE_EVS_SendEvent(ISL_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "ISL: TM packet dropped due to zero ttl");
		    }
		    ISL_AppData.HkTelemetryPkt.DeviceHK.DropPktCounter++;		    		    
		    return;
	   }

	   //status = IO_TransUdpSnd (&ISL_AppData.Udp_TMSnd, s.frameBuffer, nbFrameBytes + sizeof(s.ttl));
	   if ((nextSatId != GROUND_ID) || (ISL_AppData.stab_visible)) {  
	   	status = IO_TransUdpSnd (&ISL_AppData.Udp_TMSnd, &s.ttl, nbFrameBytes + sizeof(s.ttl));
	   }
	   else if ((nextSatId == GROUND_ID) && (!ISL_AppData.visible)) { // drop if FEEDER required and SAT not visible from ground
	  	static int num = 0;
	   	if(num++%10==0) {
	   	CFE_EVS_SendEvent(ISL_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION, "ISL: TM packet dropped due to SAT not visible from any ground station");
	   	}
	   	return;	
	   }
	   
	    // show the packet
            //if(counter < 30) {
	    //for ( int i = 0; i < (int)(nbFrameBytes); i++) {
	    //for ( int i = 0; i < 20; i++) {
	    //	printf ("%02X ",(unsigned char) s.frameBuffer[i]);
	    //}
	    //printf ("\n");
	    //counter++;
            //}

	   //printf("************** ISL send TM \n");
	   if (status < 0 ) {
		CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Cannot forward TM.  Err= %d", status);
		return;
	   }
   }
   
    return;
}


int route (uint16 destSatId)
{
  if (destSatId > NB_SATELLITES)
    {
        CFE_EVS_SendEvent(ISL_ERR_EID, CFE_EVS_EventType_ERROR, "ISL: Bad Dest Id (%d). Ground used instead", destSatId);
        return GROUND_ID;  // Error => send to ground      
    }
  else
    {
      return ISL_AppData.routeTo[destSatId];
    }
}

char *ISL_IpFromSatId (uint16 satId)			// TODO TO MODIFY, make it more generic
{
	static char address[16];

	if (satId == 0) {
		sprintf (address,SAT_IP_ADR_FMT, 5);		// FrontEnd
	}
	else {
		sprintf (address,SAT_IP_ADR_FMT, satId + 10);
	}
	return address;
}

int loadRoutingTableForCheck(const char *filename) {
		
    char path[1024];  
    sprintf(path,"%s/%s",ISL_rootDataDir,filename);
    
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    char line[1024];
    ISL_AppData.numNodes=0;

    // Skip header line (Dest ID ...)
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return 0;
    }

    int nodeId;
    while (fgets(line, sizeof(line), file)) {
        char *ptr = line;
        
        if (sscanf(ptr, "Node ID\t%d", &nodeId) != 1)
            continue;
        ptr = strchr(ptr, '\t'); // move past "Node ID"
        ptr = strchr(ptr+1, '\t'); // move past "Node ID"
        for (unsigned int dest = 0; dest < NB_SATELLITES + 1 && ptr; dest++) {
            int nextHop;
            if (sscanf(ptr, "%d", &nextHop) != 1)            
                break;
            ISL_AppData.routingTable[nodeId][dest] = nextHop;
            ptr = strchr(ptr + 1, '\t');
        }

        ISL_AppData.numNodes++;
    }

    fclose(file);
    // TO DEBUG
    //for(int sources=0; sources <8; sources++) { 
    	//for(int dest =0; dest <8; dest++) {
    		//printf("%d\t",ISL_AppData.routingTable[sources][dest]);
    		//}
    		//printf("\n");
    		//}  
    return 1;
}


int detectLoop(int src, int dest) {
    unsigned int visited[NB_SAT_MAX + 1] = {0};
    int current = src;

    while (current != dest) {
        if (visited[current]) {
            return 1; // Loop detected
        }
        visited[current] = 1;
        current = ISL_AppData.routingTable[current][dest];
    }

    return 0; // No loop
}

bool checkAllRoutesForLoops(char* filename) {

    if(!loadRoutingTableForCheck(filename)) {
    	printf("cannot read the table %s for loop check \n", filename);
    	return false;    	
    }
    
    for (int src = 0; src < ISL_AppData.numNodes; src++) {
        for (int dest = 0; dest < ISL_AppData.numNodes; dest++) {
            if (src == dest) continue;
            if (detectLoop(src, dest)) {
                printf("Loop detected from Node %d to Destination %d\n", src, dest);
                return false;
            }
        }
    }
    return true;
}
