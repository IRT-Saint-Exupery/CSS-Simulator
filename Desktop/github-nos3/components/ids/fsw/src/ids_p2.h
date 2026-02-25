#pragma once
#include <stdio.h>
#include "osapi-clock.h"
#include "ids_probes.h"
#include "../../../cryptolib/include/crypto_structs.h"
#include <string.h>
#define P2_DOS_THRESHOLD 40 // edit later, replace by 10 ? useless to verify on P2?
#define P2_DOS_PACKET_THRESHOLD 4
#define P2_NOT_ACTIVE -1
#define CFDP_TIMEOUT_THRESHOLD 10 //10 seconds ?
//#define P2_PARSE_DEBUG

#define RULE_NB 5//6 edit later, should be set dynamically
#define REAL_RULE_NB 8//1000

void init_P2(void);
void init_P2_buffers(void);
void update_Log_P2(FILE * log_file, OS_time_t simtime, TC_t *tc_frame_struct, /*uint8* buffer,*/ unsigned int seconds, struct timespec *realtime, uint16_t msgId, int16_t fc, uint8_t is_tc, int8_t ids_output, uint8_t attack_tag);
char parse_rules(void/*char ** rules_array*/);
void parse_single_rule(char *split_ptr, uint8_t *split_count, char *saveptr1, char param_data_holder[20], int i,  uint8_t statefull_rules_count);
char is_command_allowed(TC_t *tc_frame_struct, OS_time_t simtime, int8_t *alert_rule_nb);

struct P2_struct {
    int64 buffer_last_time;
    char last_time_too_quick;
	uint16_t last_MsgIds[2];
	uint8_t last_FCs[2];
	uint8_t cfdp_is_ongoing;
	unsigned long cfdp_start_sec;
	// add other tabs and such who will be populated when parsing rules
};

struct statefull_rule {
	uint16_t msgid;
	uint8_t fc;
	uint16_t param_offset;
	uint8_t param_is_string; // WILL BE USED AS BOOL, REPLACE BY ENUM TO ADD OTHER TYPES (with also "undefined" state to handle if type is not well defined)
	uint16_t param_length;
	char param_vals[11/*TEST*/]; //arducam.so + '\0'
	char action[3]; // E0-9 + '\0', PoC won't work with more tha, 10 states
};
