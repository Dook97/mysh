#include "magic.h"
#include "utils.h"
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

/* shell exit code */
int sh_exit = 0;

static void sigint_handler(int sig) {
	(void)sig;

	/* if readline is not currently awaiting input don't do anything */
	if (rl_done)
		return;

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

static void interactive(void) {
	signal(SIGINT, sigint_handler);

	char prompt_buf[PROMPT_BUFSIZE];
	char *prompt;

	char *line = NULL;
	while (prompt = get_prompt(prompt_buf, sizeof(prompt_buf)),
	       (line = readline(prompt)) != NULL) {
		if (!str_isblank(line)) {
			add_history(line);
			if (shell_str(line) != 0)
				continue;
		}
		free(line);
	}
	fprintf(stderr, "exit\n");
	rl_clear_history();
}

static void filemode(const char *path) {
	FILE *f = fopen(path, "r");
	if (f == NULL) {
		warn("%s", path);
		sh_exit = USER_ERR;
	}

	shell_file(f);

	fclose(f);
}

int main(int argc, char **argv) {
	switch (argc) {
	case 1:
		interactive();
		break;
	case 2:
		filemode(argv[1]);
		break;
	case 3:
		if (strcmp("-c", argv[1]) == 0) {
			shell_str(argv[2]);
			break;
		}
		__attribute__((fallthrough));
	default:
		warnx("Usage: %s [FILE | -c \"...\"]", argv[0]);
		sh_exit = USER_ERR;
	}

	return sh_exit;
}
