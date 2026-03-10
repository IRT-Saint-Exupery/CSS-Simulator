%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "utils.h"
  #include "errors.h"
  #include "csv_formatter.h"
//   #include "parser.tab.h"
//   #define YYLTYPE 1
  extern FILE *yyin;
%}

//%define parse.error detailed
%define parse.error verbose
%define parse.lac full 

%code requires {
    #include "rules.h"
}

%code provides {
    // #define YYDEBUG 1
    int yylex(void);
    void yyerror(const char *);
    // location_t yylloc;
    
}

%union {
    int nb;
    char *value;
    value_type_t value_type;
}

%token tPIPE tCOLON tSEMICOLON tLINE_JUMP
%token<nb> tNUM
%token<value> tSTRING
%token tOTHER // Unused but here to set an error

%type<nb> state message_id function_code offset length measure action
%type<value> value
%type<value_type> type

%expect 1

%start file_entry

%%

file_entry:
    rules
    | line_jump rules
    | line_jump
    | %empty

rules: 
    rule line_jump rules
    | rule line_jump
    | rule
    ;

rule:
    stateless_rule
    | stateful_rule
    ;

stateless_rule:
    measure rule_body {add_stateless_rule($1 == 'A' ? ALERT : DROP);}
    ;

stateful_rule:
    stateful_rule_body stateful_rule_separator stateful_rule_following
    ;

stateful_rule_following:
    stateful_rule
    | stateful_rule_body {add_stateful_rule();}
    ;

stateful_rule_body:
    state_description rule_body {add_subrule();}
    ;

state_description:
    state state_separator action {
        action_t action;
        if ($3 == 'A')
        {
            action.measure = ALERT;
        } else if ($3 == 'D')
        {
            action.measure = DROP;
        }
        {
            action.state = $3;
        }
        create_stateful_header($1, action);
    }
    ;

rule_body:
    message_id function_code parameters {create_rule_body($1, $2);}
    | message_id function_code {create_rule_body($1, $2);}
    ;

parameters:
    offset parameters_separator type parameters_separator length value {
        create_rule_parameters($1, $5, $6);
        
        // Create parameters duplicate the value, can free the value
        free($6);
    }
    ;

stateful_rule_separator:
    tSEMICOLON 
    | error {
        throw_error(SEMICOLON_ERROR_MESSAGE); YYABORT;
    }
    ;

parameters_separator:
    tCOLON 
    | error {
        throw_error(COLON_ERROR_MESSAGE); YYABORT;
    }
    ;

state_separator:
    tPIPE
    ;

measure:
    tSTRING {
        if ($1 == NULL) 
        {
            throw_error("Measure is NULL..."); YYABORT;
        }

        if (strlen($1) != 1 || ($1[0] != 'A' && $1[0] != 'D')) 
        {
            throw_error_format(MEASURE_ERROR_FORMAT, $1); YYABORT;
        }

        // From here the format is good
        $$ = $1[0];
        
        // Free the measure
        free($1);
    }

state:
    tSTRING {
        if ($1 == NULL) 
        {
            throw_error("State is NULL..."); YYABORT;
        }

        if (strlen($1) != 2 || $1[0] != 'E' || ($1[1] < '0' && $1[1] > '9')) 
        {
            throw_error_format(STATE_ERROR_FORMAT, $1); YYABORT;
        }

        // From here the format is good
        $$ = $1[1] - '0';

        // Free the state
        free($1);
    }

action: 
    tSTRING {
        if (strlen($1) != 1 || ($1[0] != 'A' && $1[0] != 'D')) 
        {
            // Not a measure, test if state
            if (strlen($1) != 2 || $1[0] != 'E' || ($1[1] < '0' && $1[1] > '9')) 
            {
                // Neither, throw error
                throw_error_format(ACTION_ERROR_FORMAT, $1); YYABORT;
            } else 
            {
                // From here the format for state is good
                $$ = $1[1] - '0';
            }
        } else 
        {
            // From here the format for measure is good
            $$ = $1[0];
        }

        // Free the action
        free($1);
    }
    ;

