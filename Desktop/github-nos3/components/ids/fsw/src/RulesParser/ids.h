#pragma once

#include <stdint.h>

/*
 * The performed action by the IDS
 */
typedef enum ids_action
{
    SWITCH_STATE, ALERT_ACTION, DROP_ACTION, NOTHING
} ids_action_t;

/*
 * The return type of the check packet function. It contains useful informtion for telemetry.
 */
typedef struct ids_return
{
    ids_action_t ids_action;
    uint8_t rule_number;
} ids_return_t;

/*
 * Checks a packet with a specified set of rules
 */
ids_return_t check_packet(rule_t *rules, int number_rules, size_t buf_len, uint8_t *buf);
