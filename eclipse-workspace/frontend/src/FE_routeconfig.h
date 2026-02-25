/*
 * FE_routecongi.h
 *
 *  Created on: Jul 8, 2024
 *      Author: j.Girard
 */
#ifndef FE_ROUTECONFIG_H_
#define FE_ROUTECONFIG_H_

#include <unordered_map>

#define  PATH_CONFIG "/home/nos3/Desktop/github-nos3/fsw/build/exe/cpu1/cf/routeConfig.txt"
#define  PATH_CONFIG_FE "/home/nos3/eclipse-workspace/frontend/config/routeConfig.txt"
#define  PATH_NETCONFIG_FE "/home/nos3/eclipse-workspace/frontend/config/netConfig.txt"

using namespace std;

typedef int this_t, dest_t, next_t;

// Global variables
extern unordered_map<dest_t,next_t>	g_routesMap;	// Routing table for the current node

#define FE_REAL_VISIBILITY 1  // 1 - real visibility is considered // 0 - always visible // check also isl_app.h REAL_VISIBILITY.

#define GROUND_NODE 0

// This function read the routing configuration from a text file.
// It fills g_route map
void readRouteConfiguration (const char *filename, const this_t &node);

void FE_ReadConfiguration (const char *netConfFilename,const char *routeConfFilename);

#endif /* FE_ROUTECONFIG_H_*/
