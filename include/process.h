#ifndef process_guard_fb920e1894c5960f029d275523605fd9241815b39387b0ac8d1e47597e4be6cc
#define process_guard_fb920e1894c5960f029d275523605fd9241815b39387b0ac8d1e47597e4be6cc

#include "command.h"
#include <err.h>
#include <sys/wait.h>
#include <unistd.h>

void exec_cmd(cmd_head_t *cmd);

#endif
