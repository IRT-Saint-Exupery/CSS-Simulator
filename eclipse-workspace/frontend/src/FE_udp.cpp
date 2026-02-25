#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include "FE_util.h"
#include "FE_error.h"
#include "FE_netconfig.h"
#include "FE_udp.h"

int createServerSocket (const SOCKADDR_IN &servaddr)  // The receive socket is left open. Created once by each thread
{
	int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        feError("socket creation failed");
    }
    if (bind(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        close(sockfd);
    	feError("bind failed (%d)", errno);
    }

	return sockfd;
}

int createSendSocket ()  // The send socket is left open. Created once for all by each thread
{
	int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        feError("socket creation failed (%d)", errno);
    }

    return sockfd;
}

void receiveUdp (int sockfd, char *buffer, int bufferSize, int &nbReadBytes)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    nbReadBytes = recvfrom(sockfd, buffer, bufferSize, MSG_WAITALL, (sockaddr*)&client_addr, &addr_len);
}

void sendUdp (int sockfd, const SOCKADDR_IN &distaddr, char *buffer, int nbBytesToSend)
{
 	sendto(sockfd, buffer, nbBytesToSend, MSG_CONFIRM, (const sockaddr *)&distaddr, sizeof(distaddr));
}

int send_udp_packet(const void *packet, size_t packet_len, int port) {
    int sockfd;
    struct sockaddr_in dest_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    ssize_t sent_len = sendto(sockfd, packet, packet_len, 0,
                              (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (sent_len < 0) {
        perror("sendto");
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return 0;
}

