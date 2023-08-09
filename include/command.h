#ifndef command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
#define command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

#include "utils.h"
#include <sys/queue.h>

enum redir {
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND
};

/* linked list of tokens in a command */
typedef struct cmd_tok {
	char *content;
	STAILQ_ENTRY(cmd_tok) next;
} cmd_tok_t;

STAILQ_HEAD(cmd_head, cmd_tok);
typedef struct cmd_head cmd_head_t;

typedef struct cmd {
	char *file;
	char *in, *out; // redirections
	char **argv; // NULL terminated arr
	size_t argc;
	bool append; // set on ">>" redirection

	cmd_head_t toklist;
} cmd_t;

typedef struct pipe_tok {
	cmd_t *content;
	STAILQ_ENTRY(pipe_tok) next;
} pipe_tok_t;

STAILQ_HEAD(pipe_head, pipe_tok);
typedef struct pipe_head pipe_head_t;

typedef struct pipe {
	pipe_head_t cmds;
	size_t cmd_count;
} pipe_t;

/* safely allocate and initialize a new command */
cmd_t *make_cmd(void);

/* safely allocate and initialize a new piped command */
pipe_t *make_pipe(void);

/* deep-deallocate a pipe_t object */
void free_pipe(pipe_t *pipe);

/* append a token to a command */
void cmd_append(cmd_t *cmd, char *content);

/* prepare cmd_t for use by filling necessary fields */
void cmd_finalize(cmd_t *out);

void cmd_redir(cmd_t *cmd, enum redir r, char *file);

#endif
