#pragma once

#include <stdio.h>
#include <time.h>
#include "osapi-clock.h"
#include "ids_probes.h"
#include "../../../cryptolib/include/crypto.h"
#include <semaphore.h>

//Defines
//#define BUFFER_TIME_SIZE 10 
#define P1_DOS_THRESHOLD 20
#define P1_NOT_ACTIVE -1
#define IV_SIZE_CBC 16
#define PAD_LEN_CBC 1
//#define IDS_P1_DEBUG

#if defined IDS_P1_ENABLED
void init_probe1(void);
void init_probe1_buffer(void);
void add_to_buffer1(int64 ticks);
int64 compute_time_mean1(void);
void print_buffer1(void);

char is_rate_allowed(OS_time_t simtime);
char is_CRC_correct(uint8_t* data_buf, int frame_size);
char spi_is_initialized(uint16_t spi);
unsigned int get_bad_crc_count(void);
char is_len_correct(int32 readBytes, uint16_t spi);
#endif

#if defined LOGGING_P1_ENABLED
void update_Log_P1(FILE * log_file,OS_time_t simtime, int32 bytes_read, uint8 *buffer, unsigned int seconds, struct timespec *realtime, int8_t ids_output, uint8_t attack_tag);
#endif

struct P1_struct {
	int64 buffer_times[BUFFER_TIME_SIZE];
	int buffer_items_nb;
	int64 arrival_mean;
        char last_time_too_quick;
	char sa_state_table[64];
	unsigned int bad_crc_count;
	sem_t cfdp_sem;
	uint8_t cfdp_is_ongoing;
};
