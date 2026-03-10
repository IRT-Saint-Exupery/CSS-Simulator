/*
 * This file includes all useful methods to write rules in a CSV format.
 */

#pragma once

#include <stdio.h>
#include "rules.h"

/*
 * Creates a CSV file. After calling this, all functions of this file
 * are callable. 
 * 
 * This returns 1 in case of error
 */
int create_csv(void);

/*
 * Writes a rule in the CSV file. This requires an existing CSV file.
 */
void write_rule(subrule_t *, value_type_t value_type);

/*
 * Closes the CSV file. This requires an existing CSV file.
 */
void close_csv(void);
