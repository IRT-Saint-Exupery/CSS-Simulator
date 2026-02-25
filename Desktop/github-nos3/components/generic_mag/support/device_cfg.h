#ifndef _GENERIC_MAG_CHECKOUT_DEVICE_CFG_H_
#define _GENERIC_MAG_CHECKOUT_DEVICE_CFG_H_

/*
** GENERIC_MAG Checkout Configuration
*/
#define GENERIC_MAG_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define GENERIC_MAG_CFG_STRING           "/dev/spi_1"
#define GENERIC_MAG_CFG_HANDLE           2 
#define GENERIC_MAG_CFG_BAUDRATE_HZ      1000000
#define GENERIC_MAG_CFG_MS_TIMEOUT       250
#define GENERIC_MAG_CFG_DEBUG            1

#endif /* _GENERIC_MAG_CHECKOUT_DEVICE_CFG_H_ */
