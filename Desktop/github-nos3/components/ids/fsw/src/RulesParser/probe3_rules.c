#include <stdlib.h>
#include "probe3_rules.h"
#include "rules.h"

rule_t probe3_rules[6];
subrule_t probe3_subrules[14];

uint8_t probe3_subrule_array0[1] = { 1 };
uint8_t probe3_subrule_array1[1] = { 1 };
uint8_t probe3_subrule_array2[2] = { 1, 3 };
uint8_t probe3_subrule_array3[1] = { 1 };
uint8_t probe3_subrule_array4[2] = { 1, 3 };
uint8_t probe3_subrule_array5[3] = { 1, 1, 1 };

void probe3_init(void)
{
    probe3_subrules[0] = (subrule_t){ .header = (header_t){ .start_state = 0, .action = { .value = 68 } }, .rule_id = 0, .rule_body = (rule_body_t){ .message_id = 0x1806, .function_code = 5, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 160, .val_type = STRING , .value = "ISL" } } };
	probe3_subrules[1] = (subrule_t){ .header = (header_t){ .start_state = 0, .action = { .value = 68 } }, .rule_id = 1, .rule_body = (rule_body_t){ .message_id = 0x1806, .function_code = 5, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 160, .val_type = STRING , .value = "CI" } } };
	probe3_subrules[2] = (subrule_t){ .header = (header_t){ .start_state = 0, .action = { .value = 1 } }, .rule_id = 2, .rule_body = (rule_body_t){ .message_id = 0x188C, .function_code = 5, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 512, .val_type = STRING , .value = "/cf/arducam.so" } } };
	probe3_subrules[3] = (subrule_t){ .header = (header_t){ .start_state = 1, .action = { .value = 0 } }, .rule_id = 2, .rule_body = (rule_body_t){ .message_id = 0x18B3, .function_code = 36, .rule_parameters = (rule_parameters_t){ .offset = 96, .length = 80, .val_type = STRING , .value = "arducam.so" } } };
	probe3_subrules[4] = (subrule_t){ .header = (header_t){ .start_state = 1, .action = { .value = 68 } }, .rule_id = 2, .rule_body = (rule_body_t){ .message_id = 0x1806, .function_code = 5, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 160, .val_type = STRING , .value = "CAM" } } };
	probe3_subrules[5] = (subrule_t){ .header = (header_t){ .start_state = 1, .action = { .value = 68 } }, .rule_id = 2, .rule_body = (rule_body_t){ .message_id = 0x1806, .function_code = 6, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 160, .val_type = STRING , .value = "CAM" } } };
	probe3_subrules[6] = (subrule_t){ .header = (header_t){ .start_state = 0, .action = { .value = 68 } }, .rule_id = 3, .rule_body = (rule_body_t){ .message_id = 0x188C, .function_code = 7, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 512, .val_type = STRING , .value = "/cf" } } };
	probe3_subrules[7] = (subrule_t){ .header = (header_t){ .start_state = 0, .action = { .value = 1 } }, .rule_id = 4, .rule_body = (rule_body_t){ .message_id = 0x18C8, .function_code = 12, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 0, .val_type = STRING , .value = NULL } } };
	probe3_subrules[8] = (subrule_t){ .header = (header_t){ .start_state = 1, .action = { .value = 0 } }, .rule_id = 4, .rule_body = (rule_body_t){ .message_id = 0x0808, .function_code = 0, .rule_parameters = (rule_parameters_t){ .offset = 256, .length = 976, .val_type = STRING , .value = "CAM EXP3 Complete" } } };
	probe3_subrules[9] = (subrule_t){ .header = (header_t){ .start_state = 1, .action = { .value = 68 } }, .rule_id = 4, .rule_body = (rule_body_t){ .message_id = 0x1806, .function_code = 6, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 160, .val_type = STRING , .value = "CAM" } } };
	probe3_subrules[10] = (subrule_t){ .header = (header_t){ .start_state = 1, .action = { .value = 68 } }, .rule_id = 4, .rule_body = (rule_body_t){ .message_id = 0x1806, .function_code = 5, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 160, .val_type = STRING , .value = "CAM" } } };
	probe3_subrules[11] = (subrule_t){ .header = (header_t){ .start_state = 0, .action = { .value = 1 } }, .rule_id = 5, .rule_body = (rule_body_t){ .message_id = 0x1940, .function_code = 2, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 8, .val_type = INT , .value = "\x00" } } };
	probe3_subrules[12] = (subrule_t){ .header = (header_t){ .start_state = 1, .action = { .value = 2 } }, .rule_id = 5, .rule_body = (rule_body_t){ .message_id = 0x1992, .function_code = 3, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 0, .val_type = INT , .value = NULL } } };
	probe3_subrules[13] = (subrule_t){ .header = (header_t){ .start_state = 2, .action = { .value = 0 } }, .rule_id = 5, .rule_body = (rule_body_t){ .message_id = 0x1940, .function_code = 2, .rule_parameters = (rule_parameters_t){ .offset = 0, .length = 8, .val_type = INT , .value = "\x03" } } };
    
    probe3_rules[0] = (rule_t){ .holder=0, .state_subrules=&probe3_subrules[0], .subrule_number=probe3_subrule_array0 };
	probe3_rules[1] = (rule_t){ .holder=0, .state_subrules=&probe3_subrules[1], .subrule_number=probe3_subrule_array1 };
	probe3_rules[2] = (rule_t){ .holder=0, .state_subrules=&probe3_subrules[2], .subrule_number=probe3_subrule_array2 };
	probe3_rules[3] = (rule_t){ .holder=0, .state_subrules=&probe3_subrules[6], .subrule_number=probe3_subrule_array3 };
	probe3_rules[4] = (rule_t){ .holder=0, .state_subrules=&probe3_subrules[7], .subrule_number=probe3_subrule_array4 };
	probe3_rules[5] = (rule_t){ .holder=0, .state_subrules=&probe3_subrules[11], .subrule_number=probe3_subrule_array5 };
}
