%{
#include <stdio.h>
#include "command.h"
#include "process.h"

void yyerror(const char *err);
int yylex(void);
extern int lines;
%}

%union {
	char *string;
	cmd_head_t *command;
}

%token <string> IDENTIFIER
%token NEWLINE SEMICOLON
%token REDIR_IN REDIR_OUT PIPE

%type <command> command

%%

all: terminated_command_queue
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
  ;

%%

void yyerror(const char *err) {
	fprintf(stderr, "error:%d: %s\n", lines, err);
}
