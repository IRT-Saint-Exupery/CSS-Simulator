/************************************************************************
** File:
**   $Id: ids_platform_cfg.h  $
**
** Purpose:
**  Define ids Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#include "ids_probes.h"

#ifndef _IDS_PLATFORM_CFG_H_
#define _IDS_PLATFORM_CFG_H_

/*
** Default IDS Configuration
*/
#ifndef IDS_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define IDS_CFG_STRING           "usart_27"
    #define IDS_CFG_HANDLE           27
    #define IDS_CFG_BAUDRATE_HZ      115200
    #define IDS_CFG_MS_TIMEOUT       50            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define IDS_CFG_DEBUG
    #define IDS_PDATA_SIZE            175
    #define IDS_TSKNAME_SIZE          20

    #ifdef IDS_TELEM_ENABLED
    //IDS Child Task
    #define IDS_CHILD_TASK_NAME                     "IDS_CHILD_TASK"
    #define IDS_CHILD_TASK_STACK_SIZE               2048
    #define IDS_CHILD_TASK_PRIORITY                 70
    #endif
#endif

#endif /* _IDS_PLATFORM_CFG_H_ */
