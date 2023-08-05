#include "utils.h"

void *safe_malloc(size_t size) {
	void *ret = malloc(size);
	if (ret == NULL) {
		perror("malloc");
		exit(1);
	}
	return ret;
}

char *safe_strdup(const char *src) {
	char *copy = strdup(src);
	if (copy == NULL) {
		perror("stdup");
		exit(1);
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
