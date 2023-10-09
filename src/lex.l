%{
#include "command.h"
#include "pars.tab.h"
#include "utils.h"
#include <err.h>
%}

IDENTIFIER_CHAR ([!-~]{-}[#;<>|])
BLANK_CHAR ([\0- ]{-}[\n])

%option noyywrap nodefault nounput noinput yylineno

%%

";"			{ return SEMICOLON; }
"|"			{ return PIPE; }
\n			{ return NEWLINE; }
">>"			{ yylval.redirect = REDIR_APPEND; return REDIR; }
"<"			{ yylval.redirect = REDIR_IN; return REDIR; }
">"			{ yylval.redirect = REDIR_OUT; return REDIR; }
"#"[^\n]*		{ /* ignore comments */ }
{BLANK_CHAR}+		{ /* ignore whitespace */ }
{IDENTIFIER_CHAR}+	{ yylval.string = safe_strdup(yytext); return IDENTIFIER; }

.			{ warnx("error:%d: unknown token: %s\n", yylineno, yytext); }

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
