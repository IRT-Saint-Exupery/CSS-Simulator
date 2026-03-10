/******************************************************************************/
/** \file  ci_custom.c
*
*   Copyright 2017 United States Government as represented by the Administrator
*   of the National Aeronautics and Space Administration.  No copyright is
*   claimed in the United States under Title 17, U.S. Code.
*   All Other Rights Reserved.
*  
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Function Definitions for Custom Layer of CI Application for UDP
*       with Telecommand Transfer Frames.
*
*   \par
*     This file defines the functions for a custom implementation of the custom
*     layer of the CI application over UDP accepting TCTF. 
*
*   \par API Functions Defined:
*     - CI_CustomInit() - Initialize the transport protocol, create child task
*     - CI_CustomAppCmds() - Process custom App Commands
*     - CI_CustomEnableTO() - Send msg to TO to enable downlink
*     - CI_CustomCleanup() - Cleanup callback to close transport channel.
*     - CI_CustomMain() - Main entry point for the custom child task. 
*     - CI_CustomGateCmds() - Process custom Gate Commands
*
*   \par Limitations, Assumptions, External Events, and Notes:
*     - All input messages are Communication Link Data Units (CLTUs)
*     - All config macros defined in ci_platform_cfg.h
*     - ciMutex must be used whenever g_CI_AppData is accessed.
*
*   \par Modification History:
*     - 2015-08-01 | Guy de Carufel | Code Started
*******************************************************************************/

/*
** Pragmas
*/

/*
** Include Files
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cfe.h"
#include "network_includes.h"

#include "tc_sync.h"
#include "cop1.h"
#include "trans_select.h"
#include "trans_udp.h"
#include "trans_rs422.h"

#include "ci_app.h"
#include "ci_platform_cfg.h"
#include "to_mission_cfg.h"

//LOUIS IRT CSS - IDS
#include "ids_p2.h"
#include <time.h>
//LOUIS IRT CSS - IDS ALERT REPORTING
#include "ids_app.h"
//LOUIS IRT CSS - TID TEST
#include <sys/types.h>

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

typedef struct
{
    TCTF_ChannelService_t   chnlService;
    uint8                   pktBuff[CI_CUSTOM_BUFFER_SIZE];  
    TO_CustomSetOcfCmd_t    clcwCmd;    /**< CLCW Update Command      */
} CI_CustomVChnl_t;

typedef struct
{
    CI_CustomVChnl_t        vChnls[CI_CUSTOM_TF_CHANNELS];
    uint8                   tfBuff[CI_CUSTOM_TF_BUFF_SIZE];
} CI_CustomMChnl_t;

typedef struct
{
    CI_CustomMChnl_t     mc;        /**< Master channel             */
    bool              cltuRand;  /**< Is the cltu code blocks 
                                          randomized                */
    uint8                cltuBuff[CI_CUSTOM_CLTU_BUFF_SIZE];
} CI_CustomPChnl_t;

typedef struct
{
    IO_TransUdp_t        udp;       /**< Socket structure           */
    CI_CustomPChnl_t     pc;        /**< Physical channel           */
} CI_CustomPChnlUdp_t;

typedef struct
{
    IO_TransSelect_t       select;       /**< Select struct          */
    CI_CustomPChnlUdp_t    pcSocket;     /**< Udp Master Channel     */
    TO_EnableOutputCmd_t   toEnableCmd;  /**< TO Enable CMD msg      */
} CI_CustomData_t;

/*
** External Global Variables
*/
extern CI_AppData_t g_CI_AppData;

/*
** Global Variables
*/
CI_CustomData_t g_CI_CustomData;

/*
** Local Variables
*/
// LOUIS IRT CSS
#ifdef LOGGING_P2_ENABLED
static FILE *log_fp;
extern unsigned long sim_start_time;
#endif
#ifdef IDS_P2_ENABLED
static FILE *log_attack;
#endif
#ifdef CPU_BENCHMARK_TEST
static FILE *tid_file;
#endif

//static int log_fd;

/*
** Local Function Definitions
*/
static int32 CI_CustomReadCltuSocket(void);


/*******************************************************************************
** Custom Application Functions (Executed by Main Task)
*******************************************************************************/

