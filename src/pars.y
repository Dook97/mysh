%{
#include <stdio.h>
#include "command.h"
#include "process.h"
#include "magic.h"

void yyerror(const char *err);
int yylex(void);
extern int yylineno;
extern char *yytext;
extern int sh_exit;
%}

%union {
	int		numeric;
	char		*string;
	cmd_t		*command;
	pipecmd_t	*pipecmd;
	redir_t		*redirect;
	enum redir_type redir_type;
}

%token	<numeric>	FILE_DESCRIPTOR			/* file descriptor */
%token	<string>	IDENTIFIER			/* commands, options, arguments */
%token	<redir_type>	REDIR FDREDIR			/* <, >, <&, >&, >> */
%token			NEWLINE SEMICOLON PIPE

%type	<command>	command redir_only_command
%type	<pipecmd>	piped_command
%type	<redirect>	redir

/* free memory in case of a parsing error */
%destructor { free($$); }		<string>
%destructor { free_cmd($$); }		<command>
%destructor { free_pipecmd($$); }	<pipecmd>

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

command_queue: piped_command				{ pipecmd_finalize($1); exec_pipecmd($1); }
	| command_queue SEMICOLON piped_command		{ pipecmd_finalize($3); exec_pipecmd($3); }
	;

piped_command: command					{ $$ = make_pipecmd(); pipecmd_append($$, $1); }
	| piped_command PIPE command			{ $$ = $1; pipecmd_append($1, $3); }
	;

command: IDENTIFIER					{ $$ = make_cmd(); cmd_append($$, $1); }
	| redir_only_command IDENTIFIER			{ $$ = $1; cmd_append($1, $2); }
	| command IDENTIFIER				{ $$ = $1; cmd_append($1, $2); }
	| command redir					{ $$ = $1; redir_append($1, $2); }
	;

redir_only_command: redir				{ $$ = make_cmd(); redir_append($$, $1); }
	| redir_only_command redir			{ $$ = $1; redir_append($1, $2); }
	;

redir: FILE_DESCRIPTOR REDIR IDENTIFIER			{ $$ = make_redir($2, $1, NULL, $3); }
	| FILE_DESCRIPTOR FDREDIR IDENTIFIER		{ $$ = make_redir($2, $1, $3, NULL); }
	| REDIR IDENTIFIER				{ $$ = make_redir($1, -1, NULL, $2); }
	| FDREDIR IDENTIFIER				{ $$ = make_redir($1, -1, $2, NULL); }
	;

%%

void yyerror(const char *err) {
	sh_exit = PARSING_ERR;
	fprintf(stderr, "error:%d: %s near unexpected token '%s'\n", yylineno, err, yytext);
}
