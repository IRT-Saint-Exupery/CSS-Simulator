#include "ids_analyzer.h"

char analyzer_check_packet(rule_t * rules, uint8_t *packet_content, size_t content_size, uint16_t msgId, uint16_t fc, int8_t *alert_rule_nb, uint8_t module_number, cond_processing_fct_ptr cond_processing_function, uint16_t probe_rules_nb)
{
	uint8_t *param_ptr;
	int offset;
	char rule_matched = 0;
	char raise_alert = 0;
	char is_accepted_packet = true;

	for(int i = 0; i<probe_rules_nb; i++)
	{
		subrule_t *sub_rule = rules[i].state_subrules;
		for(int j = 0; j<rules[i].holder; j++)
			sub_rule += rules[i].subrule_number[j]; //allows to shift directly to the subrules corresponding to the current holder value and only consider those

		uint8_t subrule_number = rules[i].subrule_number[rules[i].holder];

		for(int j = 0; j<subrule_number; j++)
		{
			unsigned int is_tlm_packet = !(sub_rule[j].rule_body.message_id & 0x1000); // if we have 0x0XXX this is true, if 0x1XXX this is false
			
			//printf("msgid : %02x, rule msgid : %02x, fc : %d, rule fc : %d\n", msgId, bus_rules.msgid_tbl[i], fc, bus_rules.fc_tbl[i]);
			if ((sub_rule[j].rule_body.message_id == msgId) && ((is_tlm_packet) || (sub_rule[j].rule_body.function_code == fc))) // TODO - This is_tlm_packet seems quite suspicious, update !!!!
			{
				//printf("possible match for rule %d : %04X, %d (is_tlm : %d)\n",i,msgId,fc, is_tlm_packet);
				//printf("holder value : %d, rule %d\n", rules[i].holder, i);

				// if rule has no params
				if (sub_rule[j].rule_body.rule_parameters.value==NULL)
				{
					cond_processing_function(&sub_rule[j], &rules[i], i, msgId, fc, NULL, &is_accepted_packet, &raise_alert, &rule_matched);
				}
				else if (sub_rule[j].rule_body.rule_parameters.val_type == STRING)
				{
					offset = sub_rule[j].rule_body.rule_parameters.offset/8;
					
					//if the packet is a command (0x1XXX)
					if (!is_tlm_packet)
						param_ptr = &(packet_content[8+offset]); // 8 bytes for the CCSDS CMD SPP Header
						// TODO - add checks to see if '\0' is present or if big trouble incoming
					else
						param_ptr = &(packet_content[16+offset]); // 16 bytes for the CCSDS TLM SPP Header
						// TODO - add checks to see if '\0' is present or if big trouble incoming
					
					//printf("param expected : %s, actual param : %s\n", sub_rule[j].rule_body.rule_parameters.value, param_ptr);
					//printf("strncmp result : %d\n\n",strncmp(sub_rule[j].rule_body.rule_parameters.value,(char *)param_ptr, sub_rule[j].rule_body.rule_parameters.length/8));
					
					if (strncmp(sub_rule[j].rule_body.rule_parameters.value,(char *)param_ptr, sub_rule[j].rule_body.rule_parameters.length/8)==0) //should use total length but maybe better to use relative length of val ??? - TODO
					{
						cond_processing_function(&sub_rule[j], &rules[i], i, msgId, fc, param_ptr, &is_accepted_packet, &raise_alert, &rule_matched);
					}
					//TODO - take into account non-string parameters and implement comparison
					//else keep testing for other rules
				}
				else //INT ! ONLY 8 BITS SUPPORTED, NEED TO EDIT !
				{
					offset = sub_rule[j].rule_body.rule_parameters.offset/8;
					
					//if the packet is a command (0x1XXX)
					if (!is_tlm_packet)
						param_ptr = &(packet_content[8+offset]); // 8 bytes for the CCSDS CMD SPP Header
					else
						param_ptr = &(packet_content[16+offset]); // 16 bytes for the CCSDS TLM SPP Header
					
					//printf("memcmp : %d\n", memcmp(sub_rule[j].rule_body.rule_parameters.value, param_ptr, sub_rule[j].rule_body.rule_parameters.length/8));
					//printf("expected value : %d, got : %d.\n", (int)*(sub_rule[j].rule_body.rule_parameters.value), (int)*param_ptr);
					/*printf("expected value : %d:%d:%d:%d, but got : %d:%d:%d:%d.\n", sub_rule[j].rule_body.rule_parameters.value[0], \
						sub_rule[j].rule_body.rule_parameters.value[1], sub_rule[j].rule_body.rule_parameters.value[2], \
						sub_rule[j].rule_body.rule_parameters.value[3], param_ptr[0], param_ptr[1], param_ptr[2], param_ptr[3]);*/

					if (!memcmp(sub_rule[j].rule_body.rule_parameters.value, param_ptr, sub_rule[j].rule_body.rule_parameters.length/8)) //if param is the expected value
					{
						cond_processing_function(&sub_rule[j], &rules[i], i, msgId, fc, param_ptr, &is_accepted_packet, &raise_alert, &rule_matched);
					}
				}
				//TODO - ADD SUPPORT FOR FLOATS ?

				if(rule_matched)
				{
					if (raise_alert)
					{
						#ifdef IDS_TELEM_ENABLED
						IDS_RaiseAlert(module_number, (uint8_t)i, (uint16_t)content_size, 0, packet_content, NULL);
						#endif
						*alert_rule_nb = i;
					}
					break;
				}
			}
		}
		if (rule_matched)
		{
			break;
		}
	}

	return is_accepted_packet;
}