/*
**
** File: ut_cfe_sb_stubs.c
**
** Copyright 2017 United States Government as represented by the Administrator
** of the National Aeronautics and Space Administration.  No copyright is
** claimed in the United States under Title 17, U.S. Code.
** All Other Rights Reserved.
**
** $Id: ut_cfe_sb_stubs.c 1.1 2011/05/04 11:20:56EDT rmcgraw Exp  $
**
** Purpose: Unit test stubs for cFE Software Bus routines
**
** $Log: ut_cfe_sb_stubs.c  $
** Revision 1.1 2011/05/04 11:20:56EDT rmcgraw
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/ut-assert/src/project.pj
** Revision 1.1 2011/04/08 16:26:41EDT rmcgraw
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/ut-assert/src/project.pj
** Revision 1.2 2011/03/10 11:18:33EST sslegel
** Added hook support to CFE_SB_Subscribe, CFE_SB_SubscribeEx, and CFE_SB_Unsunscribe
** Revision 1.1 2011/02/15 11:13:03EST sslegel
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/src/project.pj
**
*/

#include "cfe.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_sb_hooks.h"

Ut_CFE_SB_HookTable_t           Ut_CFE_SB_HookTable;
Ut_CFE_SB_ReturnCodeTable_t     Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_MAX_INDEX];

void Ut_CFE_SB_Reset(void)
{
    memset(&Ut_CFE_SB_HookTable, 0, sizeof(Ut_CFE_SB_HookTable));
    memset(&Ut_CFE_SB_ReturnCodeTable, 0, sizeof(Ut_CFE_SB_ReturnCodeTable));

    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_CREATEPIPE_INDEX, &Ut_CFE_SB_CreatePipeHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SENDMSG_INDEX, &Ut_CFE_SB_TransmitMsgHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_RCVMSG_INDEX, &Ut_CFE_SB_ReceiveBufferHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &Ut_CFE_MSG_InitHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_MSGHDRSIZE_INDEX, &Ut_CFE_SB_MsgHdrSizeHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETUSERDATA_INDEX, &Ut_CFE_SB_GetUserDataHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETMSGID_INDEX, &Ut_CFE_SB_GetMsgIdHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SETMSGID_INDEX, &Ut_CFE_MSG_SetMsgIdHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETUSERDATALENGTH_INDEX, &Ut_CFE_SB_GetUserDataLengthHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SETUSERDATALENGTH_INDEX, &Ut_CFE_SB_SetUserDataLengthHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, &Ut_CFE_SB_GetTotalMsgLengthHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SETTOTALMSGLENGTH_INDEX, &Ut_CFE_SB_SetTotalMsgLengthHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETMSGTIME_INDEX, &Ut_CFE_SB_GetMsgTimeHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SETMSGTIME_INDEX, &Ut_CFE_MSG_SetMsgTimeHook);
//    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_TIMESTAMPMSG_INDEX, &Ut_CFE_SB_TimeStampMsgHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETCMDCODE_INDEX, &Ut_CFE_SB_GetCmdCodeHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_SETCMDCODE_INDEX, &Ut_CFE_SB_SetCmdCodeHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETCHECKSUM_INDEX, &Ut_CFE_SB_GetChecksumHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GENERATECHECKSUM_INDEX, &Ut_CFE_SB_GenerateChecksumHook);
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_VALIDATECHECKSUM_INDEX, &Ut_CFE_SB_ValidateChecksumHook);

    Ut_CFE_SB_ClearPipes();
    Ut_CFE_SB_ClearMsgQueue();
}

