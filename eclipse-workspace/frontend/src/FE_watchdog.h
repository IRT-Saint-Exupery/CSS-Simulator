/*
 * FE_watchdog.h
 *
 *  Created on: Jul 9, 2025
 *      Author: nos3
 */

#ifndef SRC_FE_WATCHDOG_H_
#define SRC_FE_WATCHDOG_H_

#define WATCHDOG_TIMEOUT 10
#define MAX_WD_NBSAT     100   // watchdog messages uses SCID 100, 101, 102, ... 100 + NB_SAT by default

void watchdogInit(void);
void timer_handler(int sig);
int startTimer();


#endif /* SRC_FE_WATCHDOG_H_ */
