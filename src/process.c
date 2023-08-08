#include "process.h"

static int shell_cd(cmd_t *cmd) {
	return 1;
}

static int shell_exit(cmd_t *cmd) {
	int exit_code = 0;
	if (cmd->argc == 2) {
		char *arg = cmd->argv[1];

		/* emulate bash's behaviour */
		if (!str_isnum(arg))
			errx(1, "exit: %s: numeric argument required", arg);

		errno = 0; // errno is never set 0 by any syscall or library function
		exit_code = strtol(cmd->argv[1], NULL, 10);
		if (exit_code < 0) {
			if (errno != 0)
				warn("strtol");
			exit_code = 1;
		}
	} else if (cmd->argc > 2) {
		fprintf(stderr, "mysh: exit: too many arguments\n");
		exit_code = 1;
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
