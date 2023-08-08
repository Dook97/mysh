#include "utils.h"

void *safe_malloc(size_t size) {
	void *ret = malloc(size);
	if (ret == NULL) {
		perror("malloc");
		exit(SHELL_ERR);
	}
	return ret;
}

char *safe_strdup(const char *src) {
	char *copy = strdup(src);
	if (copy == NULL) {
		perror("stdup");
		exit(SHELL_ERR);
	}
	return copy;
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
