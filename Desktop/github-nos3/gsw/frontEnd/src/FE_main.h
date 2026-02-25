/*
 * FE_main.h
 *
 *  Created on: Jul 8, 2024
 *      Author: nos3
 */

#ifndef GSW_FRONTEND_SRC_FE_MAIN_H_
#define GSW_FRONTEND_SRC_FE_MAIN_H_

// Structures
typedef struct {
	SOCKADDR_IN servaddr;   // sat side. Include sat ip and port
	int FeRcvPort;
	int satId;
} FE_satConnection_t;

typedef struct {
	SOCKADDR_IN servaddr;	// cosmos side. Include cosmos ip and port
	int FeRcvPort;
	list<int> satIdList;
} FE_cosmosConnection_t;

// Global variables
list<FE_satConnection_t*> 		g_satList;
list<FE_cosmosConnection_t*> 	g_cosmosList;




#endif /* GSW_FRONTEND_SRC_FE_MAIN_H_ */
