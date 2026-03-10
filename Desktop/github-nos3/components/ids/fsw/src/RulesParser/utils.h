/*
 * This file gives useful tools to manage locations in parsing: 
 * - Keep track of the file's current line
 * - Keep track of the read token's position
 */

#pragma once
#include <stdio.h>

/*
 * Opens the file specified by its name and returns the file pointer, or NULL
 * if an error occurs. This also stores the file name to get the file line in
 * case of error. 
 */
FILE* open_file(char* name);

/*
 * Increments the current line. The current line is used in print_file_line and
 * in get_line.
 */
void increment_line(void);

/*
 * Returns the current line. 
 */
unsigned int get_line(void);

/*
 * Starts the token. This is used to keep track of thd location of a token. 
 * Starting the token will in fact move the token's end location to the token's
 * start location. To increment the token's end location, use increment_location. 
 */
void start_token(void);

/*
 * Increments the token's location. This is used to keep track of the location
 * of a token. Incrementing the location will in fact increment the token's end 
 * location. To set the token's start location to the current location, use 
 * start_token. 
 */
void increment_location(unsigned int);

/*
 * Returns the token's start location
 */
unsigned int get_start_location(void);

/*
 * Returns the token's end location.
 */
unsigned int get_end_location(void);

/*
 * Prints the current file line. This requires the call of open_file before hand. 
 */
void print_file_line();