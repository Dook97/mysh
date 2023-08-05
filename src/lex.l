%{
#include "command.h"
#include "pars.tab.h"

int lines = 1;
%}

IDENTIFIER_CHAR ([!-~]{-}[#;])
BLANK_CHAR [\0- ]

%option noyywrap nodefault nounput noinput

%%

";"			{ return SEMICOLON; }
[\n]+			{ lines += strlen(yytext); return NEWLINE; }
"#"[^\n]*		{ /* ignore comments */ }
{BLANK_CHAR}+		{ /* ignore whitespace */ }
{IDENTIFIER_CHAR}+	{ yylval.string = safe_strdup(yytext); return IDENTIFIER; }
.			{ fprintf(stderr, "error:%d: unknown token: %s\n", lines, yytext); }

%%

int scan_str(const char *str, cmdlist_head_t **out) {
	YY_BUFFER_STATE bs = yy_scan_string(str);
	yy_switch_to_buffer(bs);
	*out = make_cmdlist();
	int ret = yyparse(out);
	yy_delete_buffer(bs);
	return ret;
}

int scan_file(FILE *f, cmdlist_head_t **out) {
	yyrestart(f);
	*out = make_cmdlist();
	return yyparse(out);
}
