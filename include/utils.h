#ifndef utils_guard_de8afa52fbaeb5748cb3e963e330713483ce2f3fbc02171c8276638c34505b0c
#define utils_guard_de8afa52fbaeb5748cb3e963e330713483ce2f3fbc02171c8276638c34505b0c

#include "command.h"
#include <stdbool.h>
#include <unistd.h>

#define STAILQ_LEN(entry_type, head, next_member)                                       \
	({                                                                              \
		/* get number of elements in a STAILQ instance */                       \
                                                                                        \
		__auto_type STAILQ_LEN_safe_head__ = head;                              \
		size_t STAILQ_LEN_out__ = 0;                                            \
		entry_type *STAILQ_LEN_var__;                                           \
		STAILQ_FOREACH(STAILQ_LEN_var__, STAILQ_LEN_safe_head__, next_member) { \
			++STAILQ_LEN_out__;                                             \
		}                                                                       \
		STAILQ_LEN_out__;                                                       \
	})

#define STAILQ_TO_ARR(entry_type, head, next_member, content_member, content_type)           \
	({                                                                                   \
		/* convert STAILQ to a NULL-terminated array */                              \
                                                                                             \
		__auto_type STAILQ_TO_ARR_safe_head__ = head;                                \
		size_t STAILQ_TO_ARR_len__ =                                                 \
			STAILQ_LEN(entry_type, STAILQ_TO_ARR_safe_head__, next_member);      \
		content_type *STAILQ_TO_ARR_res__ =                                          \
			safe_malloc((STAILQ_TO_ARR_len__ + 1) * sizeof(content_type));       \
		entry_type *STAILQ_TO_ARR_var__ = STAILQ_FIRST(STAILQ_TO_ARR_safe_head__);   \
		for (size_t i__ = 0; i__ < STAILQ_TO_ARR_len__; ++i__) {                     \
			STAILQ_TO_ARR_res__[i__] = STAILQ_TO_ARR_var__->content_member;      \
			STAILQ_TO_ARR_var__ = STAILQ_NEXT(STAILQ_TO_ARR_var__, next_member); \
		}                                                                            \
		STAILQ_TO_ARR_res__[STAILQ_TO_ARR_len__] = NULL;                             \
		STAILQ_TO_ARR_res__;                                                         \
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

/* safely closes either the input or output pipe member of a cmd_t */
int close_pipe(cmd_t *cmd, bool input_pipe);

#endif
