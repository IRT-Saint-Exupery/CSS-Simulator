/*
 * FE_TC.h
 *
 *  Created on: Jul 8, 2024
 *      Author: j.Girard
 */

#ifndef FE_TCL_H_
#define FE_TC_H_

#define MAX_TC_SIZE	1'000'000
#define TTL_INIT 10

typedef struct
{
	unsigned char Id[6];
} ccsdsTcTfHeader_t;

#define TCTF_RD_SCID(hdr)                   ((((hdr).Id[0] & 0x03) << 8) | ((hdr).Id[1]))
//#define TCTF_RD_SCID(hdr)                   1


int feTc (void *unused);

#endif /* FE_TC_H_ */
