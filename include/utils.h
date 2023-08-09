#ifndef utils_guard_de8afa52fbaeb5748cb3e963e330713483ce2f3fbc02171c8276638c34505b0c
#define utils_guard_de8afa52fbaeb5748cb3e963e330713483ce2f3fbc02171c8276638c34505b0c

#include "magic.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#define STAILQ_LEN(entry_type, head, next_member)                            \
	({                                                                   \
		__auto_type safe_head__ = head;                              \
		size_t out__ = 0;                                            \
		entry_type *var__;                                           \
		STAILQ_FOREACH(var__, safe_head__, next_member) { ++out__; } \
		out__;                                                       \
	})

#define TOKS_TO_ARR(entry_type, head, next_member, content_member, content_type)            \
	({                                                                                  \
		__auto_type safe_head2__ = head;                                            \
		size_t len__ = STAILQ_LEN(entry_type, safe_head2__, next_member);           \
		content_type *toks_arr__ = safe_malloc((len__ + 1) * sizeof(content_type)); \
		entry_type *var__ = STAILQ_FIRST(safe_head2__);                             \
		for (size_t i__ = 0; i__ < len__; ++i__) {                                  \
			toks_arr__[i__] = var__->content_member;                            \
			var__ = STAILQ_NEXT(var__, next_member);                            \
		}                                                                           \
		toks_arr__[len__] = NULL;                                                   \
		toks_arr__;                                                                 \
	})

void *safe_malloc(size_t size);
char *safe_strdup(const char *src);
bool str_isblank(const char *line);
bool str_isnum(const char *str);

#endif
