#include "process.h"
#include "builtins.h"
#include "magic.h"
#include "utils.h"
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/* shell exit code */
extern int sh_exit;

// TODO: handle close() and dup() errors
static void set_process_redirs(cmd_t *cmd) {
	/* replace stdout with redir */
	if (cmd->out != NULL) {
		close(1);
		int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
		safe_open(cmd->out, flags, OPEN_PERMS);
	} else if (cmd->pipefd_out >= 0) {
		close(1);
		dup(cmd->pipefd_out);
	}

	/* replace stdin with redir */
	if (cmd->in != NULL) {
		close(0);
		safe_open(cmd->in, O_RDONLY, 0);
	} else if (cmd->pipefd_in >= 0) {
		close(0);
		dup(cmd->pipefd_in);
	}

	close(cmd->pipefd_out);
	close(cmd->pipefd_in);
}

static int get_sh_exit(int stat_loc, bool builtin) {
	/* if a builtin returns -1 it means no change to exit code */
	if (builtin && stat_loc != -1) {
		return stat_loc;
	} else if (WIFSIGNALED(stat_loc)) {
		int sig = WTERMSIG(stat_loc);
		fprintf(stderr, "Killed by signal %d.\n", sig);
		return sig + SIG_EXIT_OFFSET;
	} else if (WIFEXITED(stat_loc)) {
		return WEXITSTATUS(stat_loc);
	} else {
		return sh_exit;
	}
}

static pid_t exec_cmd(cmd_t *cmd, int *stat_loc) {
	pid_t pid;

	builtin *func = NULL;
	if ((func = get_builtin(cmd)) != NULL) {
		pid = 0;
		*stat_loc = func(cmd);
	} else if ((pid = fork()) == 0) {
		set_process_redirs(cmd);
		execvp(cmd->file, cmd->argv);

		// if exec was successful we shouldn't ever get here
		int exit_code = errno == ENOENT ? UNKNOWN_CMD_ERR : SHELL_ERR;
		err(exit_code, "%s", cmd->file);
	}

	close(cmd->pipefd_out);
	close(cmd->pipefd_in);
	/* invalidating to prevent double-close in free_cmd() */
	cmd->pipefd_out = -1;
	cmd->pipefd_in = -1;

	return pid;
}

void exec_pipecmd(pipecmd_t *pipecmd) {
	pipecmd_finalize(pipecmd);

	pid_t pid;
	int stat_loc = 0;
	for (size_t i = 0; i < pipecmd->cmd_count; ++i)
		pid = exec_cmd(pipecmd->cmds[i], &stat_loc);

	if (pid > 0)
		waitpid(pid, &stat_loc, 0);

	do {
		errno = 0;
		wait(NULL);
	} while (errno != ECHILD);

	sh_exit = get_sh_exit(stat_loc, pid == 0);

	free_pipecmd(pipecmd);
}
