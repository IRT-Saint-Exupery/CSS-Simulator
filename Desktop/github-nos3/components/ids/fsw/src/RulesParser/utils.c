#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

char *infile_name = NULL;
unsigned int current_line = 1;
unsigned int start_location = 0;
unsigned int end_location = 0;

FILE* open_file(char *name)
{
    if (name != NULL) 
    {
        infile_name = name;
        return fopen(name, "r");
    }

    return NULL;
}

void increment_line(void) 
{
    current_line++;
    start_location = 0;
    end_location = 0;
}

unsigned int get_line(void) 
{
    return current_line;
}

void start_token(void)
{
    start_location = end_location;
}

void increment_location(unsigned int size)
{
    end_location += size;
}

unsigned int get_start_location(void)
{
    return start_location;
}

unsigned int get_end_location(void)
{
    return end_location;
}

void print_file_line()
{
    if (infile_name != NULL)
    {
        FILE *file = fopen(infile_name, "r");
        if (file != NULL)
        {
            char *line = NULL;
            size_t len = 0;
            unsigned int line_number = 0; // to still catch the first line
            
            // Skip unused lines
            while (line_number != current_line) 
            {
                getline(&line, &len, file);
                line_number++;
            }
            
            printf("%s", line);

            // If last char is not a line jump, then line jump
            if (line[strlen(line) - 1] != '\n')
            {
                printf("\n");
            }

            fclose(file);
        }
    }
}