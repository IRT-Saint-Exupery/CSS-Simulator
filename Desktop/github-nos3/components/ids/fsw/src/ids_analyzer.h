#pragma once

#include "RulesParser/rules.h"
#include "../../../../fsw/cfe/modules/core_api/fsw/inc/cfe_sb_api_typedefs.h"
#include "ids_probes.h"
#include "ids_app.h"

typedef void (*cond_processing_fct_ptr) (subrule_t *subrule, rule_t *rule, int i, uint16_t msgId, uint16_t fc, uint8_t *param_ptr, char * is_accepted, char * raise_alert, char * rule_matched);

char analyzer_check_packet(rule_t * rules, uint8_t *packet_content, size_t content_size, uint16_t msgId, uint16_t fc, int8_t *alert_rule_nb, uint8_t module_number, cond_processing_fct_ptr cond_processing_function, uint16_t probe_rules_nb);