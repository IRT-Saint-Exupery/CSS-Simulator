# Imports
import optparse
import sys

# Classes
class Subrule(object):
    start_state: int
    end_state: int
    message_id: int
    function_code: int
    offset: int
    value_type: int
    length: int
    value: str
    
    
    def __init__(self,
                 start_state: int,
                 end_state: int,
                 message_id: int,
                 function_code: int,
                 offset: int,
                 value_type: int,
                 length: int,
                 value: str) -> None:
        self.start_state = start_state
        self.end_state = end_state
        self.message_id = message_id
        self.function_code = function_code
        self.offset = offset
        self.value_type=value_type
        self.length = length
        self.value = value
        
    
# Constants
DEFAULT_PREFIX: str = ''
DEFAULT_RULE_FILE_NAME: str = 'parsed_rules'

NO_VALUE: str = 'NULL'
CSV_FILE: str = './rules.csv'

INT_TYPE: int = 0
STRING_TYPE: int = 1

# SUBRULE_RULE_FORMAT: str = '{prefix:s}_rule{rule_id:d}_subrule{subrule_id:d}'
# ORIGINAL FORMAT -------!
#SUBRULE_STRUCTURE_FORMAT: str = '{prefix:s}_subrules[{subrule_id:d}] = (subrule_t){{ .raw_body = {{ \
#0x{action:0>2X}{start:0>2X}{rule_id:0>4X}, \
#0x{function_code:0>4X}{message_id:0>4X}, \
#0x{length:0>4X}{offset:0>4X} }}, .value={value:s} }};'
# LOUIS EDIT ------------!
SUBRULE_STRUCTURE_FORMAT: str = '{prefix:s}_subrules[{subrule_id:d}] = (subrule_t){{ \
.header = (header_t){{ .start_state = {start:d}, .action = {{ .value = {action:d} }} }}, .rule_id = {rule_id:d}, \
.rule_body = (rule_body_t){{ .message_id = 0x{message_id:0>4X}, .function_code = {function_code:d}, \
.rule_parameters = (rule_parameters_t){{ .offset = {offset:d}, .length = {length:d}, .val_type = {value_type:s} , .value = {value:s} }} }} }};'
SUBRULE_NUMBER_ARRAY_NAME_FORMAT: str = '{prefix:s}_subrule_array{rule_id:d}'
SUBRULE_NUMBER_ARRAY_FORMAT: str = 'uint8_t {name:s}[{transition_number:d}] = {subrule_number:s};'
RULE_STRUCTURE_FORMAT: str = '{prefix:s}_rules[{rule_id:d}] = (rule_t){{ .holder=0, .state_subrules=&{prefix:s}_subrules[{subrule_start:d}], .subrule_number={subrule_number_name:s} }};'
INCLUDE_FILE_STRUCTURE_FORMAT: str = '''#pragma once

#include "rules.h"

#define {cap_prefix:s}_RULE_NUMBER {rule_number:d}

extern rule_t {prefix:s}_rules[{rule_number:d}];
extern subrule_t {prefix:s}_subrules[{subrule_number:d}];

void {prefix:s}_init(void);
'''
DECL_FILE_STRUCTURE_FORMAT: str = '''#include <stdlib.h>
#include "{include_file_name:s}"
#include "rules.h"

rule_t {prefix:s}_rules[{rule_number:d}];
subrule_t {prefix:s}_subrules[{subrule_number:d}];

{subrule_number_definitions:s}

void {prefix:s}_init(void)
{{
    {subrule_definitions:s}
    
    {rule_definitions:s}
}}
'''

# Variables
rules: dict[int, list[Subrule]] = {}
subrules_str: list[str] = []
subrule_number_str: list[str] = []
rules_str: list[str] = []

# Functions
def add_subrule(line: str) -> None:
    parameters: list[str] = line.split(",")
    rule_id: int = int(parameters[0])
    start_state: int = int(parameters[1])
    end_state: int = int(parameters[2])
    message_id: int = int(parameters[3])
    function_code: int = int(parameters[4])
    offset: int = int(parameters[5])
    value_type: int = int(parameters[6])
    length: int = int(parameters[7])
    value: str = parameters[8].strip()

    subrule: Subrule = Subrule(start_state=start_state,
                               end_state=end_state,
                               message_id=message_id,
                               function_code=function_code,
                               offset=offset,
                               value_type=value_type,
                               length=length,
                               value=value)
    
    if rule_id not in rules:
        rules[rule_id] = []
        
    rules[rule_id].append(subrule)
    

