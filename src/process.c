#include "process.h"
#include "builtins.h"
#include "magic.h"
#include "utils.h"
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/wait.h>
#include <unistd.h>

/* helper for set_process_redirs */
static void handle_redir(const redir_t *redir) {
	int file_fd = FD_INVALID;
	int right_fd = redir->right_fd;
	int flags = O_WRONLY | O_CREAT | (redir->type == REDIR_APPEND ? O_APPEND : O_TRUNC);

	switch (redir->type) {
	case REDIR_IN:
		flags = O_RDONLY;
		__attribute__((fallthrough));
	case REDIR_OUT:
	case REDIR_APPEND:
		if (redir->file == NULL)
			errx(SHELL_ERR, "internal shell error: invalid file");
		file_fd = safe_open(redir->file, flags, OPEN_PERMS);
		right_fd = file_fd;
		break;
	case FDREDIR_IN:
	case FDREDIR_OUT:
		/* support for <&- and >&- as required by POSIX */
		if ((redir->file != NULL && !strcmp(redir->file, "-"))) {
			close(redir->left_fd);
			return;
		}
		break;
	}

	if (dup2(right_fd, redir->left_fd) == -1)
		err(errno == EBADF ? USER_ERR : SHELL_ERR, "dup2");

	if (file_fd != FD_INVALID)
		close(file_fd);
}

/* Set I/O files for a process started from exec_cmd.
 *
 * Based on the configuration of cmd sets the input and output files, including pipes, for the
 * process.
 *
 * @param cmd Pointer to a struct holding information about the desired configuration.
 */
static void set_process_redirs(cmd_t *cmd) {
	if ((cmd->pipefd_in != FD_INVALID && dup2(cmd->pipefd_in, FD_STDIN) == -1))
		goto error;
	if (cmd->pipefd_out != FD_INVALID && dup2(cmd->pipefd_out, FD_STDOUT) == -1)
		goto error;

	/* don't unnecessarily leak file descriptors to the child process */
	close_pipe(cmd, true);
	close_pipe(cmd, false);

	redir_tok_t *i;
	STAILQ_FOREACH(i, &cmd->redirlist, next) {
		handle_redir(i->content);
	}

	return;
error:
	err(SHELL_ERR, "dup2");
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
	int stat_loc_internal = 0;

	builtin *func = get_builtin(cmd);
	if (func != NULL) {
		pid = 0;
		stat_loc_internal = func(cmd);
	} else if ((pid = fork()) == 0) {
		set_process_redirs(cmd);
		execvp(cmd->argv[0], cmd->argv);

		/* if exec was successful we shouldn't ever get here */
		int exit_code = (errno == ENOENT) ? UNKNOWN_CMD_ERR : SHELL_ERR;
		err(exit_code, "%s", cmd->argv[0]);
	} else if (pid == -1) {
		warn("fork");
		stat_loc_internal = SHELL_ERR;
	}

	/* closing these is important because otherwise producer/consumer processes on the other
	 * side of the pipe will get stuck (also freeing system resources is a good idea lol)
	 */
	close_pipe(cmd, true);
	close_pipe(cmd, false);

	/* in case the caller is not interested in the output parameter */
	if (stat_loc != NULL)
		*stat_loc = stat_loc_internal;

	return pid;
}

void exec_pipecmd(pipecmd_t *pipecmd) {
	pipecmd_tok_t *pipetok;
	STAILQ_FOREACH(pipetok, &pipecmd->toklist, next) {
		pipecmd_tok_t *nexttok = STAILQ_NEXT(pipetok, next);
		if (nexttok == NULL)
			break;

		/* [0] == read, [1] == write */
		int pipe_fds[2];
		safe_pipe(pipe_fds);
		pipetok->content->pipefd_out = pipe_fds[1];
		nexttok->content->pipefd_in = pipe_fds[0];
		exec_cmd(pipetok->content, NULL);
	}
	int stat_loc = 0;
	pid_t pid = exec_cmd(pipetok->content, &stat_loc);

	/* wait for the last command and get its exit status */
	if (pid > 0 && waitpid(pid, &stat_loc, 0) == -1)
		err(SHELL_ERR, "waitpid");

	/* then wait for the rest */
	while (wait(NULL) != -1)
		;

	if (errno != ECHILD)
		err(SHELL_ERR, "wait");

	sh_exit = get_sh_exit(stat_loc, pid == 0);

	free_pipecmd(pipecmd);
}