void Ut_CFE_SB_SetFunctionHook(uint32 Index, void *FunPtr)
{
    if      (Index == UT_CFE_SB_CREATEPIPE_INDEX)         { Ut_CFE_SB_HookTable.CFE_SB_CreatePipe = FunPtr; }
    else if (Index == UT_CFE_SB_DELETEPIPE_INDEX)         { Ut_CFE_SB_HookTable.CFE_SB_DeletePipe = FunPtr; }
    else if (Index == UT_CFE_SB_SUBSCRIBEEX_INDEX)        { Ut_CFE_SB_HookTable.CFE_SB_SubscribeEx = FunPtr; }
    else if (Index == UT_CFE_SB_SUBSCRIBE_INDEX)          { Ut_CFE_SB_HookTable.CFE_SB_Subscribe = FunPtr; }
    else if (Index == UT_CFE_SB_SUBSCRIBELOCAL_INDEX)     { Ut_CFE_SB_HookTable.CFE_SB_SubscribeLocal = FunPtr; }
    else if (Index == UT_CFE_SB_UNSUBSCRIBE_INDEX)        { Ut_CFE_SB_HookTable.CFE_SB_Unsubscribe = FunPtr; }
    else if (Index == UT_CFE_SB_UNSUBSCRIBELOCAL_INDEX)   { Ut_CFE_SB_HookTable.CFE_SB_UnsubscribeLocal = FunPtr; }
    else if (Index == UT_CFE_SB_SENDMSG_INDEX)            { Ut_CFE_SB_HookTable.CFE_SB_TransmitMsg = FunPtr; }
    else if (Index == UT_CFE_SB_PASSMSG_INDEX)            { Ut_CFE_SB_HookTable.CFE_SB_PassMsg = FunPtr; }
    else if (Index == UT_CFE_SB_RCVMSG_INDEX)             { Ut_CFE_SB_HookTable.CFE_SB_ReceiveBuffer = FunPtr; }
    else if (Index == UT_CFE_SB_GETLASTSENDERID_INDEX)    { Ut_CFE_SB_HookTable.CFE_SB_GetLastSenderId = FunPtr; }
    else if (Index == UT_CFE_SB_ZEROCOPYGETPTR_INDEX)     { Ut_CFE_SB_HookTable.CFE_SB_ZeroCopyGetPtr = FunPtr; }
    else if (Index == UT_CFE_SB_ZEROCOPYRELEASEPTR_INDEX) { Ut_CFE_SB_HookTable.CFE_SB_ZeroCopyReleasePtr = FunPtr; }
    else if (Index == UT_CFE_SB_ZEROCOPYSEND_INDEX)       { Ut_CFE_SB_HookTable.CFE_SB_ZeroCopySend = FunPtr; }
    else if (Index == UT_CFE_SB_ZEROCOPYPASS_INDEX)       { Ut_CFE_SB_HookTable.CFE_SB_ZeroCopyPass = FunPtr; }
    else if (Index == UT_CFE_SB_INITMSG_INDEX)            { Ut_CFE_SB_HookTable.CFE_MSG_Init = FunPtr; }
    else if (Index == UT_CFE_SB_MSGHDRSIZE_INDEX)         { Ut_CFE_SB_HookTable.CFE_SB_MsgHdrSize = FunPtr; }
    else if (Index == UT_CFE_SB_GETUSERDATA_INDEX)        { Ut_CFE_SB_HookTable.CFE_SB_GetUserData = FunPtr; }
    else if (Index == UT_CFE_SB_GETMSGID_INDEX)           { Ut_CFE_SB_HookTable.CFE_SB_GetMsgId = FunPtr; }
    else if (Index == UT_CFE_SB_SETMSGID_INDEX)           { Ut_CFE_SB_HookTable.CFE_MSG_SetMsgId = FunPtr; }
    else if (Index == UT_CFE_SB_GETUSERDATALENGTH_INDEX)  { Ut_CFE_SB_HookTable.CFE_SB_GetUserDataLength = FunPtr; }
    else if (Index == UT_CFE_SB_SETUSERDATALENGTH_INDEX)  { Ut_CFE_SB_HookTable.CFE_SB_SetUserDataLength = FunPtr; }
    else if (Index == UT_CFE_SB_GETTOTALMSGLENGTH_INDEX)  { Ut_CFE_SB_HookTable.CFE_SB_GetTotalMsgLength = FunPtr; }
    else if (Index == UT_CFE_SB_SETTOTALMSGLENGTH_INDEX)  { Ut_CFE_SB_HookTable.CFE_SB_SetTotalMsgLength = FunPtr; }
    else if (Index == UT_CFE_SB_GETMSGTIME_INDEX)         { Ut_CFE_SB_HookTable.CFE_SB_GetMsgTime = FunPtr; }
    else if (Index == UT_CFE_SB_SETMSGTIME_INDEX)         { Ut_CFE_SB_HookTable.CFE_MSG_SetMsgTime = FunPtr; }
    else if (Index == UT_CFE_SB_TIMESTAMPMSG_INDEX)       { Ut_CFE_SB_HookTable.CFE_SB_TimeStampMsg = FunPtr; }
    else if (Index == UT_CFE_SB_GETCMDCODE_INDEX)         { Ut_CFE_SB_HookTable.CFE_SB_GetCmdCode = FunPtr; }
    else if (Index == UT_CFE_SB_SETCMDCODE_INDEX)         { Ut_CFE_SB_HookTable.CFE_SB_SetCmdCode = FunPtr; }
    else if (Index == UT_CFE_SB_GETCHECKSUM_INDEX)        { Ut_CFE_SB_HookTable.CFE_SB_GetChecksum = FunPtr; }
    else if (Index == UT_CFE_SB_GENERATECHECKSUM_INDEX)   { Ut_CFE_SB_HookTable.CFE_SB_GenerateChecksum = FunPtr; }
    else if (Index == UT_CFE_SB_VALIDATECHECKSUM_INDEX)   { Ut_CFE_SB_HookTable.CFE_SB_ValidateChecksum = FunPtr; }
    else if (Index == UT_CFE_SB_CLEANUPAPP_INDEX)         { Ut_CFE_SB_HookTable.CFE_SB_CleanUpApp = FunPtr; }
    else                                                  { printf("Unsupported SB Index In SetFunctionHook Call %u\n", Index); }
}