/******************************************************************************/
/** \brief Custom Initialization
*******************************************************************************/
int32 CI_CustomInit(void)
{
    uint8 scId = CFE_PLATFORM_TBL_VALID_SCID_1;
    int32 iStatus = CI_ERROR;
    uint32 taskId = 0;
    IO_TransUdpConfig_t configSocket;
    
//    void Crypto_Init_SetScId(unsigned int scId);
//    Crypto_Init_SetScId(1); // set the SCID for Cryptolib
    
    /* Init as errors */
    g_CI_CustomData.pcSocket.udp.sockId = -1;
    /*
    **PARSE RULES & GENERATE ARRAYS, MOVE LATER TO INIT
    */
    #ifdef IDS_P2_ENABLED
    init_P2();
    #endif

    /* Initialize the UDP socket */
    CFE_PSP_MemSet((void *) &configSocket, 0x0, sizeof(IO_TransUdpConfig_t));
    strncpy(configSocket.cAddr, CI_CUSTOM_UDP_ADDR, 16);
    configSocket.usPort = CI_CUSTOM_UDP_PORT;
    configSocket.timeoutRcv = CI_CUSTOM_UDP_TIMEOUT;
    
    if (IO_TransUdpInit(&configSocket, &g_CI_CustomData.pcSocket.udp) < 0)
    {
        goto end_of_function;
    }  
    
    /* Initialize select */ 
    if (IO_TransSelectClear(&g_CI_CustomData.select) < 0)
    {
        goto end_of_function;
    }
    
    /* Add to select set */
    if (IO_TransSelectAddFd(&g_CI_CustomData.select,
                            g_CI_CustomData.pcSocket.udp.sockId) < 0)
    {
        goto end_of_function;
    }

    /* Note: We are setting the Virtual Chnl ID to: Socket: 0 */

    /* Populate the ChannelService Table */
    TCTF_ChannelService_t   channelCfgTblUdp[CI_CUSTOM_TF_CHANNELS] = {
        {TCTF_SERVICE_VCP, 0, scId, 0, 0, 0, 0}
    };

    CFE_PSP_MemCpy((void *) &g_CI_CustomData.pcSocket.pc.mc.vChnls[0].chnlService,
                   (void *) &channelCfgTblUdp[0], sizeof(TCTF_ChannelService_t));
    
    /* Setup the CI Output Message (CLCW Message) */
    CFE_MSG_Init((CFE_MSG_Message_t *) &g_CI_CustomData.pcSocket.pc.mc.vChnls[0].clcwCmd.ucCmdHeader,
                   CFE_SB_ValueToMsgId(TO_APP_CMD_MID), sizeof(TO_CustomSetOcfCmd_t));
    CFE_MSG_SetFcnCode((CFE_MSG_Message_t *) &g_CI_CustomData.pcSocket.pc.mc.vChnls[0].clcwCmd,
                      TO_SET_OCF_DATA_CC);
    COP1_InitClcw(&g_CI_CustomData.pcSocket.pc.mc.vChnls[0].clcwCmd.clcw, 0);

    /* Initialize the managed parameters of physical channels */
    g_CI_CustomData.pcSocket.pc.cltuRand = CI_CUSTOM_CLTU_RANDOM_UDP;

    iStatus = CFE_ES_CreateChildTask(&taskId,
                                     "CI Custom Main Task",
                                     CI_CustomMain,
                                     CI_CUSTOM_TASK_STACK_PTR, 
                                     CI_CUSTOM_TASK_STACK_SIZE, 
                                     CI_CUSTOM_TASK_PRIO,
                                     0);
end_of_function:    
    return (iStatus);
}
    

/******************************************************************************/
/** \brief Custom app command response
*******************************************************************************/
int32 CI_CustomAppCmds(CFE_MSG_Message_t * pCmdMsg)
{
    int32 iStatus = CI_SUCCESS;
    CFE_MSG_FcnCode_t uiCmdCode = 0;
    CFE_MSG_GetFcnCode(pCmdMsg, &uiCmdCode);
    switch (uiCmdCode)
    {
        /*  Example of a valid custom command. Declare at top of file. 
        case CI_CUSTOM_EXAMPLE_CC:
            if (CI_VerifyCmdLength(pCmdMsg, sizeof(CI_CustomExampleCmd_t)))
            {
                CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdCnt);
                CFE_EVS_SendEvent(CI_CMD_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "CI: Recvd example custom app cmd (%d)", uiCmdCode);
            }
            break;
        */

        default:
            iStatus = CI_ERROR;
            break;
    }
    
    return iStatus;
}


/******************************************************************************/
/** \brief Custom response to CI_ENABLE_TO_CC cmd code
*******************************************************************************/
void CI_CustomEnableTO(CFE_MSG_Message_t * pCmdMsg)
{
    /* Copy the first part of the command (for socket setup) */
    CFE_PSP_MemCpy((void *) &g_CI_CustomData.toEnableCmd, 
                   (void *) pCmdMsg, sizeof(CI_EnableTOCmd_t));

    /* Setup the toEnableCmd */
    CFE_MSG_Init((CFE_MSG_Message_t *) &g_CI_CustomData.toEnableCmd.ucCmdHeader, 
                    CFE_SB_ValueToMsgId(TO_APP_CMD_MID), sizeof(TO_EnableOutputCmd_t)); 
    CFE_MSG_SetFcnCode((CFE_MSG_Message_t *) &g_CI_CustomData.toEnableCmd, 
                      TO_ENABLE_OUTPUT_CC);
    CFE_MSG_GenerateChecksum((CFE_MSG_Message_t *) &g_CI_CustomData.toEnableCmd);
    
    /* Send the TO Enable Telemetry Output Message */    
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *) &g_CI_CustomData.toEnableCmd, true);
    
    return;
}


