/*
 * FE_udp.h
 *
 *  Created on: Jul 8, 2024
 *      Author: j.Girard
 */
#ifndef FE_UDP_H_
#define FE_UDP_H_

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#define SOCKADDR_IN struct sockaddr_in
#endif
#include <map>


int createServerSocket (const SOCKADDR_IN &servaddr);  // Sockets are left open. Created once for all for any thread
int createSendSocket ();

void receiveUdp (int sockfd, char *buffer, int bufferSize, int &nbReadBytes); // Blocking function
void sendUdp (int sockfd, const SOCKADDR_IN &distaddr, char *buffer, int nbBytesToSend);
int send_udp_packet(const void *packet, size_t packet_len, int port);


#endif /* FE_UDP_H_*/
