%{
#include <stdio.h>
#include "command.h"

void yyerror(const char *s);
int yylex(void);
%}

%union {
	cmd_tok_t *token;
	cmd_head_t *command;
	cmdlist_head_t *cmdlist;
}

%token <token> IDENTIFIER
%token NEWLINE SEMICOLON
%token REDIR_IN REDIR_OUT PIPE

%type <command> command
%type <cmdlist> command_queue

%%

all: terminated_command_queue
  | command_queue
  ;

terminated_command_queue: command_queue SEMICOLON

command_queue: command {$$=(cmdlist_head_t *)NULL;}
  | command_queue SEMICOLON command
  ;

command: IDENTIFIER {$$=(cmd_head_t *)NULL;}
  | command IDENTIFIER {printf("%s\n", $2->content);}
  ;

%%

void yyerror(const char *s) {
	fprintf(stderr, "error: %s\n", s);
}
