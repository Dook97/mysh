#include "utils.h"

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

void safe_open(const char *file, int flags, int perms) {
	if (open(file, flags, perms) == -1)
		err(SHELL_ERR, "open: %s", file);
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
