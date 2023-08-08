#ifndef builtins_guard_83fc996e25d379b0623f480c1fdc8d7af80bea7f85a3dcd30078cf776bd130dc
#define builtins_guard_83fc996e25d379b0623f480c1fdc8d7af80bea7f85a3dcd30078cf776bd130dc

#include "command.h"
#include <err.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

typedef int (builtin)(cmd_t *cmd);

int shell_cd(cmd_t *cmd);
int shell_exit(cmd_t *cmd);
builtin *get_builtin(cmd_t *cmd);

#endif