/******************************************************************************/
/** \brief Custom Cleanup
*******************************************************************************/
void CI_CustomCleanup(void)
{
    IO_TransUdpCloseSocket(&g_CI_CustomData.pcSocket.udp);
    IO_TransSelectClear(&g_CI_CustomData.select);
}


/*******************************************************************************
** Custom Functions (Executed by Custom child task)
*******************************************************************************/

/******************************************************************************/
/** \brief Entry Point of custom child task
*******************************************************************************/
void CI_CustomMain(void)
{
    int32 size = 0;
    
    if (g_CI_CustomData.pcSocket.udp.sockId < 0)
    {
        CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_EventType_ERROR, 
                          "CI: Socket ID not set. Check init. "
                          "Quitting CI_CustomMain.");
        goto end_of_function;
    }
    
    //LOUIS
    #ifdef LOGGING_P2_ENABLED
    log_fp = fopen("/tmp/logids_p2.csv", "w");
    fprintf(log_fp, "simtime,realtime,msgId,FC,is_TC,AttackTag,IDS_output,packet\n");
    #endif
    #ifdef IDS_P2_ENABLED
    log_attack = fopen("/tmp/logattack_fw.txt", "w");
    #endif
    //log_fd = open("/tmp/logids_p2.txt", O_CREAT | O_WRONLY); // should add TRUNC to truncate file too
    //LOUIS TEST TID
    #ifdef CPU_BENCHMARK_TEST
    tid_file = fopen("/tmp/ci_custom_tid.txt", "w");
    printf("\033[96mid CI_custom : %d\033[0m\n",gettid());
    fprintf(tid_file,"%d\n",gettid());
    fflush(tid_file);
    #endif

    while(size >= 0)
    {
   // printf("**************CI udp_tf entree dans select \n"); TODO
        size = IO_TransSelectInput(&g_CI_CustomData.select, 
                                   IO_TRANS_PEND_FOREVER);

        if (size > 0)
        {
            if (IO_TransSelectFdInActive(&g_CI_CustomData.select,
                                         g_CI_CustomData.pcSocket.udp.sockId))
            {
                //printf("*************CI udp_tf commande recue \n"); TODO
                size = CI_CustomReadCltuSocket();
            }
            else 
            {
                CFE_EVS_SendEvent(CI_CUSTOM_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CI: Unexpected Active Device. "
                                  "Quitting CI_CustomMain.");
                break;
            }
        }
    }
    
end_of_function:  
    return;
}


