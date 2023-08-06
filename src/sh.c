#include "command.h"
#include "utils.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int scan_str(const char *str);
int scan_file(FILE *f);

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

static int interactive(void) {
	signal(SIGINT, sigint_handler);
	const char *prompt = "mysh$ ";

	int ret = 0;
	char *line = NULL;
	while ((line = readline(prompt)) != NULL) {
		if (!str_isblank(line)) {
			add_history(line);
			if ((ret = scan_str(line)))
				continue;
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

	int ret = scan_file(f);

	fclose(f);
	return ret;
}

int main(int argc, char **argv) {
	switch (argc) {
	case 1:
		return interactive();
	case 2:
		return filemode(argv[1]);
	case 3:
		if (strcmp("-c", argv[1]) == 0)
			return scan_str(argv[2]);
		__attribute__((fallthrough));
	default:
		fprintf(stderr, "Usage: %s [FILE | -c \"...\"]\n", argv[0]);
		return 1;
	}
}
