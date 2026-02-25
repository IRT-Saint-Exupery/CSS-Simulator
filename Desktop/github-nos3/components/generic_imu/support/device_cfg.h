#ifndef _GENERIC_IMU_CHECKOUT_DEVICE_CFG_H_
#define _GENERIC_IMU_CHECKOUT_DEVICE_CFG_H_

/*
** GENERIC_IMU Checkout Configuration
*/
#define GENERIC_IMU_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define GENERIC_IMU_CFG_STRING           "/dev/usart_29"
#define GENERIC_IMU_CFG_HANDLE           29 
#define GENERIC_IMU_CFG_BAUDRATE_HZ      115200
#define GENERIC_IMU_CFG_MS_TIMEOUT       250
#define GENERIC_IMU_CFG_DEBUG

#endif /* _GENERIC_IMU_CHECKOUT_DEVICE_CFG_H_ */
