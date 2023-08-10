#ifndef command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
#define command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

#include <stdbool.h>
#include <sys/queue.h>
#include <unistd.h>

enum redir { REDIR_IN, REDIR_OUT, REDIR_APPEND };

/* linked list of tokens in a command */
typedef struct cmd_tok {
	char *content;
	TAILQ_ENTRY(cmd_tok) next;
} cmd_tok_t;

TAILQ_HEAD(cmd_head, cmd_tok);
typedef struct cmd_head cmd_head_t;

/* a type representing a simple (non-piped) command */
typedef struct cmd {
	char *file;
	char *in, *out; // redirections
	char **argv; // NULL terminated arr
	size_t argc; // set on ">>" redirection
	bool append;

	/* if the command is a part of a pipe these are the pipe's file descriptors. The default
	 * value, meaning no fd was assigned (yet), is -1.
	 */
	int pipefd_in, pipefd_out;

	cmd_head_t toklist;
} cmd_t;

/* linked list of individual commands in a piped command */
typedef struct pipecmd_tok {
	cmd_t *content;
	TAILQ_ENTRY(pipecmd_tok) next;
} pipecmd_tok_t;

TAILQ_HEAD(pipe_head, pipecmd_tok);
typedef struct pipe_head pipe_head_t;

/* a type representing a piped command */
typedef struct pipecmd {
	cmd_t **cmds; // NULL terminated arr
	size_t cmd_count;

	pipe_head_t toklist;
} pipecmd_t;

/* Safely allocate and initialize a new command. */
cmd_t *make_cmd(void);

/* Safely allocate and initialize a new piped command. */
pipecmd_t *make_pipecmd(void);

/* Deep-deallocate a pipecmd_t object. */
void free_pipecmd(pipecmd_t *pipecmd);

/* Append a token to a command. */
void cmd_append(cmd_t *cmd, char *content);

/* Set command redirections (<, >, >>). */
void cmd_redir(cmd_t *cmd, enum redir r, char *file);

/* Once all tokens are appended this function prepares pipecmd_t for execution. */
void pipecmd_finalize(pipecmd_t *pipecmd);

/* Append a simple command to a piped command. */
void pipecmd_append(pipecmd_t *pipecmd, cmd_t *content);

#endif
