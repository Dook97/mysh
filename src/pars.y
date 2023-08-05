%{
#include <stdio.h>
#include "command.h"

void yyerror(const char *s);
int yylex(void);
%}

%union {
	char *string;
	cmd_head_t *command;
	cmdlist_head_t *command_list;
}

%token <string> IDENTIFIER
%token NEWLINE SEMICOLON
%token REDIR_IN REDIR_OUT PIPE

%type <command> command
%type <command_list> command_queue

%%

all: terminated_command_queue
  | command_queue
  ;

terminated_command_queue: command_queue SEMICOLON

command_queue: command { $$ = make_cmdlist(); cmdlist_append($$, $1); }
  | command_queue SEMICOLON command { cmdlist_append($1, $3); }
  ;

command: IDENTIFIER { $$ = make_cmd(); cmd_append($$, $1); }
  | command IDENTIFIER { cmd_append($1, $2); }
  ;

%%

void yyerror(const char *s) {
	fprintf(stderr, "error: %s\n", s);
}
