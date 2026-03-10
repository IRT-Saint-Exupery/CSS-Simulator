#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rules.h"
#include "list.h"

void remove_rule_handler(void *user_data)
{
    rule_data_t *node_data = (rule_data_t *) user_data;
    subrule_t subrule = node_data->subrule;
    if (subrule.rule_body.rule_parameters.value == NULL)
    {
        free(subrule.rule_body.rule_parameters.value);
    }

    free(node_data);
}

list_t rule_list = {.list_size = 0, .first_node = NULL, .last_node = NULL, .remove_handler = &remove_rule_handler};

unsigned int rule_id;

rule_body_t rule_body;
rule_parameters_t rule_parameters;
header_t header;
value_type_t value_type;

void create_rule_body(unsigned int message_id, unsigned int function_code)
{
    //printf("creating rule body\n");
    rule_body.message_id = message_id;
    rule_body.function_code = function_code;
    rule_body.rule_parameters = rule_parameters;
    //printf("created rule body\n");
}

void create_rule_parameters(unsigned int offset, unsigned int length, char* value)
{
    //printf("creating rule parameters\n");
    rule_parameters.offset = offset;
    rule_parameters.length = length;
    //rule_parameters.val_type = 
    rule_parameters.value = strdup(value);
    //printf("created rule parameters\n");
}

void create_stateful_header(state_t start_state, action_t action)
{
    //printf("creating stateful header\n");
    header.start_state = start_state;
    header.action = action;
    //printf("created stateful header\n");
}

void set_parameter_value_type(value_type_t type)
{
    //printf("setting value type\n");
    value_type = type;
    //printf("set value type\n");
}

value_type_t get_parameter_value_type(void)
{
    return value_type;
}

void add_subrule()
{
    //printf("adding subrule\n");
    rule_data_t *node_data = malloc(sizeof(rule_data_t));
    subrule_t *subrule = &node_data->subrule;
    node_data->value_type = value_type;

    subrule->rule_id = rule_id;
    subrule->header = header;
    subrule->rule_body = rule_body;
    add_node(&rule_list, node_data);
    //printf("added node\n");
    // Reset parameters
    //rule_parameters.int_raw[0] = 0;
    //rule_parameters.int_raw[1] = 0;
    //rule_parameters.int_raw[2] = 0;
    //rule_parameters.char_raw = 0;

    rule_parameters.length = 0;
    rule_parameters.offset = 0;
    rule_parameters.val_type = 0;
    rule_parameters.value = 0;

    // Reset rule body
    //rule_body.long_raw[0] = 0;
    //rule_body.long_raw[1] = 0;
    //rule_body.int_raw = 0;

    rule_body.function_code = 0;
    rule_body.message_id = 0;
    rule_body.rule_parameters =(rule_parameters_t) {0};

    //printf("added subrule\n");
}

void add_stateless_rule(measure_t measure)
{
    create_stateful_header(0, (action_t) measure);
    add_subrule();

    // Increment rule id
    rule_id++;
}

void add_stateful_rule()
{
    //printf("adding stateful rule\n");
    // Increment rule id
    rule_id++;
    //printf("added stateful rule\n");
}

void print_rule_parameters(rule_parameters_t rule_parameters, value_type_t value_type)
{
    //printf("printing rule parameters\n");
    //printf("rule_parameters.value = %X\n",rule_parameters.value);
    //printf("sizeof val_type : %ld\n", sizeof(rule_parameters.val_type));
    //printf("sizeof rule parameters struct : %ld\n", sizeof(rule_parameters_t));
    //printf("sizeof new rule parameters struct : %ld\n", sizeof(rule_parameters_2t));

    //printf("val_type adress : %X, value adress : %X\n",&(rule_parameters.val_type),rule_parameters.value);

    /*if (rule_parameters.value == NULL)
        printf("PAF\n");

    if (rule_parameters.value != NULL)
        printf("POUF\n");*/
    //printf("0 != NULL : %d\n", 0 != NULL);
    if (rule_parameters.value != NULL)
    {
        //printf("in if\n");
        printf(" %d:%s:%d %s", rule_parameters.offset, value_type == INT ? "INT" : "STRING", rule_parameters.length, rule_parameters.value);
    }
    //printf("printed rule parameters\n");
}

void print_rule_body(rule_body_t rule_body)
{
    //printf("printing rule body\n");
    printf(" %04X %d", rule_body.message_id, rule_body.function_code);
    //printf("printed rule body\n");
}

void print_header(header_t header)
{
    //printf("printing header\n");
    printf("E%d|", header.start_state);
    if (header.action.measure == ALERT || header.action.measure == DROP)
    {
        printf("%s", header.action.measure == ALERT ? ALLOW_STR : DENY_STR);
    } else 
    {
        printf("E%d", header.action.state);
    }
    //printf("printed header\n");
}

void print_rules(void)
{
    //printf("printing rules\n");
    if (rule_list.list_size == 0)
    {
        printf("Empty rule list\n");
    } else
    {
        printf("List size: %d\n", rule_list.list_size);
        for (int i = 0; i < rule_list.list_size; i++)
        {
            rule_data_t *node_data = get(&rule_list, i);
            subrule_t subrule = node_data->subrule;
            printf("(%d) -> ", subrule.rule_id);
            print_header(subrule.header);
            print_rule_body(subrule.rule_body);
            print_rule_parameters(subrule.rule_body.rule_parameters, node_data->value_type);
            
            printf("\n");
        }
    }
    //printf("printed rules\n");
}

int get_number_of_rules(void)
{
    //printf("getting number of rules\n");
    return rule_list.list_size;
    //printf("got number of rules\n");
}

rule_data_t* get_rule(int index)
{
    //printf("getting rule\n");
    return get(&rule_list, index);
    //printf("got rule\n");
}

int remove_rule(int index)
{
    //printf("removing rule\n");
    return remove_node(&rule_list, index);
    //printf("removed rule\n");
}
