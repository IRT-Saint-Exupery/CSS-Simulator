/*
 * FE_TM.h
 *
 *  Created on: Jul 8, 2024
 *      Author: j.Girard
 */

#ifndef FE_TML_H_
#define FE_TM_H_

#define MAX_TF_SIZE	2000

typedef struct
{
	unsigned char synchro[4];
	unsigned char Id[6];
} ccsdsTmTfHeader_t;

#define TMTF_RD_SCID(hdr)          	   (((hdr).Id[0]  & 0x3F) << 4) | (((hdr).Id[1] & 0xF0) >> 4)
//#define TMTF_RD_SCID(hdr)          	   1

int feTm (void *unused);

#endif /* FE_TM_H_ */
