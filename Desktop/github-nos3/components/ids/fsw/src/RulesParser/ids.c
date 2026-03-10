#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "rules.h"
#include "ids.h"

#define MESSAGE_ID_HIGH_INDEX 0
#define MESSAGE_ID_LOW_INDEX 1
#define FUNCTION_CODE_INDEX 6

uint8_t rule_number = 0;

void alert()
{
    printf("ALERT (rule %d)\n\n", rule_number);
}

void drop()
{
    printf("DROP (rule %d)\n\n", rule_number);
}

ids_action_t act(rule_t *rule, subrule_t *subrule)
{
    // From here, we need to act so either switch state or take a measure
    action_t action = subrule->header.action;
    ids_action_t ids_action;
    if (action.measure == ALERT)
    {
        // We alert
        alert();
        ids_action = ALERT_ACTION;
    } else if (action.measure == DROP) 
    {
        // We drop the package
        drop();
        ids_action = DROP_ACTION;
    } else 
    {
        // We switch state
        printf("Switching to state %d (rule %d)\n\n", action.state, rule_number);
        rule->holder = action.state;
        ids_action = SWITCH_STATE;
    }

    return ids_action;
}

/*
 * This will create the rule value.
 * It assumes that the packet has a valid length.
 * 
 * THIS METHOD USES MALLOC, DON'T FORGET TO FREE AFTERWARDS.
 */
uint8_t* create_rule_value(uint8_t *value, uint16_t length)
{
    uint8_t *rule_value = calloc(length, sizeof(uint8_t));

    // Compute the length of the memory to copy (the value can be smaller)
    size_t value_length = strlen(value);
    memcpy(rule_value, value, value_length);

    return rule_value;
}

/*
 * This will create the packet value from the rule's offset and length.
 * 
 * THIS METHOD USES MALLOC, DON'T FORGET TO FREE AFTERWARDS.
 */
uint8_t* create_packet_value(uint8_t *value, uint16_t offset, uint16_t length)
{
    uint8_t *packet_value = calloc(length, sizeof(uint8_t));
    memcpy(packet_value, value + offset, length);

    return packet_value;
} 

ids_action_t check_rule(rule_t *rule, uint16_t message_id, uint16_t function_code, size_t buf_len, uint8_t *buf)
{
    // Get the subrules corresponding to the rule state holder
    subrule_t *subrules = rule->state_subrules;
    for (int i = 0; i < rule->holder; i++)
    {
        subrules += rule->subrule_number[i];
    }
    uint8_t subrule_number = rule->subrule_number[rule->holder];

    // For each rule, check if the message id and the function code corresponds
    for (int i = 0; i < subrule_number; i++)
    {
        if (subrules->rule_body.message_id == message_id 
            && subrules->rule_body.function_code == function_code)
        {
            // If the rule has no parameters, then act
            if (subrules->rule_body.rule_parameters.value == NULL)
            {
                return act(rule, subrules);
            } else
            {
                // Only check if length + offset is smaller than the packet size. 
                uint16_t offset = subrules->rule_body.rule_parameters.offset >> 3; // Offset is in bits
                uint16_t length = subrules->rule_body.rule_parameters.length >> 3; // Length is in bits
                if (length != 0 && offset + length <= buf_len)
                {
                    // Extract and format values
                    uint8_t *rule_value = create_rule_value((uint8_t *)subrules->rule_body.rule_parameters.value, length);
                    uint8_t *packet_value = create_packet_value(buf, offset, length);
    
                    // Compare byte by byte
                    uint8_t is_different = 0;
                    uint8_t index = 0;
                    do
                    {
                        is_different = rule_value[index] - packet_value[index];
                        index++;
                    } while(!is_different && index < length);
    
                    // Free the values
                    free(rule_value);
                    free(packet_value);
    
                    // If there is no difference then act
                    if (!is_different)
                    {
                        return act(rule, subrules);
                    }
                }
            }
        }

        // Increment subrule
        subrules += 1;
    }

    return NOTHING;
}

ids_return_t check_packet(rule_t *rules, int number_rules, size_t buf_len, uint8_t *buf)
{
    uint32_t message_id = (buf[MESSAGE_ID_HIGH_INDEX] << 8) | buf[MESSAGE_ID_LOW_INDEX];
    uint16_t function_code = buf[FUNCTION_CODE_INDEX];

    ids_action_t ids_action = NOTHING;
    for (int i = 0; i < number_rules; i++)
    {
        rule_number = i;
        ids_action = check_rule(&rules[i], message_id, function_code, buf_len, buf);

        // If the action is not NOTHING, then break
        if (ids_action != NOTHING)
        {
            break;
        }
    }

    if (ids_action == NOTHING)
    {
        printf("No action\n\n");
    }

    ids_return_t ids_return = {.ids_action = ids_action, .rule_number = rule_number};
    return ids_return;
}

/*    FOR TESTING    */
/* Created with "make launch RULES=./tests/valid_input1.rules PREFIX=probe3 OUTPUT=probe3" */

// #include "probe3.h"
// int main()
// {
//     probe3_init();

//     // Won't trigger anything
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 30, "        AAAAAAAAAAAAAAAAAA   ");
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 31, "\x18\x8C    \x05  AAAAAAAAAAAAAAAAAA   ");
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 31, "\x18\x8C    \x05   /cf/arducam.so   ");

//     // Switch to E1 for rule 3, 5, 4
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 30, "\x18\x8C    \x05  /cf/arducam.so   ");
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 9, "\x19\x40    \x02\x00");
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 8, "\x18\xC8    \x0C");

//     // Drop for rule 3
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 31, "\x18\x06    \x05  AAACAMAAAAAAAAAAAAAAA   ");

//     // Switch to E2 for rule 5
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 8, "\x19\x92    \x03");

//     // Alert for rule 8
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 40, "\x08\x08    \x00                         INT");

//     // Switch to E0 for rule 5, 3
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 8, "\x19\x40    \x02\x03");
//     check_packet(probe3_rules, PROBE3_RULE_NUMBER, 30, "\x18\xB3    \x02  /cf/arducam.so   ");

//     return 0;
// }