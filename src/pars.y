%{
#include <stdio.h>
#include "command.h"
#include "process.h"

void yyerror(const char *err);
int yylex(void);
extern int yylineno;
extern char *yytext;
%}

%union {
	char *string;
	enum redir redirect;
	cmd_t *command;
}

%token <string> IDENTIFIER
%token <redirect> REDIR
%token NEWLINE SEMICOLON
%token PIPE

%type <command> command

%%

all: /* empty input */
  | terminated_command_queue
  | command_queue
  | lines
  ;

lines: line
  | lines line
  ;

line: command_queue NEWLINE
  | terminated_command_queue NEWLINE
  | NEWLINE
  ;

terminated_command_queue: command_queue SEMICOLON

command_queue: command { exec_cmd($1); }
  | command_queue SEMICOLON command { exec_cmd($3); }
  ;

command: IDENTIFIER { $$ = make_cmd(); cmd_append($$, $1); }
  | command IDENTIFIER { cmd_append($1, $2); }
  | command REDIR IDENTIFIER { set_redir($$, $2, $3); }
  ;

%%

void yyerror(const char *err) {
	fprintf(stderr, "error:%d: %s near unexpected token '%s'\n", yylineno, err, yytext);
}
