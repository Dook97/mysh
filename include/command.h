#ifndef command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
#define command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

#include "utils.h"
#include <sys/queue.h>

enum redir {
	redir_in,
	redir_out,
	redir_append
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

/* safely allocate and initialize a new token */
cmd_tok_t *make_cmd_tok(const char *content);

/* safely allocate and initialize a new command */
cmd_t *make_cmd(void);

/* deallocate cmd_head_t object */
void free_cmd(cmd_t *cmd);

/* append a token to a command */
void cmd_append(cmd_t *cmd, char *content);

/* get cmd_t from cmd_head_t */
void cmd_finalize(cmd_t *out);

void set_redir(cmd_t *cmd, enum redir r, char *file);

#endif
