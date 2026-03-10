/*
 * This file defines the structures used to store the rules parsed by the parser. 
 */

#pragma once

#include <stdint.h>

/* The string representation of the ALERT value */
#define ALLOW_STR "A"
/* The string representation of the DROP value */
#define DENY_STR "D"

/* The string representation of the integer type */
#define INT_STR "INT"
/* The string representation of the string type */
#define STRING_STR "STRING"

/* The type state_t hold the state's state number */
typedef uint8_t state_t;

/* 
 * The measure type holds an action performed by a stateless rule or by the action
 * (sometimes) in a stateful rule. There are two different measures: ALERT and DROP. 
 */
typedef enum measure 
{
    ALERT = 'A', DROP = 'D'
} measure_t;

/*
 * An action, in a stateful rule, is the right operand of the stateful header 
 * (state|action ...). An action is either a measure or a state.
 */
typedef union action
{
    measure_t measure;
    state_t state;
    uint8_t value;
} action_t;

/*
 * The value type holds the parameter value's type. There are two different types:
 * INT and STRING.
 */
typedef enum value_type
{
    INT = 0, STRING = 1
} /*__attribute__((packed))*/ value_type_t;

/*
 * The rule parameters type holds all the rule parameters' values. It holds the 
 * offset, the value type, the length and the parameter's value. Because a rule can 
 * exist without parameters, this type includes a is_present field to specify if 
 * the values inside the type are coherent.  
 */
typedef struct rule_parameters
{
    const char *value; // 8
    value_type_t val_type; //4
    uint16_t offset; // 2
    uint16_t length; // 2
} rule_parameters_t;

/*
 * The rule body type is a type that is present in both stateless and stateful 
 * (sub)rules. It holds the message id, the function code and the parameters of
 * the (sub)rule. 
 */
//typedef union rule_body
typedef struct rule_body
{
    /*struct
    {
        union 
        {
            struct
            {
                uint16_t message_id;           // 2
                uint16_t function_code;        // 2
            } __attribute__((packed));
    
            uint32_t raw_body;
        };
    
        rule_parameters_t rule_parameters; // 16
    } __attribute__((packed));*/

    /*struct 
    {
        uint64_t long_raw[2]; //16
        uint32_t int_raw; //4
    };*/

    //uint8_t raw[20];

    uint16_t message_id;           // 2
    uint16_t function_code;        // 2
    rule_parameters_t rule_parameters; // 16
} rule_body_t;

/*
 * The header type represents the header for a subrule (i.e. state|action). 
 * It holds the rule's start state and its action if the rule matches. For 
 * stateless rules, there is only one state (0) and the action is always a
 * measure.
 */
//typedef union header
typedef struct header
{
    /*struct
    {
        state_t start_state; // 1
        action_t action;     // 1
    } __attribute__((packed));

    uint16_t raw;*/

    state_t start_state; // 1
    action_t action; // 1

} header_t;

/*
 * The subrule type represents either a stateless rule, or the part of a 
 * stateful rule (i.e. subrule ; subrule ; ...). It holds the stateful
 * header and the rule body. 
 */
//typedef union subrule
typedef struct subrule
{
    /*struct
    {
        union
        {
            struct
            {
                uint16_t rule_id;       // 2
                header_t header;        // 2
            } __attribute__((packed));

            uint32_t raw_rule_header;
        };
        
        rule_body_t rule_body;  // 20
    } __attribute__((packed));

    struct
    {
        uint8_t raw_body[16];  // 16
        char *value;           // 8
    } __attribute__((packed));

    uint32_t raw[6];*/

    uint16_t rule_id;       // 2
    header_t header;        // 2
    rule_body_t rule_body;  // 20

} subrule_t;

/*
 * The rule type represents a rule. It holds the current state, all 
 * subrules and the transitions per state.
 */
typedef struct rule
{
    uint8_t holder;
    subrule_t *state_subrules;
    uint8_t *subrule_number;
} rule_t;

/*
 * The node data type holds the rule and its statefulness to store it in 
 * the rule list.
 */
typedef struct rule_data
{
    value_type_t value_type;
    subrule_t subrule;
} rule_data_t;

/*
 * Creates a rule body, this needs to be called after create_rule_parameters.
 */
void create_rule_body(unsigned int, unsigned int);

/* 
 * Creates rule parameters. It will duplicate the char * and will be automatically 
 * freed when the attached rule will be removed. 
 */
void create_rule_parameters(unsigned int, unsigned int, char*);

/*
 * Create a stateful header for a stateful subrule
 */
void create_stateful_header(state_t, action_t);

/*
 * Sets the parameter value type 
 */
void set_parameter_value_type(value_type_t);

/*
 * Returns the parameter value type
 */
value_type_t get_parameter_value_type(void);

/*
 * Adds a stateless rule. This needs to be called after create_rule_body. This
 * rule will be added to a rule list. You can access this list using the get_rule
 * and remove_rule. 
 */
void add_stateless_rule(measure_t);

/*
 * Adds a subrule for the current stateful rule.
 */
void add_subrule(void);

/*
 * Adds a stateful rule to the current list of rules. A stateful rule is composed 
 * of stateful subrules. A subrule is for example E0|D 0x0821 10, and subrules are 
 * separed by semicolons. 
 */
void add_stateful_rule(void);

/*
 * Prints all added rules.
 */
void print_rules(void);

/*
 * Returns the number of stored rules.
 */
int get_number_of_rules(void);

/*
 * Gets the rule at the specified index.
 */
rule_data_t* get_rule(int);

/*
 * Removes the rule at the specified index.
 */
int remove_rule(int);
