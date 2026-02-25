/************************************************************************
** File:
**   generic_fss_platform_cfg.h
**
** Purpose:
**  Define generic_fss Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _GENERIC_FSS_PLATFORM_CFG_H_
#define _GENERIC_FSS_PLATFORM_CFG_H_

/*
** Default GENERIC_FSS Configuration
*/
#ifndef GENERIC_FSS_CFG
    /* Notes: 
    **   NOS3 spi requires matching handle and bus number
    */
    #define GENERIC_FSS_CFG_STRING           "spi_1"
    #define GENERIC_FSS_CFG_HANDLE           1
    #define GENERIC_FSS_CFG_DELAY            32  //32 IRT CSS      /* SPI transaction delay, in microseconds */
    #define GENERIC_FSS_CFG_BAUD             1000000
    #define GENERIC_FSS_CFG_SPI_MODE         1
    #define GENERIC_FSS_CFG_BITS_PER_WORD    8
    #define GENERIC_FSS_CFG_BUS              0
    #define GENERIC_FSS_CFG_CS               1
    /* Note: Debug flag disabled (commented out) by default */
    //#define GENERIC_FSS_CFG_DEBUG            
#endif

#endif /* _GENERIC_FSS_PLATFORM_CFG_H_ */
