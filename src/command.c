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

cmdlist_tok_t *make_cmdlist_tok(cmd_head_t *content) {
	cmdlist_tok_t *list_tok = safe_malloc(sizeof(cmdlist_tok_t));
	*list_tok = (cmdlist_tok_t){0};
	list_tok->content = content;
	return list_tok;
}

cmdlist_head_t *make_cmdlist(void) {
	cmdlist_head_t *list = safe_malloc(sizeof(cmdlist_head_t));
	STAILQ_INIT(list);
	return list;
}

void cmd_append(cmd_head_t *cmd, char *content) {
	cmd_tok_t *tok = make_cmd_tok(content);
	STAILQ_INSERT_TAIL(cmd, tok, next);
}

void cmdlist_append(cmdlist_head_t *list, cmd_head_t *content) {
	cmdlist_tok_t *tok = make_cmdlist_tok(content);
	STAILQ_INSERT_TAIL(list, tok, next);
}
