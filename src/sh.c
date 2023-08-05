#include "command.h"
#include "utils.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>

#define PROMPT "mysh$ "

int scan_str(const char *str, cmdlist_head_t **out);
int scan_file(FILE *f, cmdlist_head_t **out);

static void sigint_handler(int sig) {
	(void)sig;

	/* move readline to a new empty line */
	rl_crlf();
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();

	/* reset position in history to latest item */
	history_set_pos(history_length);
}

static void print_cmdlist(cmdlist_head_t *list) {
	cmdlist_tok_t *cmdtok = NULL;
	STAILQ_FOREACH(cmdtok, list, next) {
		cmd_tok_t *tok = NULL;
		STAILQ_FOREACH(tok, cmdtok->content, next) {
			printf("%s ", tok->content);
		}
		puts("");
	}
}

static int interactive(void) {
	signal(SIGINT, sigint_handler);

	char *line;
	int ret = 0;
	cmdlist_head_t *cmdlist = NULL;
	while ((line = readline(PROMPT)) != NULL) {
		if (!str_isblank(line)) {
			add_history(line);
			if ((ret = scan_str(line, &cmdlist)))
				continue;
#ifdef DEBUG
			print_cmdlist(cmdlist);
#endif
		}
		free(line);
	}
	rl_clear_history();

	return ret;
}

static int filemode(const char *path) {
	FILE *f = fopen(path, "r");
	if (f == NULL) {
		perror(path);
		return 1;
	}

	cmdlist_head_t *cmdlist = NULL;
	int ret = scan_file(f, &cmdlist);

#ifdef DEBUG
	print_cmdlist(cmdlist);
#endif

	fclose(f);
	return ret;
}

int main(int argc, char **argv) {
	int ret;

	if (argc == 1) {
		ret = interactive();
	} else if (argc == 2) {
		ret = filemode(argv[1]);
	} else {
		ret = 1;
	}

	return ret;
}
