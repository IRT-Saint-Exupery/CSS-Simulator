#include <iostream>
#include <filesystem>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <fstream>
#include "FE_error.h"
#include "FE_netconfig.h"
#include "FE_routeconfig.h"
#include "FE_flagsconfig.h"
#include "FE_tc.h"
#include "FE_tm.h"
#include "FE_watchdog.h"
#include "FE_routegen.h"
#include "FE_visible.h"

using namespace std;

vector<thread> g_threads;

mutex lock_route_config_TC;

//#define GROUND_NODE 0
//this_t groundNode = GROUND_NODE;

//int nSats = 7; // this should correspond to IP_Scenario.sh !
int nSats = get_NSat_value_from_script("/home/nos3/Install_tools_nos3/IP_Scenario.sh");

// Fonction exécutée par chaque thread pour recevoir des messages UDP
/*void FE_ReadConfiguration (const char *netConfFilename,const char *routeConfFilename)
{
	try {
		readNetConfiguration   (netConfFilename);
		readRouteConfiguration (routeConfFilename, groundNode);
	}
	catch (feErrorException_t &e)
	{
		fprintf (stderr, "%s\n", e.what());
		exit (-1);
	}
}*/

void startTcThread ()
{
	g_threads.emplace_back(feTc,nullptr);
}

void startTmThread ()
{
	g_threads.emplace_back(feTm,nullptr);
}

void startVisiThread (int nSats)
{
	g_threads.emplace_back(runVisiMonitorForever, nSats);
}

void usage (void)
{
	cerr << "Usage: frontEnd <netConfigFile> <routeConfigFile> [<flagsFile>]" << endl;
}

int main(int argc, char *argv[])
{
	if (argc != 3 && argc != 4) {
		usage();
		return -1;
	}

	char pathconfig[512];
	strncpy(pathconfig,argv[0],sizeof(pathconfig)-1);
	char* pt = strrchr(pathconfig, '/');
	if (pt) {
		sprintf(pt+1,"../config/routeConfig.txt");
	}

	const char *netConfFilename = argv[1];
	const char *routeConfFilename = argv[2];
	if (argc == 4) {
		installSigHandler();
		readFlagsConfiguration(argv[3]);
	}

    // test Routing table generation (simple ring topology)
    int visibleSat = 1; // Only satX has ground visibility // this should be true at Sim Start ?!
    generateRoutingTable(PATH_CONFIG_FE, nSats, visibleSat);
    system("rm -rf /tmp/visi*"); // remove old visi files
    system("rm -rf /tmp/pos*"); // remove old pos files

    //FILE *fp = fopen("/tmp/visi1.txt", "w"); //create only visi1.txt HP: visible
    //fprintf(fp, "1");
    //fclose(fp);

    // create visi files at start
    for (int i = 1; i <= nSats; i++) {
    	std::ofstream file("visi" + std::to_string(i)+ ".txt");
    	if(i == 1 ) {
    		file << "1";
    	}
    	else {
    		file << "0";
    	}
    	file.close();
    }

    printf("FrontEnd is configured with %d Satellites.\n",nSats);
    printf("Routing table written to /config/routeConfig.txt \n");

	FE_ReadConfiguration (netConfFilename,routeConfFilename);

	watchdogInit();
	startTcThread ();
	startTmThread ();
	startTimer ();
	startVisiThread (nSats);

    // Attente de la terminaison des threads (normalement, ils ne termineront jamais)
    for (thread &t : g_threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    return 0;
}
