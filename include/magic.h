#ifndef magic_guard_81419ca4d428f38357035311c3573c5acf1617cc117f364947c6d4dc54c86f6d
#define magic_guard_81419ca4d428f38357035311c3573c5acf1617cc117f364947c6d4dc54c86f6d

/* shell exit code */
extern int sh_exit;

#define PROMPT_BASE "mysh"
#define PROMPT_BUFSIZE 256

/* exit codes */
#define USER_ERR 1
#define SHELL_ERR 2
#define PARSING_ERR 1
#define UNKNOWN_CMD_ERR 127
#define NOT_IMPLEMENTED_ERR 255

/* if a process is killed by a signal use this value + SIGVAL to determine shell exit code */
#define SIG_EXIT_OFFSET 128

/* permission bits used by the shell when creating new files
 * rw-r--r--
 */
#define OPEN_PERMS 0644

/* common file descriptors */
#define FD_INVALID -1
#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2

/* value returned by a shell builtin signalling that the exit code of the shell should remain
 * unchanged after the builtin's execution
 */
#define BUILTIN_DISCARD_EXIT -1

#endif
