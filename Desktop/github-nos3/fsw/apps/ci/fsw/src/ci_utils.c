/******************************************************************************/
/** \file  ci_utils.c
*
*   Copyright 2017 United States Government as represented by the Administrator
*   of the National Aeronautics and Space Administration.  No copyright is
*   claimed in the United States under Title 17, U.S. Code.
*   All Other Rights Reserved.
*  
*   \author Guy de Carufel (Odyssey Space Research), NASA, JSC, ER6
*
*   \brief Function Definitions of utility functions
*
*   \par
*       This file defines utility functions used by other CI functions.
*
*   \par API Functions Defined:
*     - CI_IncrHkCounter() - Increment a HK Counter with memory protection
*     - CI_VerifyCmdLength() - Verify length of command message
*
*   \par Private Functions Defined:
*
*   \par Limitations, Assumptions, External Events, and Notes:
*
*   \par Modification History:
*     - 2015-01-09 | Guy de Carufel | Code Started
*******************************************************************************/

/*
** Include Files
*/
#include "ci_app.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/


/*
** External Global Variables
*/
extern CI_AppData_t g_CI_AppData;

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Definitions
*/


/******************************************************************************/
/** \brief Increment a housekeeping packet counter
*******************************************************************************/
void CI_IncrHkCounter(uint16 * counter)
{
    OS_MutSemTake(g_CI_AppData.ciMutex);
    *counter = *counter + 1;
    OS_MutSemGive(g_CI_AppData.ciMutex);
}


/******************************************************************************/
/** \brief Verify the command length against expected length
*******************************************************************************/
bool CI_VerifyCmdLength(CFE_MSG_Message_t * pMsg,
                           uint16 usExpectedLen)
{
    bool bResult=false;
    size_t usMsgLen=0;
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t usCmdCode = 0;

    if (pMsg != NULL)
    {
        CFE_MSG_GetSize(pMsg, &usMsgLen);

        if (usExpectedLen == usMsgLen)
        {
            bResult = true;
        }
        else
        {
            CFE_MSG_GetMsgId(pMsg, &MsgId);
            CFE_MSG_GetFcnCode(pMsg, &usCmdCode);

            CFE_EVS_SendEvent(CI_MSGLEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CI: Rcvd invalid msgLen: msgId=0x%04X, "
                              "cmdCode=%d, msgLen=%d, expectedLen=%d",
                              CFE_SB_MsgIdToValue(MsgId), usCmdCode, usMsgLen, usExpectedLen);
                              
            CI_IncrHkCounter(&g_CI_AppData.HkTlm.usCmdErrCnt);
        }
    }

    return (bResult);
}

/*==============================================================================
** End of file ci_utils.c
**============================================================================*/
