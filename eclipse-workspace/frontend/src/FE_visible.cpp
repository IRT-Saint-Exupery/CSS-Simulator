#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <thread>
#include <chrono>
#include <functional>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FE_visible.h"
#include "FE_routegen.h"
#include "FE_routeconfig.h"

extern mutex lock_route_config_TC;

// Reads values from /tmp/visiX.txt for a given count
std::vector<int> getVisi(int fileCount) {
    std::vector<int> values(fileCount, 0);  // Initialize with default value 0

    std::string basePath = "/tmp/visi";

    for (int index = 1; index <= fileCount; ++index) {
        std::string filename = basePath + std::to_string(index) + ".txt";

        std::ifstream file(filename);
        if (!file.is_open()) {
            // File not opened, leave default 0 at this index
            continue;
        }

        int num = 0;
        file >> num;
        if (num == 0 || num == 1) {
            values[index - 1] = num;  // Assign valid value at proper index
        }
        // else leave default 0

        file.close();
    }

    return values;
}


void onNewPosition(int pos, int nSats) {

	int newTabSent = 0;

	printf("************** New Visible SAT, satId = %d \n", pos);

	generateRoutingTable(PATH_CONFIG_FE, nSats, pos);
	printf("************** New Routing table has been generated in /config for FE \n");

	{
	lock_guard<mutex> guard(lock_route_config_TC);
	//FE_ReadConfiguration (PATH_NETCONFIG_FE, PATH_CONFIG_FE);
	readRouteConfiguration (PATH_CONFIG_FE, GROUND_NODE);
	printf("************** Configuration updated in FE \n");
	}

	generateRoutingTable(PATH_CONFIG, nSats, pos);
	printf("************** New Routing table has been generated in /cf \n");

	// send with the new table
	newTabSent = sendRoutingTable(nSats,pos); // send Table via CFDP + config ISL (2 TC for each SAT)

	if (newTabSent >=0) {
		printf("************** New Routing table uploaded via TC to all the Satellites! \n");
	}
	else {
		printf("Upload ERROR of new table. \n");
	}

	printf("************** Visibility routing table update completed ! \n");

}


// Runs monitoring forever, passing new positions of '1' to the callback
void runVisiMonitorForever(int nSat) {
    static std::vector<int> lastVisi(nSat, 0);
    bool oneSatnomoreVisi = false;
    static int lastVisiprog = -1; // this is a satId from 1 to NBSAT
    static time_t lastchange = 0;

    while (true) {
        // Ensure getVisi returns exactly nSat values
        std::vector<int> visiValues = getVisi(nSat);

        int newOnePos = -1; // this is a satId from 1 to NBSAT

        for (int i = 0; i < nSat; ++i) {
            if (visiValues[i] == 1 && lastVisi[i] == 0) {
                newOnePos = i + 1;  // this is a satId
                lastVisi[i] = visiValues[i];
            }
            else if(visiValues[i] == 0 && lastVisi[i] == 1) {
            	lastVisi[i] = visiValues[i];
            	oneSatnomoreVisi = true;
            }

        }

        if (newOnePos == -1 && oneSatnomoreVisi) {   // case of not new
        	 for (int i = 0; i < nSat; ++i) {
					if (visiValues[i] == 1) {
						newOnePos = i + 1; // this is a satId
						break;
					}
        	 }
        }

        if (newOnePos != -1 && lastVisiprog != newOnePos && ((visiValues[lastVisiprog] == 0) || (time(NULL)-lastchange)>300)) { //TBC
            onNewPosition(newOnePos,nSat);  // Report position, 1-based index
            lastVisiprog = newOnePos;
            lastchange = time(NULL);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1)); // 1 sec sleep
    }
}


int get_NSat_value_from_script(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open script file");
        return -1; // error code
    }

    char line[256];
    int value = -1; // default if not found

    while (fgets(line, sizeof(line), file)) {
        // Look for a line that starts with "AA="
        if (strncmp(line, "NBSAT=", 6) == 0) {
            // Extract the numeric value after "AA="
            char *ptr = line + 6; // move past "AA="
            value = atoi(ptr);
            break;
        }
    }

    fclose(file);
    return value;
}

