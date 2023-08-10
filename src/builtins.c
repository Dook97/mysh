#include "builtins.h"
#include "magic.h"
#include "utils.h"
#include <err.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* shell exit code */
extern int sh_exit;

int shell_cd(cmd_t *cmd) {
	/* we're not expecting any input, nor non-error output */
	if (cmd->pipefd_in != -1)
		close(cmd->pipefd_in);
	if (cmd->pipefd_out != -1)
		close(cmd->pipefd_out);

	char *new_path = NULL;
	switch (cmd->argc) {
	case 0:
		warnx("cd: internal shell error");
		return SHELL_ERR;
	case 1:
		new_path = getenv("HOME");
		/* same as bash */
		if (new_path == NULL) {
			warnx("cd: HOME not set");
			return SHELL_ERR;
		}
		break;
	case 2:
		new_path = strcmp(cmd->argv[1], "-") == 0 ? getenv("OLDPWD") : cmd->argv[1];
		break;
	default:
		warnx("cd: too many arguments");
		return USER_ERR;
	}

	char cur_pwd[PATH_MAX], new_pwd[PATH_MAX];
	if (getcwd(cur_pwd, PATH_MAX) == NULL) {
		warn("cd: getcwd");
		return SHELL_ERR;
	}

	if (new_path == NULL)
		new_path = cur_pwd;

	if (realpath(new_path, new_pwd) == NULL) {
		warn("cd: realpath: %s", new_path);
		return SHELL_ERR;
	}

	if (setenv("PWD", new_pwd, 1) == -1 || setenv("OLDPWD", cur_pwd, 1) == -1) {
		warn("cd: setenv");
		return SHELL_ERR;
	}

	errno = 0;
	if (chdir(new_pwd) == -1) {
		warn("cd: chdir: %s", new_pwd);
		return (errno == ELOOP || errno == ENAMETOOLONG) ? SHELL_ERR : USER_ERR;
	}

	/* notify user of his new PWD if he used "cd -" */
	if (strcmp(cmd->argv[1], "-") == 0)
		fprintf(stderr, "%s\n", new_pwd);

	return 0;
}

int shell_exit(cmd_t *cmd) {
	/* we're not expecting any input, nor non-error output */
	if (cmd->pipefd_in != -1)
		close(cmd->pipefd_in);
	if (cmd->pipefd_out != -1)
		close(cmd->pipefd_out);

	int exit_code = -1;
	switch (cmd->argc) {
	case 0:
		warnx("exit: internal shell error");
		exit_code = SHELL_ERR;
		break;
	case 1:
		break;
	case 2:
		/* emulate bash's behaviour */
		if (!str_isnum(cmd->argv[1]))
			errx(USER_ERR, "exit: %s: numeric argument required", cmd->argv[1]);

		errno = 0; // errno is never set 0 by any syscall or library function
		exit_code = strtol(cmd->argv[1], NULL, 10);
		if (exit_code < 0 && errno != 0) {
			warn("exit: strtol");
			exit_code = errno == EINVAL ? USER_ERR : SHELL_ERR;
		}

		break;
	default:
		warnx("exit: too many arguments");
		return USER_ERR;
	}

	exit(exit_code == -1 ? sh_exit : exit_code);
}

builtin *get_builtin(cmd_t *cmd) {
	if (strcmp(cmd->file, "exit") == 0) {
		return shell_exit;
	} else if (strcmp(cmd->file, "cd") == 0) {
		return shell_cd;
	}
	return NULL;
}
