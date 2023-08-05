#include "command.h"

cmd_tok_t *make_tok(const char *content) {
	cmd_tok_t *tok = safe_malloc(sizeof(cmd_tok_t));
	*tok = (cmd_tok_t){0};
	tok->content = safe_strdup(content);
	return tok;
}