void Ut_CFE_SB_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt)
{
    if (Index < UT_CFE_SB_MAX_INDEX) {
        Ut_CFE_SB_ReturnCodeTable[Index].Value = RtnVal;
        Ut_CFE_SB_ReturnCodeTable[Index].Count = CallCnt;
    }
    else {
        printf("Unsupported SB Index In SetReturnCode Call %u\n", Index);
    }
}

bool Ut_CFE_SB_UseReturnCode(uint32 Index)
{
    if (Ut_CFE_SB_ReturnCodeTable[Index].Count > 0) {
        Ut_CFE_SB_ReturnCodeTable[Index].Count--;
        if (Ut_CFE_SB_ReturnCodeTable[Index].Count == 0)
            return(true);
    }

    return(false);
}

int32 CFE_SB_CreatePipe (CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth, const char *PipeName)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_CREATEPIPE_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_CREATEPIPE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_CreatePipe)
        return Ut_CFE_SB_HookTable.CFE_SB_CreatePipe(PipeIdPtr, Depth, PipeName);

    return CFE_SUCCESS;
}

int32 CFE_SB_DeletePipe (CFE_SB_PipeId_t PipeId)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_DELETEPIPE_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_DELETEPIPE_INDEX].Value;

    return CFE_SUCCESS;
}

int32 CFE_SB_SubscribeEx (CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
                          CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_SUBSCRIBEEX_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_SUBSCRIBEEX_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_SubscribeEx)
        return Ut_CFE_SB_HookTable.CFE_SB_SubscribeEx(MsgId, PipeId, Quality, MsgLim);

    return CFE_SUCCESS;
}

int32 CFE_SB_Subscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_SUBSCRIBE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_Subscribe)
        return Ut_CFE_SB_HookTable.CFE_SB_Subscribe(MsgId, PipeId);

    return CFE_SUCCESS;
}

