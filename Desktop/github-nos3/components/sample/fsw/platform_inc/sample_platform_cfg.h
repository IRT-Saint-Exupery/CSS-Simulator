/************************************************************************
** File:
**   $Id: sample_platform_cfg.h  $
**
** Purpose:
**  Define sample Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _SAMPLE_PLATFORM_CFG_H_
#define _SAMPLE_PLATFORM_CFG_H_

/*
** Default SAMPLE Configuration
*/
#ifndef SAMPLE_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define SAMPLE_CFG_STRING           "usart_29"
    #define SAMPLE_CFG_HANDLE           29 
    #define SAMPLE_CFG_BAUDRATE_HZ      115200
    #define SAMPLE_CFG_MS_TIMEOUT       50            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define SAMPLE_CFG_DEBUG
#endif

#endif /* _SAMPLE_PLATFORM_CFG_H_ */
