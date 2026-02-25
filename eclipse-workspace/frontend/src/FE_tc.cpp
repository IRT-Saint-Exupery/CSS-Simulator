#include <iostream>
#include <mutex>
#include "FE_util.h"
#include "FE_error.h"
#include "FE_udp.h"
#include "FE_netconfig.h"
#include "FE_routeconfig.h"
#include "FE_tc.h"
#include "FE_flagsconfig.h"

extern mutex lock_route_config_TC;

inline void    TCTF_WR_SCID(ccsdsTcTfHeader_t & hdr, int scid)
{
	hdr.Id[0] = hdr.Id[0] & ~0x03 | ((scid >> 8) & 0x03);
	hdr.Id[1] = scid & 0xFF;
}

void sendToSpace (int socket, SOCKADDR_IN addr, char *buffer, int nbBytes)
{
	sendUdp (socket, addr, buffer, nbBytes);

	if (peakSpaceSideFlag) {
		static int sendSocket = createSendSocket();
		sendUdp (sendSocket, peakAddr, buffer, nbBytes);
	}
}

uint16_t Crypto_Calc_FECF(const uint8_t* ingest, int len_ingest)
{
    uint16_t fecf = 0xFFFF;
    uint16_t poly = 0x1021; // TODO: This polynomial is (CRC-CCITT) for ESA testing, may not match standard protocol
    uint8_t bit;
    uint8_t c15;
    int i;
    int j;

    for (i = 0; i < len_ingest; i++)
    { // Byte Logic
        for (j = 0; j < 8; j++)
        { // Bit Logic
            bit = ((ingest[i] >> (7 - j) & 1) == 1);
            c15 = ((fecf >> 15 & 1) == 1);
            fecf <<= 1;
            if (c15 ^ bit)
            {
                fecf ^= poly;
            }
        }
    }
    
    return fecf;
}

bool checkFECF(uint8_t* buffer, int nbReadBytes, uint16_t &fecfout) 
{
    fecfout = Crypto_Calc_FECF((uint8_t*)buffer, nbReadBytes-2);	
    return ( fecfout == (buffer[nbReadBytes-2]*256 + buffer[nbReadBytes-1]) );	
}

void writeFECF(uint8_t* buffer, int nbReadBytes, uint16_t fecf) 
{
	buffer[nbReadBytes-2] = fecf >> 8;
	buffer[nbReadBytes-1] = fecf & 0xFF;
}


int feTc (void *unused)
{
	try {
		struct {
			char ttl;
			union {
				ccsdsTcTfHeader_t tcHeader;
				char buffer[MAX_TC_SIZE];
			};
		}s;
		int nbReadBytes;
		int destSatId;
		int nextSatId;

		static int receiveSocket = createServerSocket (g_serverConnections.tcServaddr);
		static int sendSocket = createSendSocket();
		uint16_t fecf; 

		std::cout << "FE_tc start" << std::endl;

		while (true) {
			s.ttl = TTL_INIT;

			receiveUdp (receiveSocket, s.buffer, MAX_TC_SIZE, nbReadBytes);

			{
				lock_guard<mutex> guard(lock_route_config_TC);


				if (peakGroundSideFlag) {
					static int sendSocket = createSendSocket();
					sendUdp (sendSocket, peakAddr, s.buffer, nbReadBytes);
				}

				destSatId = TCTF_RD_SCID (s.tcHeader);

				printf ("************** TC received by frontEnd for the SAT %d\n", destSatId);

				if (tcVerboseFlag) {
					for ( int i = 0; i < nbReadBytes; i++) {
						printf ("%02X ",(unsigned char) s.buffer[i]);
					}
					printf ("\n");

					uint16_t fecfout;

					if(!checkFECF((uint8_t*)s.buffer, nbReadBytes, fecfout)) {

						writeFECF((uint8_t*)s.buffer, nbReadBytes, fecfout);
						printf("******** Modified FECF ! ******* \n");

						for ( int i = 0; i < nbReadBytes; i++) {
							printf ("%02X ",(unsigned char) s.buffer[i]);
						}
						printf ("\n");

					}
				}
				


				if (g_routesMap.find(destSatId) != g_routesMap.end()) {

					// Normal treatment except for (attack 1 bis and (destSatId = 1 or 2))
					if (!(attack16Flag && (destSatId == 1|| destSatId == 2))) {

						nextSatId = g_routesMap [destSatId];

						if (g_satConnectionsMap.find(nextSatId) != g_satConnectionsMap.end()) {

							FE_satConnection_t *satConnection = g_satConnectionsMap[nextSatId];

							sendToSpace (sendSocket, satConnection->distaddr, &s.ttl, nbReadBytes+sizeof(s.ttl));

						}
					}

					// ================================Attack 17 Step 1=====================================
					// Duplication of commands for 1 on 2
					if (attack17Flag) {
						if (destSatId == 1) {
							destSatId = 2;
							TCTF_WR_SCID (s.tcHeader, destSatId);
							fecf = Crypto_Calc_FECF((uint8_t*)s.buffer, nbReadBytes-2);
							s.buffer[nbReadBytes-2] = fecf >> 8;
							s.buffer[nbReadBytes-1] = fecf & 0xFF;

							if (g_routesMap.find(destSatId) != g_routesMap.end()) {
								nextSatId = g_routesMap [destSatId];

								if (g_satConnectionsMap.find(nextSatId) != g_satConnectionsMap.end()) {

									FE_satConnection_t *satConnection = g_satConnectionsMap[nextSatId];

									sendToSpace (sendSocket, satConnection->distaddr, &s.ttl, nbReadBytes+sizeof(s.ttl));
								}
							}
						}
					}
					// ================================Attack 16 Step 1=====================================
					// Inversion of commands between 1 and 2
					if (attack16Flag) {
						if (destSatId == 1 || destSatId == 2) {
							destSatId = 3 - destSatId;
							TCTF_WR_SCID (s.tcHeader, destSatId);
							fecf = Crypto_Calc_FECF((uint8_t*)s.buffer, nbReadBytes-2);
							s.buffer[nbReadBytes-2] = fecf >> 8;
							s.buffer[nbReadBytes-1] = fecf & 0xFF;

							if (g_routesMap.find(destSatId) != g_routesMap.end()) {
								nextSatId = g_routesMap [destSatId];

								if (g_satConnectionsMap.find(nextSatId) != g_satConnectionsMap.end()) {

									FE_satConnection_t *satConnection = g_satConnectionsMap[nextSatId];

									sendToSpace (sendSocket, satConnection->distaddr, &s.ttl, nbReadBytes+sizeof(s.ttl));
								}
							}
						}
					}

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
