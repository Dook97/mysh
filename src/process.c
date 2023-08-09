#include "process.h"

void exec_cmd(cmd_t *cmd) {
	pid_t pid;
	int stat_loc;

	cmd_finalize(cmd);

	builtin *func = NULL;
	if ((func = get_builtin(cmd)) != NULL) {
		stat_loc = func(cmd);
	} else if ((pid = fork()) == 0) {
		execvp(cmd->file, cmd->argv);

		// if exec was successful we shouldn't ever get here
		int exit_code = errno == ENOENT ? UNKNOWN_CMD_ERR : SHELL_ERR;
		err(exit_code, "%s", cmd->file);
	} else {
		if (pid == -1) {
			warn("fork");
		} else {
			waitpid(pid, &stat_loc, 0);
		}
	}

	if (func != NULL) {
		if (stat_loc != -1) // if a builtin returns -1 it means no change to exit code
			sh_exit = stat_loc;
	} else {
		if (WIFSIGNALED(stat_loc)) {
			int sig = WTERMSIG(stat_loc);
			fprintf(stderr, "Killed by signal %d.\n", sig);
			sh_exit = sig + SIG_EXIT_OFFSET;
		} else if (WIFEXITED(stat_loc)) {
			sh_exit = WEXITSTATUS(stat_loc);
		}
	}

	free_cmd_toklist(&cmd->toklist);
	free(cmd->argv);
	free(cmd);
}
