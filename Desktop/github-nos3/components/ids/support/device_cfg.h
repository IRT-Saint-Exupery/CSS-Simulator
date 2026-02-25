#ifndef _IDS_CHECKOUT_DEVICE_CFG_H_
#define _IDS_CHECKOUT_DEVICE_CFG_H_

/*
** IDS Checkout Configuration
*/
#define IDS_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define IDS_CFG_STRING           "/dev/usart_29"
#define IDS_CFG_HANDLE           29 
#define IDS_CFG_BAUDRATE_HZ      115200
#define IDS_CFG_MS_TIMEOUT       250
#define IDS_CFG_DEBUG

#endif /* _IDS_CHECKOUT_DEVICE_CFG_H_ */
