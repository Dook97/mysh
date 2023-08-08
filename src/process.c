#include "process.h"

void exec_cmd(cmd_head_t *raw_cmd) {
	pid_t pid;
	cmd_t cmd;
	cmd_from_toks(raw_cmd, &cmd);

	builtin *func = NULL;
	if ((func = get_builtin(&cmd)) != NULL) {
		func(&cmd);
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
