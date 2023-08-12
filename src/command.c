#include "command.h"
#include "utils.h"
#include <stdlib.h>

/* Once all tokens are appended, prepares cmd_t object for execution. */
static void cmd_finalize(cmd_t *cmd) {
	char **toks = TOKS_TO_ARR(cmd_tok_t, &cmd->toklist, next, content, char *);
	cmd->file = toks[0];
	cmd->argv = toks;
}

/* Deallocates cmd_t and everything within it that needs deallocating. */
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

/* Safely allocate and initialize a new pipe_tok_t. */
static pipecmd_tok_t *make_pipecmd_tok(cmd_t *content) {
	pipecmd_tok_t *tok = safe_malloc(sizeof(pipecmd_tok_t));
	tok->content = content;
	return tok;
}

/* safely allocate and initialize a new cmd_tok_t */
static cmd_tok_t *make_cmd_tok(char *content) {
	cmd_tok_t *tok = safe_malloc(sizeof(cmd_tok_t));
	tok->content = content;
	return tok;
}

cmd_t *make_cmd(void) {
	cmd_t *cmd = safe_malloc(sizeof(cmd_t));

	*cmd = (cmd_t){
		.in = NULL,
		.out = NULL,
		.argc = 0,
		.pipefd_in = -1,
		.pipefd_out = -1,
	};

	STAILQ_INIT(&cmd->toklist);
	return cmd;
}

pipecmd_t *make_pipecmd(void) {
	pipecmd_t *pipecmd = safe_malloc(sizeof(pipecmd_t));
	pipecmd->cmd_count = 0;
	STAILQ_INIT(&pipecmd->toklist);
	return pipecmd;
}

void free_pipecmd(pipecmd_t *pipecmd) {
	pipecmd_tok_t *tok = STAILQ_FIRST(&pipecmd->toklist);
	while (tok != NULL) {
		pipecmd_tok_t *next = STAILQ_NEXT(tok, next);
		free_cmd(tok->content);
		free(tok);
		tok = next;
	}
	free(pipecmd->cmds);
	free(pipecmd);
}

void cmd_append(cmd_t *cmd, char *content) {
	cmd_tok_t *tok = make_cmd_tok(content);
	++cmd->argc;
	STAILQ_INSERT_TAIL(&cmd->toklist, tok, next);
}

void pipecmd_finalize(pipecmd_t *pipecmd) {
	pipecmd->cmds = TOKS_TO_ARR(pipecmd_tok_t, &pipecmd->toklist, next, content, cmd_t *);
}

void pipecmd_append(pipecmd_t *pipecmd, cmd_t *content) {
	cmd_finalize(content);
	pipecmd_tok_t *tok = make_pipecmd_tok(content);

	STAILQ_INSERT_TAIL(&pipecmd->toklist, tok, next);
	++pipecmd->cmd_count;
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
