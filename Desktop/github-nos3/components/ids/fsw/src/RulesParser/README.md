# Rules Parser

This repository is an example of simple rule parsing for a small IDS (Intrusion Detection System). It contains: 
- A flex lexer 
- A bison parser
- A CSV formatter in C (to transfer information to Python)
- A C code generator in Python (that will contain the parsed rules)
- An IDS that can parse a specific type of packets (see below)

## Requirements

This repository requires `gcc` and `make` to compile C code. The parser requires **Bison** (`apt install bison`), and the lexer requires **Flex** (`apt install flex`). The code generation requires **Python 3**.

## The parser

The parser is designed to parse "stateless" and "stateful" rules. A stateless rule is a rule that only has a measure (A -> Alert, D -> Drop). A stateful rule is a succession of rule separated by a semicolon that has a start state and an action (either a state or a measure). A rule is composed of a body, being a message id and a function code. A message id must be a 16 bits integer with high byte being 08h, 18h, 09h or 19h. A rule can have a parameter (optional) representing the packet value, being an offset in the packet (in bits), the value type, the value length in the packet (in bits) and the value. Below is the syntax for a simple rule, for more details look at the example or the valid inputs in the `tests` directory:

`<header> <message_id> <function_code> [<offset>:<type>:<length> <value>]`

Examples: 
- `D 0x09A5 1`
- `E0|E1 0x18FF 78 ; E1|A 0x0930 4 256:STRING:24 NO ; E1|E0 0x0930 4 256:STRING:24 YES`
- `A 0x1977 3 128:INT:32 5000`

## Packet formation

The packet for the IDS is made as such: `<message_id upper byte> <message_id lower byte> XX XX XX XX <function code byte> <offset + value>` (`XX` represents any value). The offset value is the offset from the start of the packet, this allows the user to adapt to different packets if the headers are different. 

## How to launch? 

You can use the makefile to compile, launch the parser and generate the C code. 

To launch the tests (presents in the `tests` directory) that will check if the parser is good, use `make test`.

To compile the parser, use `make all`. You can set the `VERBOSE` parameter to 1 to display the parsed rules in the console after parsing (`make all VERBOSE=1`)

To compile and generate the C code containing all the parsed rules, use `make launch RULES=<rule file>`. You can add a prefix to the generated rules with the `PREFIX` parameter, and you can set the output file name with the `OUTPUT` parameter. By default, there is no prefix and the output is `parsed_rules`. For instance: `make launch RULES=tests/valid_input1.rules PREFIX=probe3 OUTPUT=probe3`. Note that you can also put the `VERBOSE` parameter. 

In `ids.c` there is a small example with the `valid_input1.rules` file in `tests`. You can use it to understand how the IDS works. To compile, use `gcc ids.c probe3.c --short-enums`