/*
 * The file contains functions that can be used for error reporting during
 * the parsing. Use throw_error to throw an error. If the message needs to
 * be formatted, use throw_error_format (or throw_int_error_format if the
 * format is an int).
 */
#pragma once

#define SEMICOLON_ERROR_MESSAGE "Stateful rules must be separated by a semicolon (;)"
#define COLON_ERROR_MESSAGE "Parameters must be separated by a colon (:)"

#define MEASURE_ERROR_FORMAT "Measure must be either A or D, but got <%s>"
#define STATE_ERROR_FORMAT "State must be of the form E[0-9], but got <%s>"
#define ACTION_ERROR_FORMAT "Action must be a state (E[0-9]) or a measure (A or D), but got <%s>"
#define MESSAGE_ID_SIZE_ERROR_MESSAGE "The message id's size must be 2 bytes (uint16)"
#define MESSAGE_ID_BYTE_ERROR_FORMAT "The message id's upper byte must be 0x08, 0x09, 0x18, or 0x19, but got <%d>"
#define MESSAGE_ID_ERROR_MESSAGE "The message id must be a number of the form 0x08XX, 0x09XX, 0x18XX, or 0x19XX"
#define FUNCTION_CODE_ERROR_MESSAGE "The function code must be a number"
#define OFFSET_ERROR_MESSAGE "The offset code must be a number"
#define TYPE_ERROR_FORMAT "The type must be either INT or STRING, but got <%s>"
#define TYPE_ERROR_MESSAGE "The type must be a string, either INT or STRING"
#define LENGTH_ERROR_MESSAGE "The length must be a number"
#define VALUE_TYPE_GOT_STRING_ERROR_FORMAT "The value type has been set to INT but got <%s>: a string value"
#define VALUE_TYPE_GOT_INT_ERROR_FORMAT "The value type has been set to STRING but got <%d>: an integer value"
#define VALUE_ERROR_MESSAGE "The value must be a number or a string"

/*
 * This function throws an error and tries to print the line that caused 
 * that error alongside with the faulty token.
 */
void throw_error(char *);

/*
 * Same as throw_error, but formats the message before showing it.
 */
void throw_error_format(char *, char *);

/*
 * Same as throw_error_format, but with an int format.
 */
void throw_int_error_format(char*, unsigned int);