#ifndef _GENERIC_CSS_CHECKOUT_DEVICE_CFG_H_
#define _GENERIC_CSS_CHECKOUT_DEVICE_CFG_H_

/*
** GENERIC_CSS Checkout Configuration
*/
#define GENERIC_CSS_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define GENERIC_CSS_CFG_STRING           "/dev/i2c_2"
#define GENERIC_CSS_CFG_HANDLE           2 
// Question: is this in straight Hz as its name implies? If so, i2c_bus_info_t's speed is uint16_t with a max value of 65k. 
// So I have to reduce from 1,000,000 (1 MHz) to 10,000 Hz (10 KHz).
#define GENERIC_CSS_CFG_BAUDRATE_HZ      10000 
#define GENERIC_CSS_CFG_MS_TIMEOUT       250
#define GENERIC_CSS_CFG_DEBUG

#endif /* _GENERIC_CSS_CHECKOUT_DEVICE_CFG_H_ */
