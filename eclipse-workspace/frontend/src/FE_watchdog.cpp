#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "FE_util.h"
#include "FE_error.h"
#include "FE_udp.h"
#include "FE_netconfig.h"
#include "FE_routeconfig.h"
#include "FE_tc.h"
#include "FE_flagsconfig.h"
#include "FE_error.h"
#include "FE_udp.h"
#include "FE_netconfig.h"
#include "FE_routeconfig.h"
#include "FE_tm.h"
#include "FE_flagsconfig.h"
#include "FE_watchdog.h"

void sendToSpace (int socket, SOCKADDR_IN addr, char *buffer, int nbBytes);
uint16_t Crypto_Calc_FECF(const uint8_t* ingest, int len_ingest);
void writeFECF(uint8_t* buffer, int nbReadBytes, uint16_t fecf);


void watchdogInit(void) {
	for (int i = 0; i < MAX_WD_NBSAT; i++) {
		g_upMessageSent[i] = false;
	}
}

void timer_handler(int sig) {
    static bool first = 1;
    static uint8_t command[] = {0x02,0x20,0x01,0x10,0x11,0x00,0x00,0x00} ;  //TC like

    static int sendSocket;

    if(first == 1) {    
	    command[2] = MAX_WD_NBSAT;
	    uint16_t fecfout = Crypto_Calc_FECF((uint8_t*)command+1, 8-3);
	    writeFECF((uint8_t*)command+1, 7, fecfout);
	    sendSocket = createSendSocket();
	    first = 0;
    }
    
    //send watchdog
    for (FE_satConnection_t *connection : g_satConnections) {
        int destSatId = connection->satId;

    	if (g_routesMap.find(destSatId) != g_routesMap.end()) {

    		if(destSatId == g_routesMap[destSatId]) { // destSatId is visible 
    			//printf(" destSAT = %d \n", destSatId);
    			sendToSpace (sendSocket, connection->distaddr, (char*) command, sizeof(command));

    		}
    	}
    
    }
    
    //check watchdog
    //TODO when new sat is visible (new watchdog is received), then new sat is preferred and we compute and propagate the new table.

    int now = time(0);
    for (auto &pair: g_lastCom ) {
    	int satId = pair.first;
    	//printf(" SAT = %d \n", satId);
    	if(satId == g_routesMap[satId]) { // destSatId is visible

			if(now - pair.second > WATCHDOG_TIMEOUT ) {
				//printf(" test = %d \n", now - pair.second);
				printf("Link between SAT %d and MCS is down. \n", satId);
				g_upMessageSent[satId] = false;
			}

			else if(!g_upMessageSent[satId]) {
				printf("Link between SAT %d and MCS is up. \n", satId);
				g_upMessageSent[satId] = true;
			}
    	}
    }

}


int startTimer() {
    struct sigaction sa;
    struct sigevent sev;
    struct itimerspec its;
    timer_t timerid;

    // Set up signal handler
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = timer_handler;
    sigaction(SIGRTMIN, &sa, NULL);

    // Create timer
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        printf("timer_create");
        exit(EXIT_FAILURE);
    }

    // Set timer to trigger every 1 second
    its.it_value.tv_sec = 1;        // Initial delay
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 4;     // Interval
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        printf("timer_settime");
        exit(EXIT_FAILURE);
    }

    return 0;
}

