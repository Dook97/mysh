#include "process.h"

void exec_cmd(cmd_head_t *raw_cmd) {
	pid_t pid;
	if ((pid = fork()) == 0) {
		cmd_t cmd;
		cmd_from_toks(raw_cmd, &cmd);
		execvp(cmd.file, cmd.args);

		// if exec was successful we shouldn't ever get here
		int err_code = (errno == ENOENT) ? 127 : 1;
		err(err_code, "%s", cmd.file);
	} else {
		wait(NULL);
		// TODO: get process exit code so that we can set exit value of the shell
	}
}
