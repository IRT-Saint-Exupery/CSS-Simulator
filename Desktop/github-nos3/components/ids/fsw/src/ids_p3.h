#pragma once
#include <stdio.h>
#include <time.h>
#include "osapi-clock.h"
#include "ids_probes.h"
#include "cfe_es_extern_typedefs.h"
#include "../../../../fsw/cfe/modules/core_api/fsw/inc/cfe_sb_api_typedefs.h"

#include <pthread.h>
#include <unistd.h>

//Bison parsing + rule syntax
#include "RulesParser/rules.h"
#include "RulesParser/probe3_rules.h"
#include "ids_analyzer.h"

//#define BUS_REAL_RULE_NB 14//1000
//#define BUS_RULE_NB 6//7

#define P3_RULES_MODULE_NUMBER 6

#define P3_NOT_ACTIVE -1

#define CAM_RESULT_TIMEOUT 390000000 //approx 40sec in ticks
//#define P3_CAMRESULTS_WORKAROUND_DEBUG
//#define P3_SATSPIN_DEBUG
//#define IDS_BUS_TAG_DEBUG

struct p3_rules {
	rule_t * rules;
	uint8_t flow_count[5]; //Used to count the occurences of CAM_EXP3_CC before allowing the restart of CAM, cap at 255 occurrences
};

struct msg_couple {
	uint16_t MsgId;
	uint8_t FC;
};

struct task_profile {
	const char *taskname;
	struct msg_couple *couples_tab;
	uint8_t couples_nb;
};


//void init_rule_parsing(void);
void init_p3(void);
void update_Log_P3(FILE * log_file, OS_time_t *simtime, unsigned int seconds, struct timespec *realtime, uint16_t msgId, int16_t fc, uint8_t is_tc, CFE_ES_AppId_t AppId, char *TaskName, CFE_SB_Buffer_t *Content, size_t ContentSize, uint8_t attack_tag, int8_t is_allowed);
char parse_rules_bus(void);
void parse_single_rule_bus(char *split_ptr, uint8_t *split_count, char *saveptr1, char param_data_holder[20], int i,  uint8_t statefull_rules_count);

void* time_thread_fct(void *arg);
void p3_cond_action_processing(subrule_t *subrule, rule_t *rule, int i/*rule nb*/, uint16_t msgId, uint16_t fc, uint8_t *param_ptr, char * is_accepted, char * raise_alert, char * rule_matched);
char is_command_allowed_bus(CFE_SB_Buffer_t *Content, size_t ContentSize, uint16_t msgId, uint16_t fc, char *Taskname, int8_t *alert_rule_nb);