/******************************************************************************/
/** \brief Custom Read CLTU From UDP (Private)
*******************************************************************************/
int32 CI_CustomReadCltuSocket(void)
{
    int32 iStatus = 0;
    int32 size = 0;
    CI_CustomPChnl_t *pPc = &g_CI_CustomData.pcSocket.pc;
    //CI_CustomMChnl_t *pMc = &pPc->mc;
    //TCTF_Hdr_t    *pTf = (TCTF_Hdr_t *) &pMc->tfBuff;

    //uint8_t* TC_ptr = &pPc->cltuBuff[0]; // LOUIS
    CFE_MSG_Message_t  *pSbMsg;

    /* CryptoLib Variables */
    TC_t crypto_tc_frame; 
    pSbMsg = (CFE_MSG_Message_t*) &crypto_tc_frame.tc_pdu;

    
    /* Read Full CLTU from socket */
    size = IO_TransUdpRcv(&g_CI_CustomData.pcSocket.udp, 
                          &pPc->cltuBuff[0], 
                          CI_CUSTOM_CLTU_BUFF_SIZE); 
    
    if(size <= 0) 
    {
    return size;
    }
    
    //printf("******** received CLTU ci_custom.c \n"); TODO
    /* 
    ** In this implementation it is assumed that:
    **   Recevied TC frame is complete without errors
    **   A single Space Packet is in the TC frame
    **   Code blocks are not in use
    **   COP-1 is not in use
    */

   #ifdef CI_CUSTOM_DEBUG  //TODO
    /* Debug prints */
        OS_printf("CI_CustomReadCltuSocket - pPc->cltuBuff[%d] = 0x", size);
        for (uint16 i = 0; i < size; i++) 
        {
            OS_printf("%02x", pPc->cltuBuff[i]);
        }
        OS_printf("\n");
   #endif

    /* CryptoLib */
    iStatus = Crypto_TC_ProcessSecurity((uint8_t*) &pPc->cltuBuff[0], &size, &crypto_tc_frame);
    if (iStatus == CRYPTO_LIB_SUCCESS)
    {
        #ifdef CI_CUSTOM_DEBUG
            /* Debug prints */
            OS_printf("CI_CustomReadCltuSocket - crypto_tc_frame.tc_pdu[%d] = 0x", crypto_tc_frame.tc_pdu_len);     
            
            for (uint16 i = 0; i < crypto_tc_frame.tc_pdu_len; i++)
            {
                OS_printf("%02x", crypto_tc_frame.tc_pdu[i]);
            }
            OS_printf("\n");
            
        #endif
        
        //LOUIS IRT CSS
	/*
	 * Probe 2 time get
	*/
	#if defined LOGGING_P2_ENABLED || defined IDS_P2_ENABLED
    OS_time_t currtime;
	CFE_PSP_GetTime(&currtime);
	#endif
    #if defined LOGGING_P2_ENABLED
    struct timespec realtime;
    timespec_get(&realtime, TIME_UTC);
    unsigned int packet_seconds = realtime.tv_sec - sim_start_time;
    #endif
        
    /* POUR OPTI ENCODAGE BINAIRE
    ssize_t test;
    test = write(log_fd, "0x", 2); 
    OS_printf(" ------ code retour write 1 -------- : %d\n", test);
    test = write(log_fd, crypto_tc_frame.tc_pdu, size);
    OS_printf(" ------ code retour write 2 -------- : %d\n", test);
    test = write(log_fd, "\n", 1);
    OS_printf(" ------ code retour write 3 -------- : %d\n", test);
    */
    
    /* IRT CSS - IDS
    ** ADD CODE FOR TC FIREWALL HERE, before sending to software bus (in IPS mode !!)
    */
	//Toggle / Untoggle p2 TC Analyzer
    #ifdef IDS_P2_ENABLED
    char is_allowed = 1; // Change to 0 and allow only when trusted later...
    int8_t rule_nb;
    is_allowed = is_command_allowed(&crypto_tc_frame, currtime, &rule_nb);
    if (is_allowed) 
    {
    #endif	
        /* Publish to software bus */
        CFE_SB_TransmitMsg(pSbMsg, true);
    #ifdef IDS_P2_ENABLED
    }
    else
    {
        /* Block the packet and report it*/
        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        fprintf(log_attack,"Packet blocked : ");
        printf("[P2] Packet blocked\n");
        printf("len tc pdu : %d\n",crypto_tc_frame.tc_pdu_len);
        for (uint16 i = 0; i < crypto_tc_frame.tc_pdu_len; i++)
        {
            fprintf(log_attack,"%02x", crypto_tc_frame.tc_pdu[i]);
        }
        fprintf(log_attack,"%s",asctime(timeinfo));
        fflush(log_attack);
        //#ifdef IDS_TELEM_ENABLED
        //IDS_RaiseAlert(4, rule_nb, crypto_tc_frame.tc_pdu_len, 0, crypto_tc_frame.tc_pdu, NULL);
        //#endif
    }
    #endif
    
    /* LOUIS IRT CSS
        * IDS Probe 2 logging
    */
	#ifdef LOGGING_P2_ENABLED
    uint8_t attack_tag = crypto_tc_frame.tc_pdu[7] & 2;
    uint16_t msgId = ((crypto_tc_frame.tc_pdu[0]<<8) | crypto_tc_frame.tc_pdu[1]);
    int16_t fc; //uint8_t field by default in nos3 but extended to int16_t to allow -1 for TM
    uint8_t is_tc;
    // if it is a TC extract the FC, else set it to 0
    if (msgId & 0x1000)
    {
        fc = crypto_tc_frame.tc_pdu[6];
        is_tc = 1;
    }
    else
    {
        fc = 0; //truncated to 0 for an easier implementation of is_command_allowed_bus, later changed to -1 before logging if the message is a TM for easier data understanding
        is_tc = 0;
    }
    #ifndef IDS_P2_ENABLED
    char is_allowed = P2_NOT_ACTIVE;
    #endif
    update_Log_P2(log_fp, currtime, &crypto_tc_frame, /*pPc->cltuBuff,*/ packet_seconds, &realtime, msgId, fc, is_tc, is_allowed, attack_tag);   
	#endif
    }
    else
    {
        OS_printf("CI_CustomReadCltuSocket - Crypto_TC_ProcessSecurity returned error %d \n", iStatus);
    }
    return size;
}

/*==============================================================================
** End of file ci_custom.c
**============================================================================*/
