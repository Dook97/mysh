%{
#include <stdio.h>
#include "command.h"
#include "process.h"
#include "magic.h"
#include "utils.h"

void yyerror(const char *err);
int yylex(void);
extern int yylineno;
extern char *yytext;
%}

%union {
	int		numeric;
	char		*string;
	cmd_t		*command;
	pipecmd_t	*pipecmd;
	redir_t		*redirect;
	enum redir_type redir_type;
}

%token	<numeric>	FILE_DESCRIPTOR
%token	<string>	WORD
%token	<redir_type>	REDIR				/* <, >, <&, >&, >> */
%token			AND OR ';' '|' '!' '\n'

%type	<command>	command redir_only_command
%type	<pipecmd>	piped_command
%type	<redirect>	redir
%type	<numeric>	and_or_list
%type	<string>	word

/* free memory in case of a parsing error */
%destructor { free($$); }		<string>
%destructor { free_redir($$); }		<redirect>
%destructor { free_cmd($$); }		<command>
%destructor { free_pipecmd($$); }	<pipecmd>

%%

all: /* empty input */
	| terminated_command_list
	| command_list
	| lines
	;

lines: line
	| lines line
	;

line: command_list '\n'
	| terminated_command_list '\n'
	| '\n'
	;

terminated_command_list: command_list ';'
	;

command_list: and_or_list
	| command_list ';' and_or_list
	;

and_or_list: piped_command			{ $$ = exec_pipecmd($1); free_pipecmd($1); }
	| and_or_list AND piped_command
	{
		if ($$ == 0)
			$$ = exec_pipecmd($3);
		free_pipecmd($3);
	}
	| and_or_list OR piped_command
	{
		if ($$ != 0)
			$$ = exec_pipecmd($3);
		free_pipecmd($3);
	}
	;

piped_command: command				{ $$ = make_pipecmd(); pipecmd_append($$, $1); }
	| '!' command				{ $$ = make_pipecmd(); $$->negated = true; pipecmd_append($$, $2); }
	| piped_command '|' command		{ $$ = $1; pipecmd_append($1, $3); }
	;

command: WORD /* use "WORD" instead of "word" here to exclude "!" */
	{
		$$ = make_cmd();
		cmd_append($$, $1);
	}
	| redir_only_command word		{ $$ = $1; cmd_append($1, $2); }
	| command word				{ $$ = $1; cmd_append($1, $2); }
	| command redir				{ $$ = $1; redir_append($1, $2); }
	;

redir_only_command: redir			{ $$ = make_cmd(); redir_append($$, $1); }
	| redir_only_command redir		{ $$ = $1; redir_append($1, $2); }
	;

redir: REDIR word				{ $$ = make_redir($1, FD_INVALID, $2); }
	| FILE_DESCRIPTOR REDIR word		{ $$ = make_redir($2, $1, $3); }
	;

word: WORD					{ $$ = $1; }
	| '!'
	{
		char *str = safe_malloc(sizeof(char) * 2);
		str[0] = '!'; str[1] = '\0';
		$$ = str;
	}

%%

void yyerror(const char *err) {
	sh_exit = PARSING_ERR;
	fprintf(stderr, "error:%d: %s near unexpected token '%s'\n", yylineno, err, yytext);
}
