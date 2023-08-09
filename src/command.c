#include "command.h"

cmd_tok_t *make_cmd_tok(const char *content) {
	cmd_tok_t *tok = safe_malloc(sizeof(cmd_tok_t));
	tok->content = safe_strdup(content);
	return tok;
}

cmd_t *make_cmd(void) {
	cmd_t *cmd = safe_malloc(sizeof(cmd_t));
	cmd->out = cmd->in = NULL;
	STAILQ_INIT(&cmd->toklist);
	return cmd;
}

void free_cmd(cmd_t *cmd) {
	cmd_tok_t *tok = STAILQ_FIRST(&cmd->toklist);
	while (tok != NULL) {
		cmd_tok_t *next = STAILQ_NEXT(tok, next);
		free(tok->content);
		free(tok);
		tok = next;
	}

	if (cmd->out != NULL)
		free(cmd->out);
	if (cmd->in != NULL)
		free(cmd->in);

	free(cmd->argv);
	free(cmd);
}

void cmd_append(cmd_t *cmd, char *content) {
	cmd_tok_t *tok = make_cmd_tok(content);
	STAILQ_INSERT_TAIL(&cmd->toklist, tok, next);
}

void set_redir(cmd_t *cmd, char *file, enum redir r) {
	switch (r) {
	case redir_in:
		if (cmd->in != NULL)
			free(cmd->in);
		cmd->in = file;
		break;
	case redir_append:
		if (cmd->out != NULL)
			free(cmd->out);
		cmd->out = file;
		cmd->append = true;
		break;
	case redir_out:
		if (cmd->out != NULL)
			free(cmd->out);
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
