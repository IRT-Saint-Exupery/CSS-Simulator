#pragma once

#include <stdio.h>
#include <time.h>
#include "osapi-clock.h"
#include "ids_probes.h"

#define P0_NOT_ACTIVE -1

void update_Log_P0(FILE * log_file,OS_time_t simtime, int32 bytes_read, uint8 *buffer, unsigned int seconds, struct timespec *realtime, int8_t ids_output, uint8_t attack_tag);
