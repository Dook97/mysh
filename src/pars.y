%{
#include <stdio.h>
#include "command.h"

void yyerror(cmdlist_head_t **out, const char *err);
int yylex(void);
extern int lines;
%}

/* parser output parameter */
%parse-param { cmdlist_head_t **out }

%union {
	char *string;
	cmd_head_t *command;
	cmdlist_head_t *command_list;
}

%token <string> IDENTIFIER
%token NEWLINE SEMICOLON
%token REDIR_IN REDIR_OUT PIPE

%type <command> command
%type <command_list> command_queue terminated_command_queue line lines consolidate

%%

all: consolidate { *out = $1; }

consolidate: terminated_command_queue
  | command_queue
  | lines
  ;

lines: line
  | lines line { cmdlist_concat($1, $2); }
  ;

line: command_queue NEWLINE
  | terminated_command_queue NEWLINE
  ;

terminated_command_queue: command_queue SEMICOLON

command_queue: command { $$ = make_cmdlist(); cmdlist_append($$, $1); }
  | command_queue SEMICOLON command { cmdlist_append($1, $3); }
  ;

command: IDENTIFIER { $$ = make_cmd(); cmd_append($$, $1); }
  | command IDENTIFIER { cmd_append($1, $2); }
  ;

%%

void yyerror(cmdlist_head_t **out, const char *err) {
	fprintf(stderr, "error:%d: %s\n", lines, err);
}
