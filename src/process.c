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

/* helper for set_process_redirs */
static void fd_replace(const char *file, int replaced, int replacement, int flags, int perms) {
	if (file != NULL) {
		if (replacement != -1 && close(replacement) == -1)
			err(SHELL_ERR, "close");
		replacement = safe_open(file, flags, perms);
	}

	if (replacement != -1) {
		if (dup2(replacement, replaced) == -1)
			err(SHELL_ERR, "dup2");
		if (close(replacement) == -1)
			err(SHELL_ERR, "close");
	}
}

/* Set I/O files for a process started from exec_cmd.
 *
 * Based on the configuration of cmd sets the input and output files, including pipes, for the
 * process.
 *
 * @param cmd Pointer to a struct holding information about the desired configuration.
 */
static void set_process_redirs(cmd_t *cmd) {
	int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
	fd_replace(cmd->out, FD_STDOUT, cmd->pipefd_out, flags, OPEN_PERMS);

	fd_replace(cmd->in, FD_STDIN, cmd->pipefd_in, O_RDONLY, 0);
}

/* Based on the value obtained from either exec_cmd() or wait() yield a proper exit code for the
 * shell. Also notifies the user if the command was killed via a signal.
 *
 * The behaviour differs based on whether the exit_code was obtained from a process or a shell
 * builtin command.
 *
 * @param stat_loc Value obtained via wait()
 * @param builtin True if stat_loc comes from a shell builtin rather than wait()-ing on a process.
 * @return New shell exit code.
 */
static int get_sh_exit(int stat_loc, bool builtin) {
	/* if a builtin returns -1 it means no change to exit code */
	if (builtin && stat_loc != BUILTIN_DISCARD_EXIT)
		return stat_loc;

	if (WIFEXITED(stat_loc))
		return WEXITSTATUS(stat_loc);

	if (WIFSIGNALED(stat_loc)) {
		int sig = WTERMSIG(stat_loc);
		fprintf(stderr, "Killed by signal %d.\n", sig);
		return sig + SIG_EXIT_OFFSET;
	}

	return sh_exit;
}

/* Execute a simple command.
 *
 * Forks into a new process and executes the command specified in cmd ensuring redirection of I/O
 * and other parameters.
 *
 * @param cmd Pointer to a structure representing the command.
 * @param stat_loc Output parameter.
 * @return pid of the started child process.
 */
static pid_t exec_cmd(cmd_t *cmd, int *stat_loc) {
	pid_t pid;

	builtin *func = NULL;
	if ((func = get_builtin(cmd)) != NULL) {
		pid = 0;
		*stat_loc = func(cmd);
	} else if ((pid = fork()) == 0) {
		set_process_redirs(cmd);
		execvp(cmd->file, cmd->argv);

		/* if exec was successful we shouldn't ever get here */
		int exit_code = (errno == ENOENT) ? UNKNOWN_CMD_ERR : SHELL_ERR;
		err(exit_code, "%s", cmd->file);
	} else if (pid == -1) {
		warn("fork");
		*stat_loc = SHELL_ERR;
	}

	/* closing these is important because otherwise producer/consumer processes on the other
	 * side of the pipe will get stuck (also freeing system resources is a good idea lol)
	 */
	if ((cmd->pipefd_out != -1 && close(cmd->pipefd_out) == -1)
	    || (cmd->pipefd_in != -1 && close(cmd->pipefd_in) == -1))
		warn("close");

	return pid;
}

void exec_pipecmd(pipecmd_t *pipecmd) {
	pipecmd_finalize(pipecmd);

	for (size_t i = 0; i < pipecmd->cmd_count - 1; ++i) {
		/* [0] == read, [1] == write */
		int pipe_fds[2];
		cmd_t *cur = pipecmd->cmds[i], *next = pipecmd->cmds[i + 1];

		safe_pipe(pipe_fds);
		cur->pipefd_out = pipe_fds[1];
		next->pipefd_in = pipe_fds[0];
		exec_cmd(cur, NULL);
	}
	int stat_loc = 0;
	pid_t pid = exec_cmd(pipecmd->cmds[pipecmd->cmd_count - 1], &stat_loc);

	/* wait for the last command and get its exit status */
	if (pid > 0)
		waitpid(pid, &stat_loc, 0);

	/* then wait for the rest */
	do {
		errno = 0;
		wait(NULL);
	} while (errno != ECHILD);

	sh_exit = get_sh_exit(stat_loc, pid == 0);

	free_pipecmd(pipecmd);
}