def main(rules_prefix: str, include_file: str, decl_file: str) -> None:
    with open(CSV_FILE, 'r') as file:
        # First line is a header
        file.readline()
        
        line: str = file.readline()
        while line: 
            add_subrule(line=line)            
            line = file.readline()
    
    subrule_count: int = 0
    for rule_id in rules:
        # Sort subrules in each rules
        subrules: list[Subrule] = rules[rule_id]
        subrules.sort(key=lambda subrule: subrule.start_state, reverse=False)
        
        subrules_number_per_start_state: dict[int, int] = {}
        
        subrule_start_index = subrule_count
        subrule_id: int = 0
        for subrule in subrules:
            # Check if value is empty, in this case, put the subrule value to NO_VALUE
            if subrule.value == '':
                subrule.value = NO_VALUE
            
            else:
                value: str = subrule.value
                
                # Check if value is an int. In this case, transform the int to a hex string
                if subrule.value_type == INT_TYPE:
                    value = hex(int(value))[2:] # This must be an int since the parser allowed the value
                    
                    # If value has an odd number of charactrs, add a 0
                    value = ('0' + value) if len(value) & 1 == 1 else value
                    
                    # Split every two characters
                    values: list[str] = [(value[i : i + 2]) for i in range(0, len(value), 2)]
                    
                    print(values)

                    # Store back in value
                    value = f'\\x{'\\x'.join(values)}' #original string
                    
                else :
                    if value[0] == "'" : value = value[1:len(value)-1]

                # Put the string value between double quotes
                subrule.value = f'"{value}"'
            
            # Create structure string
            subrules_str.append(SUBRULE_STRUCTURE_FORMAT.format(prefix=rules_prefix, 
                                                                subrule_id=subrule_count,
                                                                action=subrule.end_state,
                                                                start=subrule.start_state,
                                                                rule_id=rule_id,
                                                                function_code=subrule.function_code,
                                                                message_id=subrule.message_id,
                                                                length=subrule.length,
                                                                offset=subrule.offset,
                                                                value_type= "STRING" if subrule.value_type == STRING_TYPE else "INT",
                                                                value=subrule.value))
            subrule_count += 1
            
            # Put in dict to count the number of transitions per state
            if subrule.start_state not in subrules_number_per_start_state:
                subrules_number_per_start_state[subrule.start_state] = 0
                
            subrules_number_per_start_state[subrule.start_state] += 1        
            subrule_id += 1
        
        # Write the different subrules            
        subrule_number: str = '{ ' + ', '.join(str(i) for i in list(subrules_number_per_start_state.values())) + ' }'
        
        # Create subrule number array
        subrule_number_name = SUBRULE_NUMBER_ARRAY_NAME_FORMAT.format(prefix=rules_prefix,rule_id=rule_id)
        subrule_number_str.append(SUBRULE_NUMBER_ARRAY_FORMAT.format(name=subrule_number_name,
                                                                     transition_number=len(subrules_number_per_start_state),
                                                                     subrule_number=subrule_number))
        
        # Create rule
        rules_str.append(RULE_STRUCTURE_FORMAT.format(prefix=rules_prefix,
                                                      rule_id=rule_id,
                                                      subrule_start=subrule_start_index,
                                                      subrule_number_name=subrule_number_name))
    
    # Print result in files
    with open(include_file, 'w') as file:
        file.write(INCLUDE_FILE_STRUCTURE_FORMAT.format(cap_prefix=rules_prefix.upper(), prefix=rules_prefix, rule_number=len(rules_str), subrule_number=len(subrules_str)))
        
    with open(decl_file, 'w') as file:
        file.write(DECL_FILE_STRUCTURE_FORMAT.format(prefix=rules_prefix,
                                                     include_file_name=include_file.split('/')[-1],
                                                     rule_number=len(rules_str),
                                                     subrule_number=len(subrules_str),
                                                     subrule_definitions='\n\t'.join(subrules_str), 
                                                     subrule_number_definitions='\n'.join(subrule_number_str),
                                                     rule_definitions='\n\t'.join(rules_str)))
    

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option('-p', '--prefix')
    parser.add_option('-o', '--output')
    
    # Parse and ensure values
    opts, args = parser.parse_args(sys.argv[1:])
    opts.ensure_value('prefix', DEFAULT_PREFIX)
    opts.ensure_value('output', DEFAULT_RULE_FILE_NAME)
    
    main(opts.prefix, f'./{opts.output}.h', f'./{opts.output}.c')