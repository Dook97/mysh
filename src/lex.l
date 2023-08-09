%{
#include <err.h>
#include <stdio.h>
#include "command.h"
#include "pars.tab.h"

int lines = 1;
%}

IDENTIFIER_CHAR ([!-~]{-}[#;])
BLANK_CHAR ([\0- ]{-}[\n])

%option noyywrap nodefault nounput noinput

%%

";"			{ return SEMICOLON; }
[\n]			{ ++lines; return NEWLINE; }
"#"[^\n]*		{ /* ignore comments */ }
{BLANK_CHAR}+		{ /* ignore whitespace */ }
{IDENTIFIER_CHAR}+	{ yylval.string = safe_strdup(yytext); return IDENTIFIER; }
.			{ warnx("error:%d: unknown token: %s\n", lines, yytext); }

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
