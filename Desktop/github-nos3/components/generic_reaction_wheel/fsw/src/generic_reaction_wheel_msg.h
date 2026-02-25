/*******************************************************************************
** File:
**  generic_reaction_wheel_app_msg.h
**
** Purpose:
**  Define Generic Reaction Wheel App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _generic_reaction_wheel_app_msg_h_
#define _generic_reaction_wheel_app_msg_h_

/*
** Generic Reaction Wheel App command codes
*/
#define GENERIC_RW_APP_NOOP_CC                 0
#define GENERIC_RW_APP_RESET_COUNTERS_CC       1
#define GENERIC_RW_APP_REQ_DATA_CC             2
#define GENERIC_RW_APP_SET_TORQUE_CC           3

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;

} GENERIC_RW_NoArgsCmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8  wheel_number;
    int16  data;

} __attribute__((packed)) GENERIC_RW_Cmd_t;
#define GENERIC_RW_CMD_LEN sizeof(GENERIC_RW_Cmd_t)

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which_open_mode
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef GENERIC_RW_NoArgsCmd_t      GENERIC_RW_Noop_t;
typedef GENERIC_RW_NoArgsCmd_t      GENERIC_RW_ResetCounters_t;

/*************************************************************************/
/*
** Type definition (Generic Reaction Wheel App housekeeping)
*/

typedef struct
{
    double momentum[3];
} GENERIC_RW_Data;


typedef struct
{
    uint8              CommandErrorCounter;
    uint8              CommandCounter;
    GENERIC_RW_Data    data;
} __attribute__((packed)) GENERIC_RW_HkTlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    GENERIC_RW_HkTlm_Payload_t  Payload;

} __attribute__((packed)) GENERIC_RW_HkTlm_t;

#endif /* _generic_reaction_wheel_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
