/*
 * FE_netconfig.h
 *
 *  Created on: Jul 8, 2024
 *      Author: j.Girard
 */

#ifndef FE_NETCONFIG_H_
#define FE_NETCONFIG_H_

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#define SOCKADDR_IN struct sockaddr_in
#endif
#include <vector>
#include <unordered_map>

using namespace std;

// Structures
typedef struct {
	SOCKADDR_IN tcServaddr;   // Tc Fe Rcv port
	SOCKADDR_IN tmServaddr;   // Tm FE Rcv port
} FE_serverConnection_t;

typedef struct {
	SOCKADDR_IN distaddr;  // sat side. Include sat ip and sat receveive port
	int satId;
} FE_satConnection_t;

typedef struct {
	SOCKADDR_IN distaddr; // cosmos side. Include cosmos ip and cosmos receveive port
	vector<int> satIdVect;
} FE_cosmosConnection_t;

// Global variables
extern FE_serverConnection_t			g_serverConnections;	// To receieve
extern vector<FE_satConnection_t*> 		g_satConnections;		// To transmit
extern vector<FE_cosmosConnection_t*> 	g_cosmosConnections;	// To transmit
extern unordered_map<int,FE_satConnection_t*> 		g_satConnectionsMap;	// To map a sat id to a satConnection
extern unordered_map<int,FE_cosmosConnection_t*> 	g_cosmosConnectionsMap;	// To map a sat id to a cosmosConnection
extern unordered_map<int,int> 	g_lastCom;	// To map a sat id to last watchdog reception time
extern unordered_map<int,bool> 	g_upMessageSent;

// This function read the network configuration from a text file.
// It fills both g_satConnectionList and g_cosmosConnectionList
void readNetConfiguration (const char *filename);


#endif /* FE_NETCONFIG_H_*/