int32 CFE_SB_SubscribeLocal(CFE_SB_MsgId_t   MsgId, CFE_SB_PipeId_t  PipeId,uint16 MsgLim)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_SUBSCRIBELOCAL_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_SUBSCRIBELOCAL_INDEX].Value;

    return CFE_SUCCESS;
}

int32 CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_UNSUBSCRIBE_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_UNSUBSCRIBE_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_Unsubscribe)
        return Ut_CFE_SB_HookTable.CFE_SB_Unsubscribe(MsgId, PipeId);

    return CFE_SUCCESS;
}

int32 CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_UNSUBSCRIBELOCAL_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_UNSUBSCRIBELOCAL_INDEX].Value;

    return CFE_SUCCESS;
}

int32 CFE_SB_TransmitMsg (CFE_MSG_Message_t *MsgPtr)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_SENDMSG_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_SENDMSG_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_TransmitMsg)
        return Ut_CFE_SB_HookTable.CFE_SB_TransmitMsg(MsgPtr, true);

    return CFE_SUCCESS;
}

int32 CFE_SB_PassMsg (CFE_MSG_Message_t *MsgPtr)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_PASSMSG_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_PASSMSG_INDEX].Value;

    return CFE_SUCCESS;
}

int32 CFE_SB_ReceiveBuffer (CFE_MSG_Message_t * *BufPtr, CFE_SB_PipeId_t PipeId,
                    int32 TimeOut)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_RCVMSG_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_RCVMSG_INDEX].Value;

    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_ReceiveBuffer)
        return Ut_CFE_SB_HookTable.CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)BufPtr, PipeId, TimeOut);

    return CFE_SUCCESS;
}

uint32  CFE_SB_GetLastSenderId(CFE_SB_SenderId_t **Ptr,CFE_SB_PipeId_t  PipeId)
{
    return CFE_SUCCESS;
}

CFE_MSG_Message_t  *CFE_SB_ZeroCopyGetPtr(uint16  MsgSize,CFE_SB_ZeroCopyHandle_t *BufferHandle)
{
    return NULL;
}

int32 CFE_SB_ZeroCopyReleasePtr(CFE_MSG_Message_t  *Ptr2Release,CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    return CFE_SUCCESS;
}

int32 CFE_SB_ZeroCopySend(CFE_MSG_Message_t   *MsgPtr, CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    return CFE_SUCCESS;
}

int32 CFE_SB_ZeroCopyPass(CFE_MSG_Message_t   *MsgPtr, CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    return CFE_SUCCESS;
}

void CFE_MSG_Init (void *MsgPtr, CFE_SB_MsgId_t MsgId, uint16 Length, bool Clear)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_MSG_Init)
        Ut_CFE_SB_HookTable.CFE_MSG_Init(MsgPtr, MsgId, Length);

    return;
}

uint16 CFE_SB_MsgHdrSize (CFE_SB_MsgId_t MsgId)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_MsgHdrSize)
        return Ut_CFE_SB_HookTable.CFE_SB_MsgHdrSize(MsgId);

    return CFE_SUCCESS;
}

void *CFE_SB_GetUserData(CFE_MSG_Message_t * MsgPtr)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_GetUserData)
        return Ut_CFE_SB_HookTable.CFE_SB_GetUserData(MsgPtr);

    return  NULL;
}

CFE_SB_MsgId_t CFE_SB_GetMsgId (CFE_MSG_Message_t * MsgPtr)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_GetMsgId)
        return Ut_CFE_SB_HookTable.CFE_MSG_GetMsgId(MsgPtr, CFE_SB_MsgId_t *MsgId);

    return CFE_SUCCESS;
}

void  CFE_MSG_SetMsgId (CFE_MSG_Message_t * MsgPtr, CFE_SB_MsgId_t MsgId)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_MSG_SetMsgId)
        Ut_CFE_SB_HookTable.CFE_MSG_SetMsgId(MsgPtr,MsgId);

    return;
}

