#include "utils.h"
#include "command.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>

#define PROMPT "mysh$ "

void flex_scan_str(const char *str, cmdlist_head_t **out);
void flex_scan_file(FILE *f, cmdlist_head_t **out);

static cmdlist_head_t *out;

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

static int shell_interactive(void) {
	signal(SIGINT, sigint_handler);

	char *line;
	while ((line = readline(PROMPT)) != NULL) {
		if (!str_isblank(line)) {
			add_history(line);
			flex_scan_str(line, &out);
		}
		free(line);
	}
	rl_clear_history();

	return 0;
}

static int shell_filemode(const char *path) {
	FILE *f = fopen(path, "r");
	if (f == NULL) {
		perror(path);
		return 1;
	}

	flex_scan_file(f, &out);
	fclose(f);

	return 0;
}

int main(int argc, char **argv) {
	int ret;

	out = NULL;

	if (argc == 1) {
		ret = shell_interactive();
	} else {
		ret = shell_filemode(argv[1]);
	}

	return ret;
}
