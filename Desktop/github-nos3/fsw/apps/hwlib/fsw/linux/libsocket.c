#include "libsocket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

// Creates an endpoint for communication
// Binds stream, server sockets to localhost and port number
//
// Inputs:
//      socket_info->address_family
//      socket_info->type
//      socket_info->port_num (used for stream sockets)
//      socket_info->block
//
// Outputs:
//      socket_info->sockfd
//      socket_info->created
//      socket_info->bound  
int32_t socket_create(socket_info_t* socket_info)
{
    int ret;
    int type;
    int address_family;
    int protocol;
    struct sockaddr_in sockaddr;
    int flags;
    int optval;
    socklen_t optlen;
    int32_t status;

    status = SOCKET_SUCCESS;

    // Set the socket address family
    if(socket_info->address_family==ip_ver_4)
    {
        address_family=AF_INET; // IP version 4
    }
    else if(socket_info->address_family==ip_ver_6)
    {
        address_family=AF_INET6; // IP version 6
    }
    else
    {
        status = SOCKET_CREATE_ERR;
        return status;        
    }

    // Set the socket type 
    if(socket_info->type==stream)
    {
        type=SOCK_STREAM; // Connection based
    }
    else if(socket_info->type==dgram)
    {
        type=SOCK_DGRAM; // Connectionless
    }
    else
    {
        status = SOCKET_CREATE_ERR;
        return status;        
    }

    // Set the socket protocol
    protocol = IPPROTO_IP; // IP protocol

    // Create the socket
    ret = socket(address_family, type, protocol);
    if(ret == -1)
    {
        status = SOCKET_CREATE_ERR;
        return status;
    }

    // Assign values to the socket_info structure 
    socket_info->sockfd = ret;
    socket_info->created = true;
    OS_printf("socket_info->sockfd = %d\n",socket_info->sockfd);
    OS_printf("socket_info->created = %d\n",socket_info->created);

    // Bind server sockets to localhost and port number
    if(socket_info->category==server || socket_info->type==dgram)
    {
        // Prepare the sockaddr_in structure
        sockaddr.sin_family = address_family;
        sockaddr.sin_addr.s_addr = inet_addr(socket_info->ip_address);
        sockaddr.sin_port = htons(socket_info->port_num);       

        // Bind the socket 
        ret = bind(socket_info->sockfd,(struct sockaddr *)&sockaddr , sizeof(sockaddr));
        if(ret != 0)
        {
            status = SOCKET_BIND_ERR;
            return status;
        }  

        // Assign values to the socket_info structure
        socket_info->bound = true;
        OS_printf("socket_info->bound = %d\n",socket_info->bound);
    }

    // Make socket non-blocking?
    if(socket_info->block==false)
    {
        flags = fcntl(socket_info->sockfd, F_GETFL, 0);
        fcntl(socket_info->sockfd, F_SETFL, flags | O_NONBLOCK);
    }

    // Turn keep alive on?
    if(socket_info->keep_alive==true)
    {
        optval = 1;
        optlen = sizeof(optval);
        setsockopt(socket_info->sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);      
    }

    return status;
}

// Listens on a connection on a socket
//
// Inputs:
//      socket_info->bound
//      socket_info->sockfd
//
// Outputs:
//      socket_info->listening
int32_t socket_listen(socket_info_t* socket_info)
{
    int ret;
    int pending_connections_queue_size;
    int32_t status;

    status = SOCKET_SUCCESS;
    pending_connections_queue_size = 5;
    
    // Only listen on a stream socket that has been bound
    if( (!socket_info->type==stream)||(!socket_info->bound) )
    {
        status = SOCKET_LISTEN_ERR;
        return status;
    }

    // Listen on the socket
    ret = listen(socket_info->sockfd, pending_connections_queue_size);
    if(ret != 0)
    {
        status = SOCKET_LISTEN_ERR;
        return status;
    }  

    // Assign values to the socket_info structure
    socket_info->listening = true;
    OS_printf("socket_info->listening = %d\n",socket_info->listening);

    return status;
}

