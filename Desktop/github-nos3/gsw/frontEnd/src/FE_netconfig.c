#include "FE_main.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <math.h>
#include <stdio.h>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#define SOCKADDR_IN struct sockaddr
#endif


void FeError (const char *fmt,...)
{
	va_list args;
	fprintf (stderr, "FE ERROR : ");
	va_start (args,fmt);
	vfprintf(stderr, fmt,args);
	va_end (args);
	fprintf (stderr, "\n");
}

void cleanSockAddr(SOCKADDR_IN *addr)
{
	memset(addr, 0, sizeof(*addr));
}

void initSockAddr(SOCKADDR_IN *addr, const char *ipAdress, int port)
{
	int inet_pton (int af, const char *cp, void *buf);
	cleanSockAddr (addr);
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
    if (inet_pton(AF_INET, ipAdress, &addr->sin_addr) <= 0) {
        FeError("bad adress format");
        exit(EXIT_FAILURE);
    }
}

void initSatList (list<int> &satIdList, char* cSatIdList)
{
	int n;
	char *ptr = cSatIdList;
	int satId;
	while (sscanf (ptr, "%d %n", &satId, &n) == 1) {
		satIdList.push_back (satId);
		ptr += n;
	}
	if (satIdList.size() == 0) {
		fprintf (stderr,"A empty list of satId for one cosmos entry \n");
		exit (-1);
	}
}

void addNewCosmosConnection (char *ipAdress, int rPort, int dPort, char *satIdList)
{
	FE_cosmosConnection_t *cosmosConnection = new FE_cosmosConnection_t();
	initSockAddr (&(cosmosConnection->servaddr), ipAdress, dPort);
	cosmosConnection->FeRcvPort = rPort;
	initSatList (cosmosConnection->satIdList, satIdList);

	g_cosmosList.push_back (cosmosConnection);
}

void addNewSatConnection (int satId, char *ipAdress, int rPort, int dPort)
{
	FE_satConnection_t *satConnection = new FE_satConnection_t();
	initSockAddr (&satConnection->servaddr, ipAdress, dPort);
	satConnection->FeRcvPort = rPort;
	satConnection->satId = satId;

	g_satList.push_back (satConnection);
}

int checkConfiguration ()
{
	// At least one satellite connexion
	if (g_satList.size() == 0) {
		FeError ("no satellite found in configuration");
	}
	// At least one cosmos
	if (g_cosmosList.size() == 0) {
		FeError ("no cosmos found in configuration \n");
	}
	// No duplication of satId in satIdList of cosmos. One satellite is managed by only one cosmos.
	{
		set<int> satIdSet;
		for (FE_cosmosConnection_t *cosmos: g_cosmosList) {
			for (int satId: cosmos->satIdList) {
				if (satIdSet.find(satId) != satIdSet.end()) {
					FeError ("satId %d managed by more than one cosmos", satId);
					}
				else {
				satIdSet.insert (satId);
				}
			}
		}
	}
	// satId set is the same at space side and cosmos side
	{
		set<int> satIdSet;
		set<int> cosmosSatIdSet;
		for (FE_satConnection_t *satConnection : g_satList) {
			satIdSet.insert (satConnection->satId);
		}
		for (FE_cosmosConnection_t *cosmosConnection: g_cosmosList) {
			for (int satId : cosmosConnection->satIdList) {
				cosmosSatIdSet.insert (satId);
			}
		}
		if (satIdSet != cosmosSatIdSet) {
			FeError ("Sat Id set is not identical at space side and cosmos side");
		}
	}
	return 0;
}


int readConfiguration (const char *filename)
{
	FILE *fileIn = fopen (filename, "r");
	if (fileIn == NULL) {
		FeError ("Unable to open configuration file");
	}
	char line[512];
	char comment[512];
	int nItems;
	char ipAdress[512];
	char satIdList[512];
	int rPort, dPort;
	int satId;
	while (!feof(fileIn)) {
		fgets(line, sizeof(line), fileIn);
		if (line[0]=='/' && line[1]=='/') {
			// This is a comment line
		}
		else if (sscanf (line, "//%[^\n]\n",comment) == 1) {
			// This is a comment line
		}
		else if (sscanf (line, "%[ \t]\n", comment) == 1) {
			// This is an empty line
		}
		else if ((nItems = sscanf (line, "COSMOS %s %d %d %[^\n]\n",
					ipAdress, &rPort, &dPort, satIdList)) == 4) {
			addNewCosmosConnection (ipAdress, rPort, dPort, satIdList);
		}
		else if ((nItems = sscanf (line, "SAT %d %s %d %d \n",
						&satId, ipAdress, &rPort, &dPort)) == 4) {
			addNewSatConnection (satId, ipAdress, rPort, dPort);
		}
		else {
			fprintf (stderr, "Configuration line not parsed |%s|", line);
		}
	}
	fclose (fileIn);
	checkConfiguration ();

	return 0;
}


int main()
{
#ifdef WIN32
	WSADATA	      wsaData;
	if(WSAStartup(MAKEWORD(2,2), &wsaData ) != 0 ){
		cerr <<"WSAStartup a échoué "<< endl;
		return 1;
	}
#endif

	readConfiguration ("c:\\temp\\config.txt");

}
