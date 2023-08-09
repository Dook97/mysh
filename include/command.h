#ifndef command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
#define command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

#include "utils.h"
#include <sys/queue.h>

enum redir { REDIR_IN, REDIR_OUT, REDIR_APPEND };

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

typedef struct pipecmd_tok {
	cmd_t *content;
	STAILQ_ENTRY(pipecmd_tok) next;
} pipecmd_tok_t;

STAILQ_HEAD(pipe_head, pipecmd_tok);
typedef struct pipe_head pipe_head_t;

typedef struct pipe {
	cmd_t **cmds;
	size_t cmd_count;

	pipe_head_t toklist;
} pipecmd_t;

/* safely allocate and initialize a new command */
cmd_t *make_cmd(void);

/* safely allocate and initialize a new piped command */
pipecmd_t *make_pipecmd(void);

/* deep-deallocate a pipecmd_t object */
void free_pipecmd(pipecmd_t *pipe);

/* append a token to a command */
void cmd_append(cmd_t *cmd, char *content);

/* set command redirection */
void cmd_redir(cmd_t *cmd, enum redir r, char *file);

void pipecmd_finalize(pipecmd_t *pipe);

void pipecmd_append(pipecmd_t *pipe, cmd_t *content);

#endif
