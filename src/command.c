#include "command.h"
#include "utils.h"
#include <stdlib.h>

static void cmd_finalize(cmd_t *cmd) {
	char **toks = TOKS_TO_ARR(cmd_tok_t, &cmd->toklist, next, content, char *);
	cmd->file = toks[0];
	cmd->argv = toks;
}

static void free_cmd(cmd_t *cmd) {
	cmd_tok_t *tok = TAILQ_FIRST(&cmd->toklist);
	while (tok != NULL) {
		cmd_tok_t *next = TAILQ_NEXT(tok, next);
		free(tok->content);
		free(tok);
		tok = next;
	}

	free(cmd->out);
	free(cmd->in);
	free(cmd->argv);

	if (cmd->pipefd_in >= 0)
		close(cmd->pipefd_in);
	if (cmd->pipefd_out >= 0)
		close(cmd->pipefd_out);

	free(cmd);
}

/* safely allocate and initialize a new pipe_tok_t */
static pipecmd_tok_t *make_pipecmd_tok(cmd_t *content) {
	pipecmd_tok_t *tok = safe_malloc(sizeof(pipecmd_tok_t));
	tok->content = content;
	return tok;
}

/* safely allocate and initialize a new cmd_tok_t */
static cmd_tok_t *make_cmd_tok(const char *content) {
	cmd_tok_t *tok = safe_malloc(sizeof(cmd_tok_t));
	tok->content = safe_strdup(content);
	return tok;
}

cmd_t *make_cmd(void) {
	cmd_t *cmd = safe_malloc(sizeof(cmd_t));

	// TODO: tell clang-format to not do this type of shit
	*cmd = (cmd_t){.in = NULL, .out = NULL, .argc = 0, .pipefd_in = -1, .pipefd_out = -1};

	TAILQ_INIT(&cmd->toklist);
	return cmd;
}

pipecmd_t *make_pipecmd(void) {
	pipecmd_t *pipecmd = safe_malloc(sizeof(pipecmd_t));
	pipecmd->cmd_count = 0;
	TAILQ_INIT(&pipecmd->toklist);
	return pipecmd;
}

void free_pipecmd(pipecmd_t *pipe) {
	pipecmd_tok_t *tok = TAILQ_FIRST(&pipe->toklist);
	while (tok != NULL) {
		pipecmd_tok_t *next = TAILQ_NEXT(tok, next);
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
	TAILQ_INSERT_TAIL(&cmd->toklist, tok, next);
}

void pipecmd_finalize(pipecmd_t *pipe) {
	pipe->cmds = TOKS_TO_ARR(pipecmd_tok_t, &pipe->toklist, next, content, cmd_t *);
}

void pipecmd_append(pipecmd_t *pipe, cmd_t *content) {
	if (pipe->cmd_count >= 1) {
		/* [0] = read, [1] = write */
		int fildes[2];
		safe_pipe(fildes);
		cmd_t *prev = TAILQ_LAST(&pipe->toklist, pipe_head)->content;

		prev->pipefd_out = fildes[1];
		content->pipefd_in = fildes[0];
	}

	cmd_finalize(content);
	pipecmd_tok_t *tok = make_pipecmd_tok(content);

	TAILQ_INSERT_TAIL(&pipe->toklist, tok, next);
	++pipe->cmd_count;
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
