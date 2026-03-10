#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "utils.h"
#include "parser.tab.h"

void print_error_token()
{
    print_file_line();

    // Print ^ on the detected token
    unsigned int start = get_start_location();
    unsigned int end = get_end_location();

    for (unsigned int i = 0; i < end; i++)
    {
        if (i < start)
        {
            printf(" ");
        } else
        {
            printf("^");
        }
    }
    printf("\n");
}

void throw_error(char* error_message)
{
    yyerror(error_message);

    // Print line with detected error
    print_error_token();
}

void throw_error_format(char* error_format, char *yytext) {
    int error_length = strlen(error_format);
    int other_len = strlen(yytext);
    char* error_msg = malloc(sizeof(char) * (other_len + error_length));
    sprintf(error_msg, error_format, yytext);

    // Print error
    throw_error(error_msg);
    free(error_msg);
}

void throw_int_error_format(char* error_format, unsigned int number) {
    int error_length = strlen(error_format);
    int other_len = 12;
    char* error_msg = malloc(sizeof(char) * (other_len + error_length));
    sprintf(error_msg, error_format, number);

    throw_error(error_msg);
    free(error_msg);
}