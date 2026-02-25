#include <iostream>
#include "FE_error.h"
#include "FE_udp.h"
#include "FE_netconfig.h"
#include "FE_routeconfig.h"
#include "FE_tm.h"
#include "FE_flagsconfig.h"
#include "FE_watchdog.h"
//#include "FE_tc.h"

int feTm (void *unused)
{
	try {
		struct {
			char ttl;
			union {
				ccsdsTmTfHeader_t tmHeader;
				char buffer[MAX_TF_SIZE];
			};
		} s;

		int nbReadBytes;
		int satId;

		static int receiveSocket = createServerSocket (g_serverConnections.tmServaddr);
		static int sendSocket = createSendSocket();

		std::cout << "FE_tm start" << std::endl;

		while (true) {
			receiveUdp (receiveSocket, &s.ttl, sizeof(s), nbReadBytes);

			//printf ("************* ttl %d, nbReadBytes %d \n", s.ttl, nbReadBytes);

			if (peakSpaceSideFlag) {
				static int sendSocket = createSendSocket();
				sendUdp (sendSocket, peakAddr, &s.ttl, nbReadBytes);
			}

			satId = TMTF_RD_SCID (s.tmHeader) ; //TODO TBC
			//printf("SatId = %d \n", satId);

		    if(satId >= MAX_WD_NBSAT) { // this is specific to watchdog messages
		        int origSatId = satId - MAX_WD_NBSAT; //TODO this limit watchdog to 100 sat
		        int now = time(0);
		        //printf("origSat = %d \n", origSatId);

		        if(g_lastCom.find(origSatId) != g_lastCom.end()) { // valid origSatId
		        	g_lastCom[origSatId] = now;
		        	//printf("Watchdog received from SAT %d \n", origSatId);
		        }
		        //return 0;
		    }

            //printf("g_cosmosConnectionsMap.find(satId) = %d \n",g_cosmosConnectionsMap.find(satId));

		    else if (g_cosmosConnectionsMap.find(satId) != g_cosmosConnectionsMap.end()) {

				//printf ("************* TLM received by frontEnd for the SAT %d\n", satId);

				FE_cosmosConnection_t *cosmosConnection = g_cosmosConnectionsMap[satId];

				sendUdp (sendSocket, cosmosConnection->distaddr, s.buffer, nbReadBytes - sizeof(s.ttl));

				// show the packet
				//for ( int i = 0; i < (int)(nbReadBytes-sizeof(s.ttl)); i++) {
			    //for ( int i = 0; i < 20; i++) {
				//		    		printf ("%02X ",(unsigned char) s.buffer[i]);
			    //}
			    //printf ("\n");

				if (peakGroundSideFlag) {
					static int sendSocket = createSendSocket();
					sendUdp (sendSocket, peakAddr, s.buffer, nbReadBytes - sizeof(s.ttl));
				}
			}
		}
	}
	catch (feErrorException_t &e)
	{
		fprintf (stderr, "%s\n", e.what());
		return -1;
	}

}