message_id:
    tNUM {
        if ($1 > 0xFFFF) {
            throw_error(MESSAGE_ID_SIZE_ERROR_MESSAGE); YYABORT;
        }

        int high_byte = $1 & 0xFF00;
        if (high_byte != 0x0800 && high_byte != 0x0900 &&
                high_byte != 0x1800 && high_byte != 0x1900) {
            throw_int_error_format(MESSAGE_ID_BYTE_ERROR_FORMAT, $1); YYABORT;
        }

        // From here the format is good
        $$ = $1;
    }
    | error {
        throw_error(MESSAGE_ID_ERROR_MESSAGE); YYABORT;
    }

function_code:
    tNUM {$$ = $1;}
    | error {
        throw_error(FUNCTION_CODE_ERROR_MESSAGE); YYABORT;
    }
    ;

offset:
    tNUM {$$ = $1;}
    | error {
        throw_error(OFFSET_ERROR_MESSAGE); YYABORT;
    }
    ;

type:
    tSTRING {
        // Check if value is INT or STRING
        int len = strlen($1);
        if (len == 3 && !strncmp("INT", $1, 3))
        {
            set_parameter_value_type(INT);
            $$ = INT;
        } else if (len == 6 && !strncmp("STRING", $1, 6))
        {
            set_parameter_value_type(STRING);
            $$ = STRING;
        } else 
        {
            throw_error_format(TYPE_ERROR_FORMAT, $1); YYABORT;
        }

        free($1);
    }
    | error {
        throw_error(TYPE_ERROR_MESSAGE); YYABORT;
    }
    ;

length:
    tNUM {$$ = $1;}
    | error {
        throw_error(LENGTH_ERROR_MESSAGE); YYABORT;
    }
    ;

value:
    tNUM {
        // Verify if the type is INT
        if (get_parameter_value_type() != INT)
        {
            throw_int_error_format(VALUE_TYPE_GOT_INT_ERROR_FORMAT, $1);
        }

        char *int_str = malloc(sizeof(char) * 12); // max int is 10, include minus sign and \0
        sprintf(int_str, "%d", $1);
        $$ = int_str;
    }
    | tSTRING {
        // Verify that type is STRING
        if (get_parameter_value_type() != STRING)
        {
            throw_error_format(VALUE_TYPE_GOT_STRING_ERROR_FORMAT, $1);
        }

        // Freed in the "parameters" rule
        $$ = $1;
    }
    | error {
        throw_error(VALUE_ERROR_MESSAGE); YYABORT;
    }
    ;

line_jump:
    tLINE_JUMP line_jump
    | tLINE_JUMP
    ;

%%

void yyerror(const char *msg) {
    if (msg == NULL) {
        printf("[ERROR] - Unexpected error... (NULL error message)\n");
        exit(1);
    } 

    printf("[ERROR] - %s (line %d)...\n", msg, get_line());
}

int main(int argc, char** argv) {
    if (argc != 2)
    {
        printf("[ERROR] - There must be one argument: the name of the file to parse\n");
        exit(1);
    }

    FILE *file = open_file(argv[1]);
    if (file == NULL)
    {
        printf("[ERROR] - There was a problem when opening the file... Does it exist?\n");
        exit(1);
    }

    yyin = file;
    int parse_result = yyparse();

    #ifdef VERBOSE
    // Print parsed rules
    if (!parse_result)
    {
        print_rules();   
    }
    #endif

    // Print rules in another file in a CSV form
    int nb_rules = get_number_of_rules();
    if (nb_rules != 0)
    {
        if (!create_csv())
        {
            for (int i = 0; i < nb_rules; i++)
            {
                rule_data_t *rule_data = get_rule(i);
                write_rule(&rule_data->subrule, rule_data->value_type); 
            }
            close_csv();
        }
    }

    return parse_result;
}