#include "command.h"

cmd_tok_t *make_cmd_tok(const char *content) {
	cmd_tok_t *tok = safe_malloc(sizeof(cmd_tok_t));
	*tok = (cmd_tok_t){0};
	tok->content = safe_strdup(content);
	return tok;
}

cmd_head_t *make_cmd(void) {
	cmd_head_t *cmd = safe_malloc(sizeof(cmd_head_t));
	STAILQ_INIT(cmd);
	return cmd;
}

void cmd_append(cmd_head_t *cmd, char *content) {
	cmd_tok_t *tok = make_cmd_tok(content);
	STAILQ_INSERT_TAIL(cmd, tok, next);
}

void cmd_from_toks(cmd_head_t *list, cmd_t *out) {
	char **toks = TOKS_TO_ARR(cmd_tok_t, list, next, content);

	*out = (cmd_t){
		.file = toks[0],
		.args = toks
	};
}
