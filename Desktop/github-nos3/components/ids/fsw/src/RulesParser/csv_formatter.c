#include <stdio.h>
#include <stdlib.h>
#include "csv_formatter.h"
#include "rules.h"

#define CSV_FILE_NAME "./rules.csv"
#define CSV_HEADER "Rule id,Start_state,End_state,Message id,Function code,Offset,Type,Length,Value\n"
#define CSV_LINE_FORMAT "%d,%d,%d,%d,%d,%d,%d,%d,%s\n"
#define NULL_STR ""

FILE *csv_file = NULL;

int create_csv(void)
{
    // If the file exists, do nothing
    if (csv_file == NULL)
    {
        csv_file = fopen(CSV_FILE_NAME, "w");
        if (csv_file == NULL)
        {
            return 1;
        }

        // Write header
        fprintf(csv_file, CSV_HEADER);
    }
    
    return 0;
}

void write_rule(subrule_t *subrule, value_type_t value_type)
{
    rule_body_t rule_body = subrule->rule_body;
    rule_parameters_t rule_parameters = rule_body.rule_parameters;

    fprintf(csv_file, CSV_LINE_FORMAT, 
        // Subrule header
        subrule->rule_id, 
        subrule->header.start_state, 
        subrule->header.action.value,
        // Subrule body
        rule_body.message_id, 
        rule_body.function_code,
        // Subrule parameters
        rule_parameters.offset,
        value_type,
        rule_parameters.length, 
        rule_parameters.value != NULL ? rule_parameters.value : NULL_STR);
}

void close_csv(void)
{
    if (csv_file != NULL)
    {
        fclose(csv_file);
        csv_file = NULL;
    }
}
