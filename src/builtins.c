#include "builtins.h"

int shell_cd(cmd_t *cmd) {
	char *new_path = NULL;
	switch (cmd->argc) {
	case 0:
		warnx("cd: internal shell error");
		return 1;
	case 1:
		new_path = getenv("HOME");
		/* same as bash */
		if (new_path == NULL) {
			warnx("cd: HOME not set");
			return 1;
		}
		break;
	case 2:
		new_path = strcmp(cmd->argv[1], "-") == 0 ? getenv("OLDPWD") : cmd->argv[1];
		break;
	default:
		warnx("cd: too many arguments");
		return 1;
	}

	char cur_pwd[PATH_MAX], new_pwd[PATH_MAX];
	if (getcwd(cur_pwd, PATH_MAX) == NULL) {
		warn("cd: getcwd");
		return 1;
	}

	if (new_path == NULL)
		new_path = cur_pwd;

	if (realpath(new_path, new_pwd) == NULL) {
		warn("cd: realpath: %s", new_path);
		return 1;
	}

	if (setenv("PWD", new_pwd, 1) == -1 || setenv("OLDPWD", cur_pwd, 1) == -1) {
		warn("cd: setenv");
		return 1;
	}

	if (chdir(new_pwd) == -1) {
		warn("cd: chdir: %s", new_pwd);
		return 1;
	}

	return 0;
}

int shell_exit(cmd_t *cmd) {
	int exit_code = 0;
	switch (cmd->argc) {
	case 0:
		warnx("exit: internal shell error");
		exit_code = 1;
		break;
	case 1:
		break;
	case 2:
		/* emulate bash's behaviour */
		if (!str_isnum(cmd->argv[1]))
			errx(1, "exit: %s: numeric argument required", cmd->argv[1]);

		errno = 0; // errno is never set 0 by any syscall or library function
		exit_code = strtol(cmd->argv[1], NULL, 10);
		if (exit_code < 0 && errno != 0) {
			warn("exit: strtol");
			exit_code = 1;
		}

		break;
	default:
		warnx("exit: too many arguments");
		return 1;
	}

	exit(exit_code);
}

builtin *get_builtin(cmd_t *cmd) {
	if (strcmp(cmd->file, "exit") == 0) {
		return shell_exit;
	} else if (strcmp(cmd->file, "cd") == 0) {
		return shell_cd;
	}
	return NULL;
}