uint16 CFE_SB_GetUserDataLength(CFE_MSG_Message_t * MsgPtr)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_GetUserDataLength)
        return Ut_CFE_SB_HookTable.CFE_SB_GetUserDataLength(MsgPtr);

    return CFE_SUCCESS;
}/* end CFE_SB_GetUserDataLength */

void CFE_SB_SetUserDataLength(CFE_MSG_Message_t * MsgPtr,uint16 DataLength)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_SetUserDataLength)
        Ut_CFE_SB_HookTable.CFE_SB_SetUserDataLength(MsgPtr,DataLength);

    return;
}

uint16 CFE_MSG_GetSize(CFE_MSG_Message_t * MsgPtr, CFE_MSG_Size_t *Size)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_GetTotalMsgLength)
        return Ut_CFE_SB_HookTable.CFE_MSG_GetSize(MsgPtr, CFE_MSG_Size_t *Size);

    return CFE_SUCCESS;
}

void CFE_MSG_SetSize(CFE_MSG_Message_t * MsgPtr, uint16 TotalLength)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_SetTotalMsgLength)
        Ut_CFE_SB_HookTable.CFE_MSG_SetSize(MsgPtr, TotalLength);

    return;
}

CFE_TIME_SysTime_t CFE_SB_GetMsgTime (CFE_MSG_Message_t * MsgPtr)
{
    CFE_TIME_SysTime_t  Time;

    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_GetMsgTime)
        return Ut_CFE_SB_HookTable.CFE_SB_GetMsgTime(MsgPtr);

    Time.Seconds    =  0;
    Time.Subseconds =  0;

    return Time;
}

int32 CFE_MSG_SetMsgTime (CFE_MSG_Message_t * MsgPtr, CFE_TIME_SysTime_t time)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_MSG_SetMsgTime)
        return Ut_CFE_SB_HookTable.CFE_MSG_SetMsgTime(MsgPtr, time);

    return CFE_SUCCESS;
}

void CFE_SB_TimeStampMsg (CFE_MSG_Message_t * MsgPtr)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_TimeStampMsg)
        Ut_CFE_SB_HookTable.CFE_SB_TimeStampMsg(MsgPtr);

    return;
}

uint16 CFE_SB_GetCmdCode (CFE_MSG_Message_t * MsgPtr)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_GetCmdCode)
        return Ut_CFE_SB_HookTable.CFE_MSG_GetFcnCode(MsgPtr, CFE_MSG_FcnCode_t *FcnCode);

    return CFE_SUCCESS;
}

int32 CFE_SB_SetCmdCode (CFE_MSG_Message_t * MsgPtr, uint16 CmdCode)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_SetCmdCode)
        return Ut_CFE_SB_HookTable.CFE_MSG_SetFcnCode(MsgPtr, CmdCode);

    return CFE_SUCCESS;
}

uint16 CFE_SB_GetChecksum(CFE_MSG_Message_t * MsgPtr)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_GetChecksum)
        return Ut_CFE_SB_HookTable.CFE_SB_GetChecksum(MsgPtr);

    return CFE_SUCCESS;
}

void CFE_SB_GenerateChecksum(CFE_MSG_Message_t * MsgPtr)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_GenerateChecksum)
        Ut_CFE_SB_HookTable.CFE_SB_GenerateChecksum(MsgPtr);

    return;
}

bool CFE_SB_ValidateChecksum(CFE_MSG_Message_t * MsgPtr)
{
    /* Check for Function Hook */
    if (Ut_CFE_SB_HookTable.CFE_SB_ValidateChecksum)
        return Ut_CFE_SB_HookTable.CFE_SB_ValidateChecksum(MsgPtr);

    return CFE_SUCCESS;
}

int32 CFE_SB_CleanUpApp (uint32 AppId)
{
    /* Check for specified return */
    if (Ut_CFE_SB_UseReturnCode(UT_CFE_SB_CLEANUPAPP_INDEX))
        return Ut_CFE_SB_ReturnCodeTable[UT_CFE_SB_CLEANUPAPP_INDEX].Value;

    return CFE_SUCCESS;
}
