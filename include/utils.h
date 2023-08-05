#ifndef utils_guard_de8afa52fbaeb5748cb3e963e330713483ce2f3fbc02171c8276638c34505b0c
#define utils_guard_de8afa52fbaeb5748cb3e963e330713483ce2f3fbc02171c8276638c34505b0c

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *safe_malloc(size_t size);
char *safe_strdup(const char *src);
bool str_isblank(const char *line);

#endif
