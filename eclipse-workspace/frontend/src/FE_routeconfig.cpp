
#include <math.h>
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include "FE_util.h"
#include "FE_error.h"
#include "FE_routeconfig.h"
#include "FE_netconfig.h"

static vector<int> g_destIdVect;
static vector<int> g_nextIdVect;

unordered_map<dest_t,next_t>	g_routesMap;	// Routing table for the current node

void buildDestinationsIdVect (const char *cStringOfDestIds)
{
	if (g_destIdVect.size() != 0) {
		feError("Multiple defined destination Id list");
	}
	g_destIdVect = vectorFromStringOfIds (cStringOfDestIds);

	if (g_destIdVect.size() == 0) {
		feError("Found an empty list for destination Id list");
	}
}

void buildNextIdVect (const int node, const char *cStringOfNextIds)
{
	if (g_destIdVect.size() == 0) {
		feError ("Destinations Id list shall be defined before node routing");
	}

	g_nextIdVect = vectorFromStringOfIds (cStringOfNextIds);

	if (g_nextIdVect.size() != g_destIdVect.size()) {
		feError("Next id list doesn't define as many Id as the number of destinations (for node %d)", node);
	}

	return;
}

void buildRouteMap ()
{
	g_routesMap.clear();
	for (int i = 0; i < g_destIdVect.size(); i++) {
		dest_t destination = g_destIdVect[i];
		next_t nextStep    = g_nextIdVect[i];
		g_routesMap[destination] = nextStep;
	}
}

void parseRouteConfigurationFile (const char *filename, const this_t &thisNode)
{

	FILE *fileIn = fopen (filename, "r");
	if (fileIn == NULL) {
		feError ("Unable to open route configuration file (%s)", filename);
	}


	char *ret;
	char line[512];
	char cStringOfDestIds[512];
	char cStringOfNextIds[512];
	//char comment[512];
	int nodeId;
	g_destIdVect.clear();

	while (!feof(fileIn)) {
		ret = fgets(line, sizeof(line), fileIn);

		if (ret == nullptr || lineToIgnore(line)) {
		}
		else if (sscanf (line, "Dest ID %[^\n]\n",
				cStringOfDestIds) == 1) {
			buildDestinationsIdVect (cStringOfDestIds);
		}
		else if (sscanf (line, "Node ID %d %[^\n]\n",
				&nodeId, cStringOfNextIds) == 2) {
			if (nodeId == thisNode) {
				buildNextIdVect (nodeId, cStringOfNextIds);
			}
		}
		else {
			feWarning ("Configuration line not parsed |%s|", line);
		}
	}
	fclose (fileIn);
}

void readRouteConfiguration (const char *filename, const this_t &thisNode)
{
	parseRouteConfigurationFile (filename, thisNode);
	buildRouteMap ();
}


void FE_ReadConfiguration (const char *netConfFilename,const char *routeConfFilename)
{
	try {
		readNetConfiguration   (netConfFilename);
		readRouteConfiguration (routeConfFilename, GROUND_NODE);
	}
	catch (feErrorException_t &e)
	{
		fprintf (stderr, "%s\n", e.what());
		exit (-1);
	}
}
