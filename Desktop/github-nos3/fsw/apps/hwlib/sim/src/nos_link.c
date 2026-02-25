/* Copyright (C) 2009 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
any warranty that the software will be error free.

In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
documentation or services provided hereunder

ITC Team
NASA IV&V
ivv-itc@lists.nasa.gov
*/

#include "nos_link.h"
#include "hwlib.h"
#include <stdlib.h>

/* nos usart connection table */
nos_connection_t nos_usart_connection[NUM_USARTS] = {
    {"tcp://192.168.100.11:12000", "usart_0"},
    {"tcp://192.168.100.11:12000", "usart_1"},
    {"tcp://192.168.100.11:12000", "usart_2"},
    {"tcp://192.168.100.11:12000", "usart_3"},
    {"tcp://192.168.100.11:12000", "usart_4"},
    {"tcp://192.168.100.11:12000", "usart_5"},
    {"tcp://192.168.100.11:12000", "usart_6"},
    {"tcp://192.168.100.11:12000", "usart_7"},
    {"tcp://192.168.100.11:12000", "usart_8"},
    {"tcp://192.168.100.11:12000", "usart_9"},
    {"tcp://192.168.100.11:12000", "usart_10"},
    {"tcp://192.168.100.11:12000", "usart_11"},
    {"tcp://192.168.100.11:12000", "usart_12"},
    {"tcp://192.168.100.11:12000", "usart_13"},
    {"tcp://192.168.100.11:12000", "usart_14"},
    {"tcp://192.168.100.11:12000", "usart_15"},
    {"tcp://192.168.100.11:12000", "usart_16"},
    {"tcp://192.168.100.11:12000", "usart_17"},
    {"tcp://192.168.100.11:12000", "usart_18"},
    {"tcp://192.168.100.11:12000", "usart_19"},
    {"tcp://192.168.100.11:12000", "usart_20"},
    {"tcp://192.168.100.11:12000", "usart_21"},
    {"tcp://192.168.100.11:12000", "usart_22"},
    {"tcp://192.168.100.11:12000", "usart_23"},
    {"tcp://192.168.100.11:12000", "usart_24"},
    {"tcp://192.168.100.11:12000", "usart_25"},
    {"tcp://192.168.100.11:12000", "usart_26"},
    {"tcp://192.168.100.11:12000", "usart_27"},
    {"tcp://192.168.100.11:12000", "usart_28"},
    {"tcp://192.168.100.11:12000", "usart_29"}
};

/* nos i2c connection table */
nos_connection_t nos_i2c_connection[NUM_I2C_DEVICES] = {
    {"tcp://192.168.100.11:12000", "i2c_0"},
    {"tcp://192.168.100.11:12000", "i2c_1"},
    {"tcp://192.168.100.11:12000", "i2c_2"},
    {"tcp://192.168.100.11:12000", "i2c_3"},
    {"tcp://192.168.100.11:12000", "i2c_4"},
    {"tcp://192.168.100.11:12000", "i2c_5"},
    {"tcp://192.168.100.11:12000", "i2c_6"},
    {"tcp://192.168.100.11:12000", "i2c_7"},
    {"tcp://192.168.100.11:12000", "i2c_8"},
    {"tcp://192.168.100.11:12000", "i2c_9"},
    {"tcp://192.168.100.11:12000", "i2c_10"},
    {"tcp://192.168.100.11:12000", "i2c_11"},
    {"tcp://192.168.100.11:12000", "i2c_12"},
    {"tcp://192.168.100.11:12000", "i2c_13"},
    {"tcp://192.168.100.11:12000", "i2c_14"},
    {"tcp://192.168.100.11:12000", "i2c_15"},
    {"tcp://192.168.100.11:12000", "i2c_16"},
    {"tcp://192.168.100.11:12000", "i2c_17"},
    {"tcp://192.168.100.11:12000", "i2c_18"},
    {"tcp://192.168.100.11:12000", "i2c_19"},
    {"tcp://192.168.100.11:12000", "i2c_20"},
    {"tcp://192.168.100.11:12000", "i2c_21"},
    {"tcp://192.168.100.11:12000", "i2c_22"},
    {"tcp://192.168.100.11:12000", "i2c_23"},
    {"tcp://192.168.100.11:12000", "i2c_24"},
    {"tcp://192.168.100.11:12000", "i2c_25"},
    {"tcp://192.168.100.11:12000", "i2c_26"},
    {"tcp://192.168.100.11:12000", "i2c_27"},
    {"tcp://192.168.100.11:12000", "i2c_28"},
    {"tcp://192.168.100.11:12000", "i2c_29"}
};

