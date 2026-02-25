#ifndef _GENERIC_RADIO_CHECKOUT_DEVICE_CFG_H_
#define _GENERIC_RADIO_CHECKOUT_DEVICE_CFG_H_

/*
** GENERIC_RADIO Checkout Configuration
*/
#define GENERIC_RADIO_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define GENERIC_RADIO_CFG_STRING           "/dev/usart_29"
#define GENERIC_RADIO_CFG_HANDLE           29 
#define GENERIC_RADIO_CFG_BAUDRATE_HZ      115200
#define GENERIC_RADIO_CFG_MS_TIMEOUT       250
#define GENERIC_RADIO_CFG_DEBUG

#endif /* _GENERIC_RADIO_CHECKOUT_DEVICE_CFG_H_ */
