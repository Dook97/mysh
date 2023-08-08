#include "process.h"

void exec_cmd(cmd_head_t *raw_cmd) {
	pid_t pid;
	cmd_t cmd;
	int stat_loc;
	cmd_from_toks(raw_cmd, &cmd);

	builtin *func = NULL;
	if ((func = get_builtin(&cmd)) != NULL) {
		stat_loc = func(&cmd);
	} else if ((pid = fork()) == 0) {
		execvp(cmd.file, cmd.argv);

		// if exec was successful we shouldn't ever get here
		err(SHELL_ERR, "%s", cmd.file);
	} else {
		if (pid == -1) {
			warn("fork");
		} else {
			waitpid(pid, &stat_loc, 0);
		}
	}

	free_cmd(raw_cmd);
	free(cmd.argv);
}
