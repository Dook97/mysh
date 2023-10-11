#include "magic.h"
#include "utils.h"
#include <err.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <readline/history.h>
#include <readline/readline.h>

/* run shell over an in-memory string */
int shell_str(const char *str);
/* run shell over a file */
int shell_file(FILE *f);

/* shell exit code */
int sh_exit = 0;

static void sigint_handler(int sig) {
	(void)sig;

	/* move readline to a new empty line */
	rl_crlf();
	rl_on_new_line();
	rl_replace_line("", 0);

	/* if readline isn't currently awaiting input, we're done */
	if (rl_done)
		return;

	rl_redisplay();

	/* reset position in history to latest item */
	history_set_pos(history_length);
}

/* Get a shell prompt string.
 *
 * If the path to PWD isn't too long it will be displayed in the prompt, otherwise a shortened
 * version will be presented.
 *
 * @param buf A buffer which may store the prompt.
 * @param bufsize The size of the buffer.
 * @return A pointer to the prompt, which may or may not be the address of the buffer.
 */
static char *get_prompt(char buf[], size_t bufsize) {
	char getcwd_buf[PATH_MAX];
	char *pwd = getcwd(getcwd_buf, PATH_MAX);
	if (pwd == NULL)
		getcwd_buf[0] = '\0';

	int ret = snprintf(buf, bufsize, PROMPT_BASE ":%s$ ", getcwd_buf);
	return (ret == -1 || (size_t)ret >= bufsize || pwd == NULL) ? PROMPT_BASE "$" : buf;
}

/* Run the shell in interactive mode. */
static void interactive(void) {
	struct sigaction sa = {
		.sa_handler = sigint_handler,
		.sa_flags = SA_RESTART,
	};
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGINT, &sa, NULL) == -1)
		err(SHELL_ERR, "sigaction: failed to register SIGINT handler");

	char prompt_buf[PROMPT_BUFSIZE];
	char *prompt;

	char *line = NULL;
	while (prompt = get_prompt(prompt_buf, sizeof(prompt_buf)), (line = readline(prompt)) != NULL) {
		if (!str_isblank(line)) {
			add_history(line);
			shell_str(line);
		}
		free(line);
	}
	/* ^D was pressed */
	fprintf(stderr, "exit\n");
	rl_clear_history();
}

/* Run the shell using a file for input.
 *
 * @param path The path to the file to be executed by the shell.
 */
static void filemode(const char *path) {
	FILE *f = fopen(path, "r");
	if (f == NULL) {
		warn("%s", path);
		sh_exit = USER_ERR;
		return;
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
		/* -c option works same as bash */
		if (strcmp("-c", argv[1]) == 0) {
			shell_str(argv[2]);
			break;
		}
		__attribute__((fallthrough));
	default:
		warnx("Usage: %s [FILE | -c \"...\"]", argv[0]);
		sh_exit = USER_ERR;
		break;
	}

	return sh_exit;
}
