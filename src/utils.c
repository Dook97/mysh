#include "utils.h"
#include "magic.h"
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

void *safe_malloc(size_t size) {
	void *ret = malloc(size);
	if (ret == NULL)
		err(SHELL_ERR, "malloc");
	return ret;
}

char *safe_strdup(const char *src) {
	char *copy = strdup(src);
	if (copy == NULL)
		err(SHELL_ERR, "strdup");

	return copy;
}

void safe_pipe(int fildes[2]) {
	if (pipe(fildes) == -1)
		err(SHELL_ERR, "pipe");
}

int safe_open(const char *file, int flags, int perms) {
	int fd;
	if ((fd = open(file, flags, perms)) != -1)
		return fd;

	int code;
	switch (errno) {
	case EACCES:
	case EEXIST:
	case EISDIR:
	case ENOENT:
	case ENOTDIR:
		code = USER_ERR;
		break;
	default:
		code = SHELL_ERR;
		break;
	}

	err(code, "open: %s", file);
}

bool str_isblank(const char *str) {
	for (; *str != '\0'; ++str) {
		if (!isblank(*str))
			return false;
	}
	return true;
}

bool str_isnum(const char *str) {
	for (; *str != '\0'; ++str) {
		if (!isdigit(*str))
			return false;
	}
	return true;
}
