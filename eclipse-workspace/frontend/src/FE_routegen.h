/*
 * FE_routegen.h
 *
 *  Created on: Sep 2, 2025
 *      Author: nos3
 */

#ifndef SRC_FE_ROUTEGEN_H_
#define SRC_FE_ROUTEGEN_H_

int nextHop(int current, int dest, int nSats);
void generateRoutingTable(const char *filename, int nSats, int visibleSat);
int sendRoutingTable(int nSats, int pos);


#endif /* SRC_FE_ROUTEGEN_H_ */
