#include "process.h"

void exec_cmd(cmd_head_t *raw_cmd) {
	pid_t pid;
	if ((pid = fork()) == 0) {
		cmd_t cmd;
		cmd_from_toks(raw_cmd, &cmd);
		execvp(cmd.file, cmd.args);
		err(1, "%s", cmd.file);
	} else {
		wait(NULL);
		// TODO: get process exit code so that we can set exit value of the shell
	}
}
