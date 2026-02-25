/************************************************************************
** File:
**   $Id: generic_eps_platform_cfg.h  $
**
** Purpose:
**  Define generic_eps Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_EPS_PLATFORM_CFG_H_
#define _GENERIC_EPS_PLATFORM_CFG_H_

/*
** Default GENERIC_EPS Configuration
*/
#ifndef GENERIC_EPS_CFG
    #define GENERIC_EPS_CFG
    #define GENERIC_EPS_CFG_I2C_HANDLE       1
    #define GENERIC_EPS_CFG_I2C_SPEED        1000
    #define GENERIC_EPS_CFG_I2C_ADDRESS      0x2B // 7-bit address
    #define GENERIC_EPS_CFG_I2C_TIMEOUT      10
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_EPS_CFG_DEBUG
#endif

#endif /* _GENERIC_EPS_PLATFORM_CFG_H_ */
