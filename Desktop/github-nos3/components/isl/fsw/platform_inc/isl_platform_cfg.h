/************************************************************************
** File:
**   $Id: isl_platform_cfg.h  $
**
** Purpose:
**  Define isl Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _ISL_PLATFORM_CFG_H_
#define _ISL_PLATFORM_CFG_H_

/*
** ISL Task Definitions
*/
#define ISL_DEVICE_NAME              "ISL_ISL"
#define ISL_DEVICE_STACK_SIZE        2048
#define ISL_DEVICE_PRIORITY          80
#define ISL_DEVICE_MS_LOOP_DELAY     100

#define WATCHDOG_TIMEOUT             10   

/*
** Default ISL Configuration
*/
#undef ISL_CFG  // TODO A VIRER !! NORMALEMENT support/device_cfg.h définit ISL_CFG et la suite.
				// Or j'ai bien ISL_CFG positionné puisque je ne passe pas dans le bloc ci-dessous
				// mais je n'ai pas les PORTS positionnés alors qu'ils sont bien définis dans support/device_cfg.h
#ifndef ISL_CFG
        #define ISL_CUSTOM_CLTU_BUFF_SIZE 2000 // > iCaduSize in to_custom.c
        /* Notes: 
        **   NOS3 uart requires matching handle and bus number
        */
        #define ISL_CFG_STRING           "usart_28"
        #define ISL_CFG_HANDLE           28 
        #define ISL_CFG_BAUDRATE_HZ      115200
        #define ISL_CFG_MS_TIMEOUT       50            /* Max 255 */
       /* Note: Debug flag disabled (commented out) by default */
       //#define ISL_CFG_DEBUG

	#define ISL_CFG_FSW_IP           "0.0.0.0"

	/* Listen local ports */
        #define ISL_CFG_TC_RCV_PORT  	5012
        #define ISL_CFG_TM_RCV_PORT	5011

	/* Send local ports */
        #define ISL_CFG_UDP_SND_PORT  	0

	/* Distant ports */
	#define	ISL_CFG_ISL_TC_DST_PORT		ISL_CFG_TC_RCV_PORT
	#define	ISL_CFG_FEEDER_DST_PORT		ISL_CFG_TM_RCV_PORT
	#define	ISL_CFG_ISL_TM_DST_PORT		ISL_CFG_TM_RCV_PORT
	#define ISL_CFG_FSW_DST_PORT 		5010

	#define SAT_IP_ADR_FMT "192.168.100.%d"

	#define NB_SAT_MAX 7
	extern unsigned int NB_SATELLITES;
	/* #define ISL_rootDataDir 	"/home/nos3/Desktop/github-nos3/sims/cfg"*/
	#define ISL_rootDataDir 	"/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf"
	#define ISL_routeConfigFile     "routeConfig.txt"        
	#define GROUND_ID	0

#endif

#endif /* _ISL_PLATFORM_CFG_H_ */
