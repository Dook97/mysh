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
typedef struct cmd_list_tok {
	cmd_head_t cmd;
	STAILQ_ENTRY(cmd_list_tok) next;
} cmd_list_tok_t;

STAILQ_HEAD(cmdlist_head, cmd_list_tok);
typedef struct cmdlist_head cmdlist_head_t;

/* safely allocate and initialize new token */
cmd_tok_t *make_tok(const char *content);

#endif
