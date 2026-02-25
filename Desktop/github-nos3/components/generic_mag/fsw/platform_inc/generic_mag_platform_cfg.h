/************************************************************************
** File:
**   $Id: generic_mag_platform_cfg.h  $
**
** Purpose:
**  Define generic_mag Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_MAG_PLATFORM_CFG_H_
#define _GENERIC_MAG_PLATFORM_CFG_H_

/*
** Default GENERIC_MAG Configuration
*/
#ifndef GENERIC_MAG_CFG
    /* Notes: 
    **   NOS3 spi requires matching handle and bus number
    */
    #define GENERIC_MAG_CFG_STRING           "spi_2"
    #define GENERIC_MAG_CFG_HANDLE           2
    #define GENERIC_MAG_CFG_DELAY            32 //32 IRT CSS       /* SPI transaction delay, in microseconds */
    #define GENERIC_MAG_CFG_BAUD             1000000
    #define GENERIC_MAG_CFG_SPI_MODE         1
    #define GENERIC_MAG_CFG_BITS_PER_WORD    8
    #define GENERIC_MAG_CFG_BUS              0
    #define GENERIC_MAG_CFG_CS               2
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_MAG_CFG_DEBUG            1
#endif

#endif /* _GENERIC_MAG_PLATFORM_CFG_H_ */
