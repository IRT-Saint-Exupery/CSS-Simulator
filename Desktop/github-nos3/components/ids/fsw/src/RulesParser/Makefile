PREFIX?=
OUTPUT?=
RULES?=
VERBOSE?=0

CSV_FILE=./rules.csv
CFLAGS=#--short-enums
PYTHON_PREFIX:=
PYTHON_OUTPUT:=

ifneq (${VERBOSE}, 0)
	CFLAGS+=-DVERBOSE
endif

ifneq (${PREFIX}, )
	PYTHON_PREFIX:=-p ${PREFIX}
endif

ifneq (${OUTPUT}, )
	PYTHON_OUTPUT:=-o ${OUTPUT}
endif

test: clean all
	./tester.sh

launch: test clean all
	./parser ${RULES}
	python3.13 generator.py ${PYTHON_PREFIX} ${PYTHON_OUTPUT}

all: utils.o errors.o rules.o list.o csv_formatter.o lex.yy.c parser.tab.c parser.tab.h
	gcc -o parser parser.tab.c lex.yy.c utils.o errors.o rules.o list.o csv_formatter.o -lm ${CFLAGS}

utils.o: utils.h
	gcc utils.c -o utils.o -c -g -Wall

errors.o: parser.tab.h errors.h 
	gcc errors.c -o errors.o -c -g -Wall

rules.o: rules.h
	gcc rules.c -o rules.o -c -g -Wall

list.o: list.h
	gcc list.c -o list.o -c -g -Wall

csv_formatter.o: csv_formatter.h
	gcc csv_formatter.c -o csv_formatter.o -c -g -Wall

parser.tab.c parser.tab.h: parser.y
	bison -t -v -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

show_conflicts: parser.y
	bison -t -v -d -Wcex parser.y

clean:
	@rm -f *.o *.out *.yy.c *.tab.* *.output parser *.csv parsed_rules.h
