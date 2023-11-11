#ifndef command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
#define command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

#include <stdbool.h>
#include <sys/queue.h>
#include <unistd.h>

enum redir_type {
	REDIR_IN,	// [0-9]*< FILE
	REDIR_OUT,	// [0-9]*> FILE
	REDIR_APPEND,	// [0-9]*>> FILE
	FDREDIR_IN,	// [0-9]*<&[0-9]+
	FDREDIR_OUT,	// [0-9]*>&[0-9]+
};

typedef struct redir {
	enum redir_type type;
	int left_fd;
	int right_fd;
	char *file;
} redir_t;

typedef struct redir_tok {
	redir_t *content;
	STAILQ_ENTRY(redir_tok) next;
} redir_tok_t;

STAILQ_HEAD(redir_head, redir_tok);
typedef struct redir_head redir_head_t;

/* linked list of tokens in a command */
typedef struct cmd_tok {
	char *content;
	STAILQ_ENTRY(cmd_tok) next;
} cmd_tok_t;

STAILQ_HEAD(cmd_head, cmd_tok);
typedef struct cmd_head cmd_head_t;

/* a type representing a simple (non-piped) command */
typedef struct cmd {
	char **argv; /* NULL terminated arr */
	size_t argc;

	/* if the command is a part of a pipe these are the pipe's file descriptors. The default
	 * value, meaning no fd was assigned (yet), is FD_INVALID.
	 */
	int pipefd_in, pipefd_out;

	redir_head_t redirlist;
	cmd_head_t toklist;
} cmd_t;

/* linked list of individual commands in a piped command */
typedef struct pipecmd_tok {
	cmd_t *content;
	STAILQ_ENTRY(pipecmd_tok) next;
} pipecmd_tok_t;

STAILQ_HEAD(pipe_head, pipecmd_tok);
typedef struct pipe_head pipe_head_t;

/* a type representing a piped command */
typedef struct pipecmd {
	cmd_t **cmds; /* NULL terminated arr */
	size_t cmd_count;

	pipe_head_t toklist;
} pipecmd_t;

/* safely allocate and initialize a new command */
cmd_t *make_cmd(void);

/* safely allocate and initialize a new piped command */
pipecmd_t *make_pipecmd(void);

/* safely allocate and initialize a new redirect */
redir_t *make_redir(enum redir_type type, int leftfd, char *rightfd, char *file);

/* free a redir_t */
void free_redir(redir_t *redir);

/* Deallocates cmd_t and everything within it that needs deallocating */
void free_cmd(cmd_t *cmd);

/* Deep-deallocate a pipecmd_t object */
void free_pipecmd(pipecmd_t *pipecmd);

/* Append a token to a command */
void cmd_append(cmd_t *cmd, char *content);

/* Append a simple command to a piped command */
void pipecmd_append(pipecmd_t *pipecmd, cmd_t *content);

/* Append a redirect to a list of redirects */
void redir_append(cmd_t *cmd, redir_t *content);

/* Once all tokens are appended this function prepares pipecmd_t for execution */
void pipecmd_finalize(pipecmd_t *pipecmd);

#endif
