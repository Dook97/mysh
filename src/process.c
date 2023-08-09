#include "process.h"
#include <fcntl.h>

static void set_redirs(cmd_t *cmd) {
	/* replace stdout with redir */
	if (cmd->out != NULL) {
		if (close(1) == -1)
			err(SHELL_ERR, "close");

		int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
		if (open(cmd->out, flags, OPEN_PERMS) == -1)
			err(SHELL_ERR, "open: %s", cmd->out);
	}

	/* replace stdin with redir */
	if (cmd->in != NULL) {
		if (close(0) == -1)
			err(SHELL_ERR, "close");
		if (open(cmd->in, O_RDONLY) == -1)
			err(SHELL_ERR, "open: %s", cmd->in);
	}
}

static int get_sh_exit(int stat_loc, bool builtin) {
	if (builtin) {
		if (stat_loc != -1) // if a builtin returns -1 it means no change to exit code
			return stat_loc;
	} else {
		if (WIFSIGNALED(stat_loc)) {
			int sig = WTERMSIG(stat_loc);
			fprintf(stderr, "Killed by signal %d.\n", sig);
			return sig + SIG_EXIT_OFFSET;
		} else if (WIFEXITED(stat_loc)) {
			return WEXITSTATUS(stat_loc);
		}
	}
	return sh_exit;
}

void exec_cmd(cmd_t *cmd) {
	pid_t pid;
	int stat_loc;

	cmd_finalize(cmd);

	builtin *func = NULL;
	if ((func = get_builtin(cmd)) != NULL) {
		stat_loc = func(cmd);
	} else if ((pid = fork()) == 0) {
		set_redirs(cmd);
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

	sh_exit = get_sh_exit(stat_loc, func != NULL);
	free_cmd(cmd);
}
