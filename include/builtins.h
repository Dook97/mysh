#ifndef builtins_guard_83fc996e25d379b0623f480c1fdc8d7af80bea7f85a3dcd30078cf776bd130dc
#define builtins_guard_83fc996e25d379b0623f480c1fdc8d7af80bea7f85a3dcd30078cf776bd130dc

#include "command.h"

typedef int(builtin)(cmd_t *cmd);

/* Shell builtin command to change the PWD.
 *
 * @param cmd Object holding arguments etc.
 * @return exit code
 * @retval -1 Shell exit code is to remain unchanged.
 */
int shell_cd(cmd_t *cmd);

/* Shell builtin command to exit the shell.
 *
 * @param cmd Object holding arguments etc.
 * @return exit code
 * @retval -1 Shell exit code is to remain unchanged.
 */
int shell_exit(cmd_t *cmd);

/* Get a pointer to a function representing a builtin command.
 *
 * @param cmd Object holding information about a command.
 * @return Pointer to matching builtin command function.
 * @retval NULL if no builtin matches the command.
 */
builtin *get_builtin(cmd_t *cmd);

#endif
