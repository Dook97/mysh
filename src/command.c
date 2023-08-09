#include "command.h"

cmd_tok_t *make_cmd_tok(const char *content) {
	cmd_tok_t *tok = safe_malloc(sizeof(cmd_tok_t));
	tok->content = safe_strdup(content);
	return tok;
}

cmd_t *make_cmd(void) {
	cmd_t *cmd = safe_malloc(sizeof(cmd_t));
	STAILQ_INIT(&cmd->toklist);
	return cmd;
}

void free_cmd_toklist(cmd_head_t *cmd) {
	cmd_tok_t *tok = STAILQ_FIRST(cmd);
	while (tok != NULL) {
		cmd_tok_t *next = STAILQ_NEXT(tok, next);
		free(tok->content);
		free(tok);
		tok = next;
	}
}

void cmd_append(cmd_t *cmd, char *content) {
	cmd_tok_t *tok = make_cmd_tok(content);
	STAILQ_INSERT_TAIL(&cmd->toklist, tok, next);
}

void set_redir(cmd_t *cmd, char *file, enum redir r) {
	switch (r) {
	case redir_in:
		cmd->in = file;
		break;
	case redir_append:
		cmd->out = file;
		cmd->append = true;
		break;
	case redir_out:
		cmd->out = file;
		cmd->append = false;
		break;
	}
}

void cmd_finalize(cmd_t *cmd) {
	char **toks = TOKS_TO_ARR(cmd_tok_t, &cmd->toklist, next, content);
	cmd->file = toks[0];
	cmd->argv = toks;
	cmd->argc = STAILQ_LEN(cmd_tok_t, &cmd->toklist, next);
}
