/*
 * FE_flagconfig.h
 *
 *  Created on: Oct 10, 2024
 *      Author: j.Girard
 */

#ifndef FE_FLAGSCONFIG_H_
#define FE_FLAGSCONFIG_H_


#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#define SOCKADDR_IN struct sockaddr_in
#endif

// Structures

// Global variables
extern bool peakSpaceSideFlag;
extern bool peakGroundSideFlag;
extern SOCKADDR_IN peakAddr;

// Attack Flag
extern bool attack17Flag;
extern bool attack16Flag;

extern bool tcVerboseFlag;

// This function read the flag configuration from a text file.
// It is called on SIGUSR1 reception
void installSigHandler ();
void readFlagsConfiguration (const char *filename);


#endif /* FE_FLAGSCONFIG_H_*/
