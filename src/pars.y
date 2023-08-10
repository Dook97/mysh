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
	char		*string;
	cmd_t		*command;
	pipecmd_t	*pipecmd;
	enum redir	redirect;
}

%token	<string>	IDENTIFIER			/* commands, options, arguments */
%token	<redirect>	REDIR				/* <, >, >> */
%token			NEWLINE SEMICOLON PIPE

%type	<command>	command redir_only_command	/* for commands which start with a redirect */
%type	<pipecmd>	piped_command

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
	;

command_queue: piped_command				{ exec_pipecmd($1); }
	| command_queue SEMICOLON piped_command		{ exec_pipecmd($3); }
	;

piped_command: command					{ $$ = make_pipecmd(); pipecmd_append($$, $1); }
	| piped_command PIPE command			{ pipecmd_append($$, $3); }
	;

command: IDENTIFIER					{ $$ = make_cmd(); cmd_append($$, $1); }
	| redir_only_command IDENTIFIER			{ cmd_append($$, $2); }
	| command IDENTIFIER				{ cmd_append($1, $2); }
	| command REDIR IDENTIFIER			{ cmd_redir($$, $2, $3); }
	;

redir_only_command: REDIR IDENTIFIER			{ $$ = make_cmd(); cmd_redir($$, $1, $2); }
	| redir_only_command REDIR IDENTIFIER		{ cmd_redir($$, $2, $3); }
	;

%%

void yyerror(const char *err) {
	fprintf(stderr, "error:%d: %s near unexpected token '%s'\n", yylineno, err, yytext);
}
