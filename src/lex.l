%{
#include "command.h"
#include "pars.tab.h"
%}

IDENTIFIER_CHAR ([!-~]{-}[#;])
BLANK_CHAR [\0- ]

%option noyywrap nodefault nounput noinput

%%

";"			{ return SEMICOLON; }
\n			{ return NEWLINE; }
"#"[^\n]*		{ /* ignore comments */ }
{BLANK_CHAR}+		{ /* ignore whitespace */ }
{IDENTIFIER_CHAR}+	{ yylval.string = safe_strdup(yytext); return IDENTIFIER; }
.			{ fprintf(stderr, "Unknown token: %s\n", yytext); }

%%

int scan_str(const char *str, cmdlist_head_t **out) {
	YY_BUFFER_STATE bs = yy_scan_string(str);
	yy_switch_to_buffer(bs);
	yyparse(out);
	yy_delete_buffer(bs);
}

int scan_file(FILE *f, cmdlist_head_t **out) {
	yyrestart(f);
	yyparse(out);
}