/* nos can connection table */
nos_connection_t nos_can_connection[NUM_CAN_DEVICES] = {
    {"tcp://192.168.100.11:12000", "can_0"},
    {"tcp://192.168.100.11:12000", "can_1"},
    {"tcp://192.168.100.11:12000", "can_2"},
    {"tcp://192.168.100.11:12000", "can_3"},
    {"tcp://192.168.100.11:12000", "can_4"},
    {"tcp://192.168.100.11:12000", "can_5"},
    {"tcp://192.168.100.11:12000", "can_6"},
    {"tcp://192.168.100.11:12000", "can_7"},
    {"tcp://192.168.100.11:12000", "can_8"},
    {"tcp://192.168.100.11:12000", "can_9"},
    {"tcp://192.168.100.11:12000", "can_10"},
    {"tcp://192.168.100.11:12000", "can_11"},
    {"tcp://192.168.100.11:12000", "can_12"},
    {"tcp://192.168.100.11:12000", "can_13"},
    {"tcp://192.168.100.11:12000", "can_14"},
    {"tcp://192.168.100.11:12000", "can_15"},
    {"tcp://192.168.100.11:12000", "can_16"},
    {"tcp://192.168.100.11:12000", "can_17"},
    {"tcp://192.168.100.11:12000", "can_18"},
    {"tcp://192.168.100.11:12000", "can_19"},
    {"tcp://192.168.100.11:12000", "can_20"},
    {"tcp://192.168.100.11:12000", "can_21"},
    {"tcp://192.168.100.11:12000", "can_22"},
    {"tcp://192.168.100.11:12000", "can_23"},
    {"tcp://192.168.100.11:12000", "can_24"},
    {"tcp://192.168.100.11:12000", "can_25"},
    {"tcp://192.168.100.11:12000", "can_26"},
    {"tcp://192.168.100.11:12000", "can_27"},
    {"tcp://192.168.100.11:12000", "can_28"},
    {"tcp://192.168.100.11:12000", "can_29"}
};

/* nos spi connection table */
nos_connection_t nos_spi_connection[NUM_SPI_DEVICES] = {
    {"tcp://192.168.100.11:12000", "spi_0"},
    {"tcp://192.168.100.11:12000", "spi_1"},
    {"tcp://192.168.100.11:12000", "spi_2"},
    {"tcp://192.168.100.11:12000", "spi_3"},
    {"tcp://192.168.100.11:12000", "spi_4"},
    {"tcp://192.168.100.11:12000", "spi_5"},
    {"tcp://192.168.100.11:12000", "spi_6"},
    {"tcp://192.168.100.11:12000", "spi_7"},
    {"tcp://192.168.100.11:12000", "spi_8"},
    {"tcp://192.168.100.11:12000", "spi_9"},
    {"tcp://192.168.100.11:12000", "spi_10"},
    {"tcp://192.168.100.11:12000", "spi_11"},
    {"tcp://192.168.100.11:12000", "spi_12"},
    {"tcp://192.168.100.11:12000", "spi_13"},
    {"tcp://192.168.100.11:12000", "spi_14"},
    {"tcp://192.168.100.11:12000", "spi_15"},
    {"tcp://192.168.100.11:12000", "spi_16"},
    {"tcp://192.168.100.11:12000", "spi_17"},
    {"tcp://192.168.100.11:12000", "spi_18"},
    {"tcp://192.168.100.11:12000", "spi_19"},
    {"tcp://192.168.100.11:12000", "spi_20"},
    {"tcp://192.168.100.11:12000", "spi_21"},
    {"tcp://192.168.100.11:12000", "spi_22"},
    {"tcp://192.168.100.11:12000", "spi_23"},
    {"tcp://192.168.100.11:12000", "spi_24"},
    {"tcp://192.168.100.11:12000", "spi_25"},
    {"tcp://192.168.100.11:12000", "spi_26"},
    {"tcp://192.168.100.11:12000", "spi_27"},
    {"tcp://192.168.100.11:12000", "spi_28"},
    {"tcp://192.168.100.11:12000", "spi_29"}
};

/* common transport hub */
NE_TransportHub *hub = NULL;

/* internal hardware bus init/destroy */
extern void nos_destroy_usart_link(void);
extern void nos_init_i2c_link(void);
extern void nos_destroy_i2c_link(void);
extern void nos_init_can_link(void);
extern void nos_destroy_can_link(void);
extern void nos_init_spi_link(void);
extern void nos_destroy_spi_link(void);

/* initialize nos engine link */
void nos_init_link(void)
{
    OS_printf("initializing nos engine link...\n");

    /* create transport hub */
    hub = NE_create_transport_hub(0);

    /* initialize buses */
    /* can, i2c, and uart do not need init for mutex*/
    nos_init_spi_link();
}

/* destroy nos engine link */
void nos_destroy_link(void)
{
    OS_printf("destroying nos engine link...\n");

    /* destroy buses */
    nos_destroy_usart_link();
    nos_destroy_i2c_link();
    nos_destroy_can_link();
    nos_destroy_spi_link();

    /* destroy transport hub */
    NE_destroy_transport_hub(&hub);
}