// Accepts a connection on a socket
//
// Inputs:
//      socket_info->listening
//      socket_info->sockfd
//
// Outputs:
//      socket_info->connected
//      socket_info->sockfd
int32_t socket_accept(socket_info_t* socket_info)
{
    int c;
    int ret;
    struct sockaddr_in client;
    int32_t status;
    int ip_str_len;

    status = SOCKET_SUCCESS;

    // Only accept connections on a socket that is in a listening state
    if(socket_info->listening==false)
    {
		status = SOCKET_ACCEPT_ERR;
        return status;        
    }

    // Accept incoming connection 
    c = sizeof(struct sockaddr_in);
    ret = accept(socket_info->sockfd, (struct sockaddr *)&client, (socklen_t*)&c);
	if (ret == -1)
	{
        // Handle non-blocking sockets
        if( (socket_info->block==false) && (errno==EAGAIN) )
        {
		    status = SOCKET_TRY_AGAIN;
            return status;
        }   
        if( (socket_info->block==false) && (errno==EWOULDBLOCK) )
        {
		    status = SOCKET_TRY_AGAIN;
            return status;
        }          
		status = SOCKET_ACCEPT_ERR;
        return status;
	}

    // Assign values to the socket_info structure
    socket_info->sockfd = ret;
    socket_info->connected = true;
    OS_printf("socket_info->sockfd = %d\n",socket_info->sockfd);
    OS_printf("socket_info->connected = %d\n",socket_info->connected);

    return status;
} 

// Initiates a connection to a remote ip address and port number
//
// Inputs:
//      socket_info->created
//      socket_info->category
//      socket_info->address_family
//      socket_info->sockfd
//      remote_ip_address (the remote ip address)
//      remote_port_num (the remote port number)
//
// Outputs:
//      socket_info->connected
int32_t socket_connect(socket_info_t* socket_info, char* remote_ip_address, int remote_port_num)
{
    int ret;
    int address_family;
    struct sockaddr_in server;
    int32_t status;
    int error_num;

    status = SOCKET_SUCCESS;

    // Only make outbound connections on client sockets that have been created
    if(  (socket_info->category!=client)||(!socket_info->created) )
    {
        status = SOCKET_CONNECT_ERR;
        return status; 
    }

    // Set the socket address family
    if(socket_info->address_family==ip_ver_4)
    {
        address_family=AF_INET; // IP version 4
    }
    else if(socket_info->address_family==ip_ver_6)
    {
        address_family=AF_INET6; // IP version 6
    }
    else
    {
        status = SOCKET_CONNECT_ERR;
        return status;        
    }

    // Prepare the server structure 
	server.sin_family = address_family;
	server.sin_addr.s_addr = inet_addr(remote_ip_address);
	server.sin_port = htons(remote_port_num);  

    // Connect to remote address/port 
	ret = connect(socket_info->sockfd , (struct sockaddr *)&server , sizeof(server));
	if (ret == -1)
	{
        error_num = errno;
        // Ignore "Operation in progress" error on a non-blocking socket
        if( (socket_info->block==0) && (error_num==115) )
        {
            // Do nothing 
        }
        else
        {
            printf("errno = %d\n",error_num);
            status = SOCKET_CONNECT_ERR;
            return status;
        }
        
	}    

    // Assign values to the socket_info structure
    socket_info->connected = true;    

    return status;
}

