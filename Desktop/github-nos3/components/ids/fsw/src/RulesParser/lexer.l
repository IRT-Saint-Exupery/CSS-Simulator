%option noyywrap
%option noinput
%option nounput

%{
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include "utils.h"
    #include "parser.tab.h"
%}

COMMENT [#].*
PIPE [|]
COLON [:]
SEMICOLON [;]
HEX 0x[0-9a-fA-F]+
NUM [0-9]+
STRING [a-zA-Z0-9/\.]+
PATH ['][a-zA-Z0-9 /\.]+[']

%% 

{COMMENT} ;
{PIPE} {start_token(); increment_location(1); return tPIPE;}
{COLON} {start_token(); increment_location(1); return tCOLON;}
{SEMICOLON} {start_token(); increment_location(1); return tSEMICOLON;}
{HEX} {start_token(); increment_location(strlen(yytext)); yylval.nb = strtol(yytext + 2, NULL, 16); return tNUM;}
{NUM} {start_token(); increment_location(strlen(yytext)); yylval.nb = atoi(yytext); return tNUM;}
{STRING} {start_token(); increment_location(strlen(yytext)); yylval.value = strdup(yytext); return tSTRING;}
{PATH} {start_token(); increment_location(strlen(yytext)); yylval.value = strdup(yytext); return tSTRING;}

"\n" {increment_line(); return tLINE_JUMP;}
"\r" ;
" " {increment_location(1);}    
. {start_token(); increment_location(1); return tOTHER;}
