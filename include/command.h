#ifndef command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
#define command_guard_e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

#include "utils.h"
#include <sys/queue.h>

/* linked list of tokens in a command */
typedef struct cmd_tok {
	char *content;
	STAILQ_ENTRY(cmd_tok) next;
} cmd_tok_t;

STAILQ_HEAD(cmd_head, cmd_tok);
typedef struct cmd_head cmd_head_t;

/* linked list of commands */
typedef struct cmdlist_tok {
	cmd_head_t *content;
	STAILQ_ENTRY(cmdlist_tok) next;
} cmdlist_tok_t;

STAILQ_HEAD(cmdlist_head, cmdlist_tok);
typedef struct cmdlist_head cmdlist_head_t;

/* safely allocate and initialize a new token */
cmd_tok_t *make_cmd_tok(const char *content);

/* safely allocate and initialize a new command */
cmd_head_t *make_cmd(void);

/* safely allocate and initialize a new token */
cmdlist_tok_t *make_cmdlist_tok(cmd_head_t *content);

/* safely allocate and initialize a new list of commands */
cmdlist_head_t *make_cmdlist(void);

/* append a token to a command */
void cmd_append(cmd_head_t *cmd, char *content);

/* append a command to a list of commands */
void cmdlist_append(cmdlist_head_t *list, cmd_head_t *content);

#endif