int32_t socket_send(socket_info_t* socket_info, uint8_t* buffer, size_t buflen, size_t* bytes_sent, char* remote_ip_address, int remote_port_num)
{
    int ret;
    int32_t status;
    int address_family;
    struct sockaddr_in remote_sockaddr;
    unsigned int i;
    status = SOCKET_SUCCESS;

   switch(socket_info->type)
    {
        case stream:
        {
            // Only send on stream sockets in a connected state
            if(socket_info->connected == false)
            {
                status = SOCKET_SEND_ERR;
                return status;
            }
            else
            {           
                ret = send(socket_info->sockfd, buffer, buflen, 0);
                if(ret == -1)
                {
                    status = SOCKET_SEND_ERR;     
                    return status;           
                }
            }
            *bytes_sent = ret;
            break;
        }
        case dgram:
        {
            // Prepare the remote_sockaddr structure 
            remote_sockaddr.sin_family = socket_info->address_family;
            remote_sockaddr.sin_addr.s_addr = inet_addr(remote_ip_address);
            remote_sockaddr.sin_port = htons(remote_port_num);

            ret = sendto(socket_info->sockfd, (void*)buffer, buflen, 0, (struct sockaddr *)&remote_sockaddr , sizeof(remote_sockaddr));
            if(ret == -1)
            {
                OS_printf("socket_send: sendto returned error %d \n", ret);
                status = SOCKET_SEND_ERR;     
                return status;           
            }

            if(ret != (int) buflen)
            {
                OS_printf("socket_send: sendto sent only %d out of %lu bytes! \n", ret, buflen);
            }

            *bytes_sent = ret;
            break;
        }
        default: 
        {
            status = SOCKET_SEND_ERR; 
            break;
        }
    }

    return status;
}

int32_t socket_recv(socket_info_t* socket_info, uint8_t* buffer, size_t buflen, size_t* bytes_recvd)
{
    int c;
    int ret;
    int ip_str_len;
    int32_t status;
    struct sockaddr_in remote_sockaddr;

    status = SOCKET_SUCCESS;

    switch(socket_info->type)
    {
        case stream:
        {
            // Only recv on stream sockets in a connected state
            if(socket_info->connected == false)
            {
                status = SOCKET_RECV_ERR;
                return status;
            }
            else
            {           
                ret = recv(socket_info->sockfd, (void*)buffer, buflen, 0);
                if(ret == 0)
                {
                    // Client disconnected
                    socket_info->connected = false;
                    status = SOCKET_RECV_ERR;
                    return status;
                }
                else if(ret == -1)
                {
                    // Handle non-blocking sockets
                    if( (socket_info->block==false) && (errno==EAGAIN) )
                    {
                        status = SOCKET_TRY_AGAIN;
                        return status;
                    }   
                    if( (socket_info->block==false) && (errno==EWOULDBLOCK) )
                    {
                        status = SOCKET_TRY_AGAIN;
                        return status;
                    }
                    status = SOCKET_RECV_ERR;     
                    return status;           
                }
                *bytes_recvd = ret;
            }
            break;
        }
        case dgram:
        {
            c = sizeof(struct sockaddr_in);
            ret = recvfrom(socket_info->sockfd, (void*)buffer, buflen, 0, (struct sockaddr *)&remote_sockaddr, (socklen_t*)&c);
            if(ret == -1)
            {
                // Handle non-blocking sockets
                if( (socket_info->block==false) && (errno==EAGAIN) )
                {
                    status = SOCKET_TRY_AGAIN;
                    return status;
                }   
                if( (socket_info->block==false) && (errno==EWOULDBLOCK) )
                {
                    status = SOCKET_TRY_AGAIN;
                    return status;
                }
                status = SOCKET_RECV_ERR;     
                return status;           
            }   
            *bytes_recvd = ret;   
            break;
        }
        default: 
        {
            status = SOCKET_RECV_ERR; 
            break;
        }
    }

    return status;
}

int32_t socket_close(socket_info_t* socket_info)
{
    int ret;
    int32_t status;

    status = SOCKET_SUCCESS;

    ret = close(socket_info->sockfd);
    if(ret == -1)
    {
        status = SOCKET_CLOSE_ERR;
        return status;
    }

    // Assign values to the socket_info structure
    // TBD

    return status;
}
