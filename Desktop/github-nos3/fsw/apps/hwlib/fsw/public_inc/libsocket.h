#ifndef _lib_socket_h_
#define _lib_socket_h_

#include "hwlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

/* Defines */
#define SOCKET_SUCCESS             OS_SUCCESS
#define SOCKET_ERROR               OS_ERROR
#define SOCKET_CREATE_ERR          -2
#define SOCKET_BIND_ERR            -3
#define SOCKET_LISTEN_ERR          -4 
#define SOCKET_ACCEPT_ERR          -5 
#define SOCKET_CONNECT_ERR         -6
#define SOCKET_RECV_ERR            -7
#define SOCKET_SEND_ERR            -8 
#define SOCKET_CLOSE_ERR           -9
#define SOCKET_TRY_AGAIN           -10

/* Types */
typedef enum {
    ip_ver_4,
    ip_ver_6
} addr_fam_e;

typedef enum {
    stream,
    dgram
} type_e;

typedef enum {
    server,
    client
} category_e;

typedef struct {
    int        sockfd;            // Socket descriptor 
    int        port_num;          // Function dependent
    char*      ip_address;        // Function dependent  
    addr_fam_e address_family;    // IP version 4 or 6
    type_e     type;              // Stream or Data Gram 
    category_e category;          // Server or Client 
    bool       block;             // 1 = Block, 0 = Non-block
    bool       keep_alive;        // 1 = Keep Alive On, 0 = Keep Alive Off
    bool       created;           // Has a file desciptor been assigned? 
    bool       bound;             // Is the socket bound to an ip address and port num?
    bool       listening;         // Is the socket being listened on?
    bool       connected;         // Is the stream socket in a connected state?
} socket_info_t;

/* Function Prototypes */
void socket_dummy(void);
int32_t socket_create(socket_info_t* socket_info);
int32_t socket_listen(socket_info_t* socket_info);
int32_t socket_accept(socket_info_t* socket_info);
int32_t socket_connect(socket_info_t* socket_info, char* remote_ip_address, int remote_port_num);
int32_t socket_send(socket_info_t* socket_info, uint8_t* buffer, size_t buflen, size_t* bytes_sent, char* remote_ip_address, int remote_port_num);
int32_t socket_recv(socket_info_t* socket_info, uint8_t* buffer, size_t buflen, size_t* bytes_recvd);
int32_t socket_close(socket_info_t* socket_info);

#endif
