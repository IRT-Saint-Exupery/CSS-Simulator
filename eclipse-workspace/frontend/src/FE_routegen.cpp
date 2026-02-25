#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdint.h>
#include <unistd.h>
#include "FE_udp.h"

//TODO improve the computation to handle ISL failure on some sat of the ring.

// Compute next hop in a ring topology
int nextHop(int current, int dest, int nSats, int visibleSat) {
    if (current == dest) return current;

    // Routing to ground (0) always goes through visibleSat
    if (dest == 0) {
        if (current == visibleSat) return 0; // visible sat → ground
        return nextHop(current, visibleSat, nSats, visibleSat);
    }

    // Ring distances (satellites are numbered 1..nSats)
    int leftDist  = (current - dest + nSats) % nSats;
    int rightDist = (dest - current + nSats) % nSats;

    if (leftDist <= rightDist) {
        // step left
        return (current == 1 ? nSats : current - 1);
    } else {
        // step right
        return (current == nSats ? 1 : current + 1);
    }
}


void generateRoutingTable(const char *filename, int nSats, int visibleSat) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Error opening file");
        return;
    }

    // Header row
    fprintf(fp, "Dest ID\t\t");  // Two tabs before first number
    for (int d = 0; d <= nSats; d++) {
        if (d < nSats)
            fprintf(fp, "%d\t", d);  // tab only between values
        else
            fprintf(fp, "%d", d);    // no trailing tab
    }
    fprintf(fp, "\n");

    // Each row = node ID
    for (int node = 0; node <= nSats; node++) {
        fprintf(fp, "Node ID\t%d\t", node);  // Tab before the number
        for (int dest = 0; dest <= nSats; dest++) {
            int value;
            if (node == dest) {
                value = node;
            } else if (node == 0) {
                value = visibleSat;  // Ground always forwards via visibleSat
            } else {
                value = nextHop(node, dest, nSats, visibleSat);
            }

            if (dest < nSats)
                fprintf(fp, "%d\t", value);  // tab only between values
            else
                fprintf(fp, "%d", value);    // last value, no trailing tab
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}




int sendRoutingTable(int nSats, int pos) {

	int status = 0;
	int statusAll = 0;
	// CF uplink request for new routing table /routeConfig.txt
	static uint8_t commandCFDP[] = {0x18, 0xb3, 0xc0, 0x00, 0x00, 0x82, 0x1e, 0x00, 0x00,
								0x2f, 0x72, 0x6f, 0x75, 0x74, 0x65, 0x43, 0x6f,
								0x6e, 0x66, 0x69, 0x67, 0x2e, 0x74, 0x78, 0x74,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x2f, 0x72, 0x6f, 0x75, 0x74, 0x65, 0x43, 0x6f,
								0x6e, 0x66, 0x69, 0x67, 0x2e, 0x74, 0x78, 0x74,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


	static uint8_t commandISL[] =  {0x18, 0x72, 0xC0, 0x00, 0x00, 0x45, 0x04, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x72, 0x6f, 0x75, 0x74,
									0x65, 0x43, 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x2e,
									0x74, 0x78, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00 };


	size_t packet_lenCFDP = sizeof(commandCFDP);
	size_t packet_lenISL = sizeof(commandISL);
	int cryptolibPort[] = {6010, 6012, 6014, 6016, 6018, 6020, 6022}; // Cryptolib input UDP ports //TODO define input file

	int posi = pos-1; // 0-based instead of 1 based SAtId
	std::vector<int> tabindex; // create a vector to define the order in the update of the routeConfig
	tabindex.push_back(posi);  // visible sat is the first and then neighbor
    tabindex.push_back((posi+nSats+1)%nSats);
    tabindex.push_back((posi+nSats-1)%nSats);
    tabindex.push_back((posi+nSats+2)%nSats);
    tabindex.push_back((posi+nSats-2)%nSats);
    tabindex.push_back((posi+nSats+3)%nSats);
    tabindex.push_back((posi+nSats-3)%nSats);

    printf("Order of update (satId) of routeConfig.txt \n");
    for(auto i:tabindex) {
    	printf("%d ",i+1);
    }
    printf("\n");

	//update tables via CFDP // update as a function of the new visible sat in the order
	for (int i = 0; i < nSats; i++) {
		status = send_udp_packet(commandCFDP, packet_lenCFDP, cryptolibPort[tabindex[i]]);
		statusAll = status + statusAll;
		usleep(100000);
	}

	sleep(5); // wait 5 sec before updating ISL

	// use the new tables via ISL // update as a function of the new visible sat in the order
	for (int i = 0; i < nSats; i++) {
		status = send_udp_packet(commandISL, packet_lenISL, cryptolibPort[tabindex[i]]);
		statusAll = status + statusAll;
		usleep(100000);
	}

	//return status success or not
	return statusAll;
}







