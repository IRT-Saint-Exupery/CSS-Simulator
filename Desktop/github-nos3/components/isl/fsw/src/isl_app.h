/*******************************************************************************
** File: isl_app.h
**
** Purpose:
**   This is the main header file for the ISL application.
**
*******************************************************************************/
#ifndef _ISL_APP_H_
#define _ISL_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "isl_device.h"
#include "isl_events.h"
#include "isl_platform_cfg.h"
#include "isl_perfids.h"
#include "isl_msg.h"
#include "isl_msgids.h"
#include "isl_version.h"
#include "hwlib.h"
#include "trans_udp.h"
#include "trans_select.h"
//#include "isl_visible.h"


/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define ISL_PIPE_DEPTH            32


/*
** Specified maximum local IP @
*/
#define MAX_LOCAL_IP            32


/*
** Enabled and Disabled Definitions
*/
#define ISL_DEVICE_DISABLED       0
#define ISL_DEVICE_ENABLED        1

/*
** Routes
*/
#define ISL_TTL_INIT 10
#define ISL_TTL_CHECK	1			// Replace by 0 to suppress ttl feature
#define ISL_NO_TTL	(ISL_TTL_INIT + ISL_TTL_CHECK)
#define ISL_NO_ROUTE	-1

/*
** activate / deactivate the loop checking on routing tables.
*/
#define ISL_LOOP_CHECK                    // comment to suppress the loop checking   

typedef struct
	{
	        unsigned char Id[6];
	} ccsdsTfHeader_t;
	
typedef struct
	{
		unsigned char synchro[4]; 
	        unsigned char Id[6];
	} ccsdsTmTfHeader_t;
	
#define TMTF_RD_SCID(hdr)	(((hdr)->Id[0]  & 0x3F) << 4) | (((hdr)->Id[1] & 0xF0) >> 4)
#define TCTF_RD_SCID(hdr)	((((hdr)->Id[0] & 0x03) << 8) | ((hdr)->Id[1]))

/*
** visibility computation parameters and ground station 
*/
#define REAL_VISIBILITY 	1                    // 1 - real visibility is considered for communications // 0 - always visible
#define MIN_ELEV_DEG  		3  //default 3       // min ground station elevation deg
// Svalbard Satellite Station 
#define GS_LAT_DEG   		78.228       // ground station LAT deg
#define GS_LON_DEG   		15.399       // ground station LON deg
// Toulouse 
//#define GS_LAT_DEG   43.5634       // ground station LAT deg
//#define GS_LON_DEG   1.4885        // ground station LON deg
// Sydney
//#define GS_LAT_DEG   -33.8727   // ground station LAT deg
//#define GS_LON_DEG   151.2027   // ground station LON deg


/*
** watchdog param 
*/
#define MAX_WD_NBSAT  100   // watchdog messages uses SCID 100, 101, 102, ... 100 + NB_SAT by default (= max number of SAT for ISL watchdog is limited to 100)

#define MAX_STATIONS  100  // max number of ground stations for visi computation 


/*
** GPS Novatel Parameters
*/
typedef struct
{
    double Weeks;
    double SecondsIntoWeek;
    double Fractions;
    double ECEFX;
    double ECEFY;
    double ECEFZ;
    double VelX;
    double VelY;
    double VelZ;
} __attribute__((packed)) Generic_Gps_Tlm_Payload_t; 


typedef struct {
    double latitude;
    double longitude;
    bool exists;
    const char* name;
} LatLong;

