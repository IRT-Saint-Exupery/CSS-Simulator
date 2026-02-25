
#include <math.h>
#include <stdio.h>
#include <cstring>
#include <set>
#include "FE_netconfig.h"
#include "FE_util.h"
#include "FE_error.h"

FE_serverConnection_t			g_serverConnections;
vector<FE_satConnection_t*> 	g_satConnections;
vector<FE_cosmosConnection_t*> 	g_cosmosConnections;
unordered_map<int,FE_satConnection_t*> 		g_satConnectionsMap;	// To map a sat id to a satConnection
unordered_map<int,FE_cosmosConnection_t*> 	g_cosmosConnectionsMap;	// To map a sat id to a cosmosConnection
unordered_map<int,int> 	g_lastCom;	// To map a sat id to last watchdog reception time
unordered_map<int,bool> 	g_upMessageSent;

static void cleanSockAddr(SOCKADDR_IN *addr)
{
	memset(addr, 0, sizeof(*addr));
}

static void initDistSockAddr(SOCKADDR_IN *addr, const char *ipAdress, uint16_t destPort)
{
	cleanSockAddr (addr);
	addr->sin_family = AF_INET;
	addr->sin_port = htons(destPort);
    if (inet_pton(AF_INET, ipAdress, &addr->sin_addr) <= 0) {
        feError("bad adress format %s", ipAdress);
    }
}

static void initServerSockAddr(SOCKADDR_IN *addr, uint16_t recPort)
{
	cleanSockAddr (addr);
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(recPort);
}

static void registerServerConnections (uint16_t tcRPort, uint16_t tmRPort)
{
	initServerSockAddr (&g_serverConnections.tcServaddr, tcRPort);
	initServerSockAddr (&g_serverConnections.tmServaddr, tmRPort);
}

static void addNewCosmosConnection (const char *cIpAdress, uint16_t destPort, const char *stringOfsatIds)
{
	int ret = FE_SUCCESS;
	FE_cosmosConnection_t *cosmosConnection = new FE_cosmosConnection_t();
	initDistSockAddr (&(cosmosConnection->distaddr), cIpAdress, destPort);
	cosmosConnection->satIdVect = vectorFromStringOfIds (stringOfsatIds);
	if (cosmosConnection->satIdVect.empty()) {
		feError ("Empty sat id list for a cosmos");
	}

	g_cosmosConnections.push_back (cosmosConnection);
}

static void addNewSatConnection (const char *cIpAdress, uint16_t destPort, int satId)
{
	FE_satConnection_t *satConnection = new FE_satConnection_t();
	initDistSockAddr (&(satConnection->distaddr), cIpAdress, destPort);
	satConnection->satId = satId;

	g_satConnections.push_back (satConnection);
}

static int checkNetConfiguration ()
{
	int ret = FE_SUCCESS;
	// At least one satellite connexion
	if (g_satConnections.size() == 0) {
		feError ("no satellite found in net configuration");
	}
	// At least one cosmos
	if (g_cosmosConnections.size() == 0) {
		feError ("no cosmos found innet  configuration \n");
	}
	// No duplication of satId in satIdList of cosmos. One satellite is managed by only one cosmos.
	{
		set<int> satIdSet;
		for (FE_cosmosConnection_t *cosmosConnection: g_cosmosConnections) {
			for (int satId: cosmosConnection->satIdVect) {
				if (satIdSet.find(satId) != satIdSet.end()) {
					feError ("satId %d managed by more than one cosmos", satId);
					}
				else {
				satIdSet.insert (satId);
				}
			}
		}
	}
	// satId set are the same at space side and cosmos side
	{
		set<int> satIdSet;
		set<int> cosmosSatIdSet;
		for (FE_satConnection_t *satConnection : g_satConnections) {
			satIdSet.insert (satConnection->satId);
		}
		for (FE_cosmosConnection_t *cosmosConnection: g_cosmosConnections) {
			for (int satId : cosmosConnection->satIdVect) {
				cosmosSatIdSet.insert (satId);
			}
		}
		if (satIdSet != cosmosSatIdSet) {
			feError ("Sat Id sets are not identical at space side and cosmos side");
		}
	}
	return ret;
}

static void buildSatConnectionMap()
{
	for (FE_satConnection_t *satConnection : g_satConnections) {
		g_satConnectionsMap[satConnection->satId] = satConnection;
	}
}

static void buildCosmosConnectionMap()
{
	for (FE_cosmosConnection_t *cosmosConnection : g_cosmosConnections) {
		for (int satId : cosmosConnection->satIdVect) {
			g_cosmosConnectionsMap[satId] = cosmosConnection;
		}
	}
}

static void buildWatchdogMap()
{
	int now = time(0);
	for (FE_satConnection_t *satConnection : g_satConnections) {
		g_lastCom[satConnection->satId] = now;
	}
}

static void parseNetConfigurationFile (const char *filename)
{
	FILE *fileIn = fopen (filename, "r");
	if (fileIn == NULL) {
		feError ("Unable to open network configuration file (%s)", filename);
	}
	char *ret;
	char line[512];
	char ipAdress[512];
	char satIdList[512];
	int nItems;
	int dPort;
	int tcRPort, tmRPort;
	int satId;

	while (!feof(fileIn)) {
		ret = fgets(line, sizeof(line), fileIn);
		if (ret == nullptr || lineToIgnore(line)) {
		}
		else if ((nItems = sscanf (line, "PORTS %d %d\n",
					&tcRPort, &tmRPort)) == 2) {
			registerServerConnections (tcRPort, tmRPort);
		}
		else if ((nItems = sscanf (line, "COSMOS %s %d %[^\n]\n",
					ipAdress, &dPort, satIdList)) == 3) {
			addNewCosmosConnection (ipAdress, dPort, satIdList);
		}
		else if ((nItems = sscanf (line, "SAT %d %s %d\n",
						&satId, ipAdress, &dPort)) == 3) {
			addNewSatConnection (ipAdress, dPort, satId);
		}
		else {
			feWarning ("Configuration line not parsed |%s|", line);
		}
	}
	fclose (fileIn);
}


void readNetConfiguration (const char *filename)
{
	parseNetConfigurationFile (filename);
	checkNetConfiguration ();
	buildCosmosConnectionMap();
	buildSatConnectionMap();
	buildWatchdogMap();
}


