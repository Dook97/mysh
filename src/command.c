#include "command.h"

/* safely allocate and initialize a new cmd_tok_t */
static cmd_tok_t *make_cmd_tok(const char *content) {
	cmd_tok_t *tok = safe_malloc(sizeof(cmd_tok_t));
	tok->content = safe_strdup(content);
	return tok;
}

/* safely allocate and initialize a new pipe_tok_t */
static pipecmd_tok_t *make_pipecmd_tok(cmd_t *content) {
	pipecmd_tok_t *tok = safe_malloc(sizeof(pipecmd_tok_t));
	tok->content = content;
	return tok;
}

cmd_t *make_cmd(void) {
	cmd_t *cmd = safe_malloc(sizeof(cmd_t));
	cmd->out = cmd->in = NULL;
	cmd->argc = 0;
	STAILQ_INIT(&cmd->toklist);
	return cmd;
}

pipecmd_t *make_pipecmd(void) {
	pipecmd_t *pipe = safe_malloc(sizeof(pipecmd_t));
	pipe->cmd_count = 0;
	STAILQ_INIT(&pipe->toklist);
	return pipe;
}

static void free_cmd(cmd_t *cmd) {
	cmd_tok_t *tok = STAILQ_FIRST(&cmd->toklist);
	while (tok != NULL) {
		cmd_tok_t *next = STAILQ_NEXT(tok, next);
		free(tok->content);
		free(tok);
		tok = next;
	}

	free(cmd->out);
	free(cmd->in);
	free(cmd->argv);
	free(cmd);
}

void free_pipecmd(pipecmd_t *pipe) {
	pipecmd_tok_t *tok = STAILQ_FIRST(&pipe->toklist);
	while (tok != NULL) {
		pipecmd_tok_t *next = STAILQ_NEXT(tok, next);
		free_cmd(tok->content);
		free(tok);
		tok = next;
	}
	free(pipe->cmds);
	free(pipe);
}

void cmd_append(cmd_t *cmd, char *content) {
	cmd_tok_t *tok = make_cmd_tok(content);
	++cmd->argc;
	STAILQ_INSERT_TAIL(&cmd->toklist, tok, next);
}

static void cmd_finalize(cmd_t *cmd) {
	char **toks = TOKS_TO_ARR(cmd_tok_t, &cmd->toklist, next, content, char *);
	cmd->file = toks[0];
	cmd->argv = toks;
}

void pipecmd_finalize(pipecmd_t *pipe) {
	pipe->cmds = TOKS_TO_ARR(pipecmd_tok_t, &pipe->toklist, next, content, cmd_t *);
}

void pipecmd_append(pipecmd_t *pipe, cmd_t *content) {
	pipecmd_tok_t *tok = make_pipecmd_tok(content);
	cmd_finalize(content);
	++pipe->cmd_count;
	STAILQ_INSERT_TAIL(&pipe->toklist, tok, next);
}

void cmd_redir(cmd_t *cmd, enum redir r, char *file) {
	switch (r) {
	case REDIR_IN:
		free(cmd->in);
		cmd->in = file;
		break;
	case REDIR_APPEND:
		free(cmd->out);
		cmd->out = file;
		cmd->append = true;
		break;
	case REDIR_OUT:
		free(cmd->out);
		cmd->out = file;
		cmd->append = false;
		break;
	}
}
