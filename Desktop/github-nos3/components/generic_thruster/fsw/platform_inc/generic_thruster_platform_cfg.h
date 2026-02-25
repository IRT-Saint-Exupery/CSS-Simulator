/************************************************************************
** File:
**   $Id: generic_thruster_platform_cfg.h  $
**
** Purpose:
**  Define generic_thruster Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_THRUSTER_PLATFORM_CFG_H_
#define _GENERIC_THRUSTER_PLATFORM_CFG_H_

/*
** Default GENERIC_THRUSTER Configuration
*/
#ifndef GENERIC_THRUSTER_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define GENERIC_THRUSTER_CFG_STRING           "usart_29"
    #define GENERIC_THRUSTER_CFG_HANDLE           29
    #define GENERIC_THRUSTER_CFG_BAUDRATE_HZ      115200
    #define GENERIC_THRUSTER_CFG_MS_TIMEOUT       50            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_THRUSTER_CFG_DEBUG
#endif

#endif /* _GENERIC_THRUSTER_PLATFORM_CFG_H_ */
