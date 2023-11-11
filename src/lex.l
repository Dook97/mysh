%{
#include "command.h"
#include "pars.tab.h"
#include "utils.h"
#include <err.h>
#include <stdlib.h>
%}

NUMBER			(0|[1-9][0-9]*)
BLANK_CHAR		([\0- ]{-}[\n])
IDENTIFIER_CHAR		([!-~\x80-\xf7]{-}[#;<>|])

%option noyywrap nodefault nounput noinput yylineno

%%

";"				{ return ';'; }
"|"				{ return '|'; }
\n				{ return '\n'; }
{NUMBER}/([<>]&?|>>)		{ yylval.numeric = strtol(yytext, NULL, 10); return FILE_DESCRIPTOR; }
"<"				{ yylval.redir_type = REDIR_IN; return REDIR; }
">"				{ yylval.redir_type = REDIR_OUT; return REDIR; }
"<&"				{ yylval.redir_type = FDREDIR_IN; return REDIR; }
">&"				{ yylval.redir_type = FDREDIR_OUT; return REDIR; }
">>"				{ yylval.redir_type = REDIR_APPEND; return REDIR; }
"<>"				{ yylval.redir_type = REDIR_OPEN; return REDIR; }
"#"[^\n]*			{ /* ignore comments */ }
{BLANK_CHAR}+			{ /* ignore whitespace */ }
{IDENTIFIER_CHAR}+		{ yylval.string = safe_strdup(yytext); return IDENTIFIER; }

.				{ warnx("error:%d: unknown token: %s", yylineno, yytext); }

%%

int shell_str(const char *str) {
	YY_BUFFER_STATE bs = yy_scan_string(str);
	yy_switch_to_buffer(bs);
	int ret = yyparse();
	yy_delete_buffer(bs);
	return ret;
}

int shell_file(FILE *f) {
	yyrestart(f);
	return yyparse();
}
