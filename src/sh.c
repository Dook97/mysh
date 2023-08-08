#include "utils.h"
#include "magic.h"
#include <err.h>
#include <linux/limits.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int shell_str(const char *str);
int shell_file(FILE *f);

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

static char *get_prompt(char buf[], size_t bufsize) {
	char getcwd_buf[PATH_MAX];
	char *pwd = getcwd(getcwd_buf, PATH_MAX);
	if (pwd == NULL)
		getcwd_buf[0] = '\0';

	int ret = snprintf(buf, bufsize, PROMPT_BASE ":%s$ ", getcwd_buf);
	return (ret == -1 || (size_t)ret >= bufsize || pwd == NULL) ? PROMPT_BASE "$" : buf;
}

static int interactive(void) {
	signal(SIGINT, sigint_handler);

	char prompt_buf[PROMPT_BUFSIZE];
	char *prompt;

	int ret = 0;
	char *line = NULL;
	while (prompt = get_prompt(prompt_buf, sizeof(prompt_buf)),
	       (line = readline(prompt)) != NULL) {
		if (!str_isblank(line)) {
			add_history(line);
			if ((ret = shell_str(line)) != 0)
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
		warn("%s", path);
		return SHELL_ERR;
	}

	int ret = shell_file(f);

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
			return shell_str(argv[2]);
		__attribute__((fallthrough));
	default:
		warnx("Usage: %s [FILE | -c \"...\"]\n", argv[0]);
		return SHELL_ERR;
	}
}
