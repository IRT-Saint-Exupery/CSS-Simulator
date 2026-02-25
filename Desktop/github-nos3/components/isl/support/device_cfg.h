#ifndef _ISL_CHECKOUT_DEVICE_CFG_H_
#define _ISL_CHECKOUT_DEVICE_CFG_H_

/*
** ISL Checkout Configuration
*/
#define ISL_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define ISL_CFG_STRING           "/dev/usart_28"
#define ISL_CFG_HANDLE           28 
#define ISL_CFG_BAUDRATE_HZ      115200
#define ISL_CFG_MS_TIMEOUT       250
#define ISL_CFG_DEBUG

/* Note: Debug flag disabled (commented out) by default */
//#define ISL_CFG_DEBUG

#define ISL_SPP_MAX_ATA_SIZE   	10000
#define ISL_CFG_FSW_IP           "0.0.0.0"

/* Listen local ports */
#define ISL_CFG_FEEDER_RCV_PORT  	5012
#define ISL_CFG_ISL_TC_RCV_PORT		7373
#define ISL_CFG_ISL_TM_RCV_PORT		7374
#define ISL_CFG_FSW_RCV_PORT 		5013

/* Send local ports */
#define ISL_CFG_FEEDER_SND_PORT  	6012
#define ISL_CFG_ISL_TC_SND_PORT		8373
#define ISL_CFG_ISL_TM_SND_PORT		8374
#define ISL_CFG_FSW_SND_PORT 		6013

/* Distant ports */
#define	ISL_CFG_FEEDER_DST_PORT		5013
#define	ISL_CFG_ISL_TC_DST_PORT		ISL_CFG_ISL_TC_RCV_PORT
#define	ISL_CFG_ISL_TM_DST_PORT		ISL_CFG_ISL_TM_RCV_PORT
#define ISL_CFG_FSW_DST_PORT 		5014

#endif /* _ISL_CHECKOUT_DEVICE_CFG_H_ */
