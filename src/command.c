#include "command.h"
#include "magic.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

/* Once all tokens are appended, prepares cmd_t object for execution. */
static void cmd_finalize(cmd_t *cmd) {
	char **toks = STAILQ_TO_ARR(cmd_tok_t, &cmd->toklist, next, content, char *);
	cmd->argv = toks;
}

/* safely allocate and initialize a new pipe_tok_t. */
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

/* safely allocate and initialize a new redir_tok_t */
static redir_tok_t *make_redir_tok(redir_t *content) {
	redir_tok_t *tok = safe_malloc(sizeof(redir_tok_t));
	tok->content = content;
	return tok;
}

cmd_t *make_cmd(void) {
	cmd_t *cmd = safe_malloc(sizeof(cmd_t));

	*cmd = (cmd_t){
		.argv = NULL, /* to enable safe free()-ing in case of parser error */
		.argc = 0,
		.pipefd_in = FD_INVALID,
		.pipefd_out = FD_INVALID,
	};

	STAILQ_INIT(&cmd->toklist);
	STAILQ_INIT(&cmd->redirlist);
	return cmd;
}

pipecmd_t *make_pipecmd(void) {
	pipecmd_t *pipecmd = safe_malloc(sizeof(pipecmd_t));
	pipecmd->cmd_count = 0;
	pipecmd->negated = false;
	STAILQ_INIT(&pipecmd->toklist);
	return pipecmd;
}

redir_t *make_redir(enum redir_type type, int left_fd, char *right) {
	if (left_fd < 0)
		left_fd = (type == REDIR_IN || type == FDREDIR_IN || type == REDIR_OPEN)
				  ? FD_STDIN
				  : FD_STDOUT;

	int right_fd = FD_INVALID;
	char *endptr = NULL;
	right_fd = strtol(right, &endptr, 10);

	/* *endptr == '\0' iff entire string is valid for conversion to a number */
	if (*endptr != '\0')
		right_fd = FD_INVALID;

	redir_t *out = safe_malloc(sizeof(redir_t));
	*out = (redir_t){
		.type = type,
		.left_fd = left_fd,
		.right_fd = right_fd,
		.file = right,
	};

	return out;
}

void free_redir(redir_t *redir) {
	free(redir->file);
	free(redir);
}

void free_cmd(cmd_t *cmd) {
	cmd_tok_t *tok = STAILQ_FIRST(&cmd->toklist);
	while (tok != NULL) {
		cmd_tok_t *next = STAILQ_NEXT(tok, next);
		free(tok->content);
		free(tok);
		tok = next;
	}

	redir_tok_t *rtok = STAILQ_FIRST(&cmd->redirlist);
	while (rtok != NULL) {
		redir_tok_t *next = STAILQ_NEXT(rtok, next);
		free_redir(rtok->content);
		free(rtok);
		rtok = next;
	}

	free(cmd->argv);
	free(cmd);
}

void free_pipecmd(pipecmd_t *pipecmd) {
	pipecmd_tok_t *tok = STAILQ_FIRST(&pipecmd->toklist);
	while (tok != NULL) {
		pipecmd_tok_t *next = STAILQ_NEXT(tok, next);
		free_cmd(tok->content);
		free(tok);
		tok = next;
	}
	free(pipecmd);
}

void cmd_append(cmd_t *cmd, char *content) {
	cmd_tok_t *tok = make_cmd_tok(content);
	++cmd->argc;
	STAILQ_INSERT_TAIL(&cmd->toklist, tok, next);
}

void pipecmd_append(pipecmd_t *pipecmd, cmd_t *content) {
	cmd_finalize(content);
	pipecmd_tok_t *tok = make_pipecmd_tok(content);

	STAILQ_INSERT_TAIL(&pipecmd->toklist, tok, next);
	++pipecmd->cmd_count;
}

void redir_append(cmd_t *cmd, redir_t *content) {
	redir_tok_t *tok = make_redir_tok(content);
	STAILQ_INSERT_TAIL(&cmd->redirlist, tok, next);
}
