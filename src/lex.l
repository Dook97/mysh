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
{IDENTIFIER_CHAR}+	{ yylval.token = make_tok(yytext); return IDENTIFIER; }
.			{ fprintf(stderr, "Unknown token: %s\n", yytext); }

%%

void flex_scan_str(const char *str) {
	YY_BUFFER_STATE bs = yy_scan_string(str);
	yy_switch_to_buffer(bs);
	yyparse();
	yy_delete_buffer(bs);
}

void flex_scan_file(FILE *f) {
	yyrestart(f);
	yyparse();
}
