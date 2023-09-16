#ifndef utils_guard_de8afa52fbaeb5748cb3e963e330713483ce2f3fbc02171c8276638c34505b0c
#define utils_guard_de8afa52fbaeb5748cb3e963e330713483ce2f3fbc02171c8276638c34505b0c

#include <stdbool.h>
#include <unistd.h>

#define STAILQ_LEN(entry_type, head, next_member)                 \
	({                                                        \
		/* get number of elements in a STAILQ instance */ \
                                                                  \
		__auto_type safe_head__ = head;                   \
		size_t out__ = 0;                                 \
		entry_type *var__;                                \
		STAILQ_FOREACH(var__, safe_head__, next_member) { \
			++out__;                                  \
		}                                                 \
		out__;                                            \
	})

#define TOKS_TO_ARR(entry_type, head, next_member, content_member, content_type)            \
	({                                                                                  \
		/* convert STAILQ to a NULL-terminated array */                             \
                                                                                            \
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

/* malloc() except it kills the program with an err message on failure. */
void *safe_malloc(size_t size);

/* strdup() except it kills the program with an err message on failure. */
char *safe_strdup(const char *src);

/* pipe() except it kills the program with an err message on failure. */
void safe_pipe(int fildes[2]);

/* open() except it kills the program with an err message on failure. */
int safe_open(const char *file, int flags, int perms);

/* true if for every char c in str isblank(c) returns non-zero. */
bool str_isblank(const char *str);

/* true if for every char c in str isdigit(c) returns non-zero. */
bool str_isnum(const char *str);

#endif