/*
** ISL global data structure
** The cFE convention is to put all global app data in a single struct. 
** This struct is defined in the `isl_app.h` file with one global instance 
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    ISL_Hk_tlm_t   HkTelemetryPkt;   /* ISL Housekeeping Telemetry Packet */
    
    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t * MsgPtr;             /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t CmdPipe;            /* Pipe Id for HK command pipe */
    uint32 RunStatus;                   /* App run status for controlling the application state */

    bool ISL_TmDestinationSet;

    /*
	** Device data 
    ** TODO: Make specific to your application
	*/
    ISL_Device_tlm_t DevicePkt;      	/* Device specific data packet */
	uint32 DeviceID;		            /* Device ID provided by CFS on initialization */


    /* 
    ** Device protocol
    */ 
	/* input local sockets */
    IO_TransUdp_t TC_Rcv;
    IO_TransUdp_t TM_Rcv;

    IO_TransSelect_t  rcv_selector;

	/* local socket for output */
    IO_TransUdp_t Udp_TCSnd;
    IO_TransUdp_t Udp_TMSnd;

    struct in_addr ipv4[MAX_LOCAL_IP]; /* array of local adresses */
    size_t ipv4Count;

    uart_info_t IslUart;             /* Hardware protocol definition */

    unsigned int routeTo[NB_SAT_MAX + 1];  /* Routing table for current sat. Indice 0 is for ground */

    int routingTable[NB_SAT_MAX + 1][NB_SAT_MAX + 1];  /* for loop checking*/
    int numNodes; /* for loop checking*/
    
    int64 LastCom[NB_SAT_MAX + 1]; // time of last watchdog com for each neighbour sat 
    
    Generic_Gps_Tlm_Payload_t Gps;  
    
    int visible; 
    int prec_visible;
    int stab_visible;
    
    bool upMessageSent[NB_SAT_MAX + 1]; 
    
    LatLong stations[MAX_STATIONS];
    int nb_stations;
    
} ISL_AppData_t;



/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern ISL_AppData_t ISL_AppData; /* ISL App Data */


/*
**
** Local function prototypes. 
**
** Note: Except for the entry point (ISL_AppMain), these
**       functions are not called from any other source module.
*/
void  ISL_AppMain(void);
int32 ISL_AppInit(void);
int32 cacheLocalIps(void);
int isLocalIp (struct in_addr *ip);
int32 ISL_RouteConfigInit(uint16 scid, const char* routeConfigFile);
int32  readNextIdsFromStringOfIds (const char * stringOfScids);
int32 udpInit (IO_TransUdp_t *udp, uint16 usPort);
void  ISL_ProcessCommandPacket(void);
void  ISL_ProcessGroundCommand(void);
void  ISL_ProcessTelemetryRequest(void);
void  ISL_ReportHousekeeping(void);
void  ISL_ReportDeviceTelemetry(void);
void  ISL_ResetCounters(void);
void  ISL_Enable(void);
void  ISL_Disable(void);
int32 ISL_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 expected_length);
void  ISL_ISL_Task (void);
void  ISL_ManageTcInput(IO_TransUdp_t *udp);
void  ISL_ManageTmInput(IO_TransUdp_t *udp);
int   route (uint16 destSatId);
char *ISL_IpFromSatId (uint16 satId);
int   loadRoutingTableForCheck(const char *filename);
int   detectLoop(int src, int dest);
bool  checkAllRoutesForLoops(char* filename);
void ISL_ManageWatchdog(void);
void ISL_SendWatchdog(void);
void ISL_CheckWatchdog(void);
void Generic_ISL_ingest_generic_Gps(CFE_MSG_Message_t * Msg, Generic_Gps_Tlm_Payload_t *Gps);
int is_satellite_visible(double gs_lat_deg, double gs_lon_deg, double min_elev_deg, double sat_x, double sat_y, double sat_z);
int is_satellite_visible_allMCS(double min_elev_deg, double sat_x, double sat_y, double sat_z, double sat_nx, double sat_ny, double sat_nz);
int is_satellite_visible_now_allMCS(double min_elev_deg, double sat_x, double sat_y, double sat_z);
void satellite_propagation_ecef(double x_prev, double y_prev, double z_prev,
                                double x_curr, double y_curr, double z_curr,
                                double *x, double *y, double *z);
int parseGroundStations(const char* filename);

#endif /* _ISL_APP_H_ */
