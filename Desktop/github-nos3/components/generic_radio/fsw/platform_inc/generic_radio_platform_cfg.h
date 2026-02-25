/************************************************************************
** File:
**   $Id: generic_radio_platform_cfg.h  $
**
** Purpose:
**  Define generic_radio Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_RADIO_PLATFORM_CFG_H_
#define _GENERIC_RADIO_PLATFORM_CFG_H_

/*
** GENERIC_RADIO Task Definitions
*/
#define GENERIC_RADIO_DEVICE_NAME              "GENERIC_RADIO"
#define GENERIC_RADIO_DEVICE_STACK_SIZE        2048
#define GENERIC_RADIO_DEVICE_PRIORITY          80
#define GENERIC_RADIO_DEVICE_MS_LOOP_DELAY     100

/*
** Default GENERIC_RADIO Configuration
*/
#ifndef GENERIC_RADIO_CFG
    #define GENERIC_RADIO_CFG_PROX_DATA_SIZE   64
    #define GENERIC_RADIO_CFG_FSW_IP           "0.0.0.0"
    #define GENERIC_RADIO_CFG_DEVICE_IP        "0.0.0.0"
    #define GENERIC_RADIO_CFG_DEVICE_DELAY_MS  10
    #define GENERIC_RADIO_CFG_UDP_PROX_TO_FSW  7010
    #define GENERIC_RADIO_CFG_UDP_FSW_TO_PROX  7011
    #define GENERIC_RADIO_CFG_UDP_FSW_TO_RADIO 5014
    #define GENERIC_RADIO_CFG_UDP_RADIO_TO_FSW 5015
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_RADIO_CFG_DEBUG
#endif

#endif /* _GENERIC_RADIO_PLATFORM_CFG_H_ */
