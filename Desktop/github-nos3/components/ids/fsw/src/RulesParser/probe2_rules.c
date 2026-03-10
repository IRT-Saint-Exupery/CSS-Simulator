#include <stdlib.h>
#include "probe2_rules.h"
#include "rules.h"

rule_t probe2_rules[4];
subrule_t probe2_subrules[7];

uint8_t probe2_subrule_array0[1] = { 1 };
uint8_t probe2_subrule_array1[1] = { 1 };
uint8_t probe2_subrule_array2[2] = { 1, 3 };
uint8_t probe2_subrule_array3[1] = { 1 };

void probe2_init(void)
{
    probe2_subrules[0] = (subrule_t){ .header = (header_t){ .start_state = 0, .action = { .value = 68 } }, .rule_id = 0, .rule_body = (rule_body_t){ .message_id = 0x1806, .function_code = 5, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 160, .val_type = STRING , .value = "CI" } } };
	probe2_subrules[1] = (subrule_t){ .header = (header_t){ .start_state = 0, .action = { .value = 68 } }, .rule_id = 1, .rule_body = (rule_body_t){ .message_id = 0x1806, .function_code = 5, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 160, .val_type = STRING , .value = "ISL" } } };
	probe2_subrules[2] = (subrule_t){ .header = (header_t){ .start_state = 0, .action = { .value = 1 } }, .rule_id = 2, .rule_body = (rule_body_t){ .message_id = 0x188C, .function_code = 5, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 512, .val_type = STRING , .value = "/cf/arducam.so" } } };
	probe2_subrules[3] = (subrule_t){ .header = (header_t){ .start_state = 1, .action = { .value = 0 } }, .rule_id = 2, .rule_body = (rule_body_t){ .message_id = 0x18B3, .function_code = 36, .rule_parameters = (rule_parameters_t){ .offset = 96, .length = 80, .val_type = STRING , .value = "arducam.so" } } };
	probe2_subrules[4] = (subrule_t){ .header = (header_t){ .start_state = 1, .action = { .value = 68 } }, .rule_id = 2, .rule_body = (rule_body_t){ .message_id = 0x1806, .function_code = 5, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 160, .val_type = STRING , .value = "CAM" } } };
	probe2_subrules[5] = (subrule_t){ .header = (header_t){ .start_state = 1, .action = { .value = 68 } }, .rule_id = 2, .rule_body = (rule_body_t){ .message_id = 0x1806, .function_code = 6, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 160, .val_type = STRING , .value = "CAM" } } };
	probe2_subrules[6] = (subrule_t){ .header = (header_t){ .start_state = 0, .action = { .value = 68 } }, .rule_id = 3, .rule_body = (rule_body_t){ .message_id = 0x188C, .function_code = 7, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 512, .val_type = STRING , .value = "/cf" } } };
    
    probe2_rules[0] = (rule_t){ .holder=0, .state_subrules=&probe2_subrules[0], .subrule_number=probe2_subrule_array0 };
	probe2_rules[1] = (rule_t){ .holder=0, .state_subrules=&probe2_subrules[1], .subrule_number=probe2_subrule_array1 };
	probe2_rules[2] = (rule_t){ .holder=0, .state_subrules=&probe2_subrules[2], .subrule_number=probe2_subrule_array2 };
	probe2_rules[3] = (rule_t){ .holder=0, .state_subrules=&probe2_subrules[6], .subrule_number=probe2_subrule_array3 };
}
