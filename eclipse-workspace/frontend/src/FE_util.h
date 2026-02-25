/*
 * FE_udp.h
 *
 *  Created on: Jul 8, 2024
 *      Author: j.Girard
 */

#ifndef FE_UTIL_H_
#define FE_UTIL_H_

#include <vector>

std::vector<int> vectorFromStringOfIds (const char* cStringOfIds);
bool lineToIgnore (const char* line);


#endif /* FE_UTIL_H_ */
