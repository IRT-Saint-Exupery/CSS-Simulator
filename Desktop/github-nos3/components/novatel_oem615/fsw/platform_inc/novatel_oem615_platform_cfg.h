/************************************************************************
** File:
**   $Id: novatel_oem615_platform_cfg.h  $
**
** Purpose:
**  Define novatel_oem615 Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _NOVATEL_OEM615_PLATFORM_CFG_H_
#define _NOVATEL_OEM615_PLATFORM_CFG_H_

/*
** Default NOVATEL_OEM615 Configuration
*/
#ifndef NOVATEL_OEM615_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define NOVATEL_OEM615_CFG_STRING           "usart_1"
    #define NOVATEL_OEM615_CFG_HANDLE           1
    #define NOVATEL_OEM615_CFG_BAUDRATE_HZ      115200
    #define NOVATEL_OEM615_CFG_MS_TIMEOUT       50    /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define NOVATEL_OEM615_CFG_DEBUG //IRT CSS - DO NOT USE, WARNINGS RELATED TO CHILD GPS DATA
#endif

#endif /* _NOVATEL_OEM615_PLATFORM_CFG_H_ */
