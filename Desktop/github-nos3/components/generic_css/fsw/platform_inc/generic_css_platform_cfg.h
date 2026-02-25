/************************************************************************
** File:
**   $Id: generic_css_platform_cfg.h  $
**
** Purpose:
**  Define generic_css Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_CSS_PLATFORM_CFG_H_
#define _GENERIC_CSS_PLATFORM_CFG_H_

/*
** Default GENERIC_CSS Configuration
*/
#ifndef GENERIC_CSS_CFG
    /* Notes: 
    **   NOS3 uart requires matching handle and bus number
    */
    #define GENERIC_CSS_CFG_STRING           "i2c_2"
    #define GENERIC_CSS_CFG_HANDLE           2 
    #define GENERIC_CSS_CFG_BAUDRATE_HZ      1000
    #define GENERIC_CSS_CFG_MS_TIMEOUT       250            /* Max 255 */
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_CSS_CFG_DEBUG
#endif

#endif /* _GENERIC_CSS_PLATFORM_CFG_H_ */
