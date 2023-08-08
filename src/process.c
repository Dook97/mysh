#include "process.h"

static int shell_cd(cmd_t *cmd) {
	char *new_path = NULL;
	switch (cmd->argc) {
	case 0:
		warnx("cd: internal shell error");
		return 1;
	case 1:
		new_path = getenv("HOME");
		break;
	case 2:
		new_path = strcmp(cmd->argv[1], "-") == 0 ? getenv("OLDPWD") : cmd->argv[1];
		break;
	default:
		warnx("cd: too many arguments");
		return 1;
	}

	int ret = 1;
	char *cur_pwd = NULL, *new_pwd = NULL;
	if ((cur_pwd = getcwd(NULL, 0)) == NULL) {
		warn("cd: getcwd");
		goto fail;
	}

	if (new_path == NULL)
		new_path = cur_pwd;

	if ((new_pwd = realpath(new_path, NULL)) == NULL) {
		warn("cd: realpath: %s", new_path);
		goto fail;
	}

	if (setenv("PWD", new_pwd, 1) == -1 || setenv("OLDPWD", cur_pwd, 1) == -1) {
		warn("cd: setenv");
		goto fail;
	}

	if (chdir(new_pwd) == -1) {
		warn("cd: chdir: %s", new_pwd);
		goto fail;
	}

	ret = 0;

fail:
	free(cur_pwd);
	free(new_pwd);

	return ret;
}

static int shell_exit(cmd_t *cmd) {
	int exit_code = 0;
	switch (cmd->argc) {
	case 0:
		warnx("cd: internal shell error");
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
			warn("strtol");
			exit_code = 1;
		}

		break;
	default:
		warnx("exit: too many arguments");
		return 1;
	}

	exit(exit_code);
}

void exec_cmd(cmd_head_t *raw_cmd) {
	pid_t pid;
	cmd_t cmd;
	cmd_from_toks(raw_cmd, &cmd);

	if (strcmp(cmd.file, "exit") == 0) {
		shell_exit(&cmd);
	} else if (strcmp(cmd.file, "cd") == 0) {
		shell_cd(&cmd);
	} else if ((pid = fork()) == 0) {
		execvp(cmd.file, cmd.argv);

		// if exec was successful we shouldn't ever get here
		int err_code = (errno == ENOENT) ? 127 : 1;
		err(err_code, "%s", cmd.file);
	} else {
		if (pid == -1) {
			warn("fork");
		} else {
			wait(NULL);
		}
		// TODO: get process exit code so that we can set exit value of the shell
	}

	free_cmd(raw_cmd);
	free(cmd.argv);
}
