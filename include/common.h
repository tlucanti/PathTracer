
#ifndef _COMMON_H
# define _COMMON_H

# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>

# ifndef __always_inline
#  define __always_inline inline __attribute__((__always_inline__))
# endif
# ifndef __cold
#  define __cold __attribute__((__cold__))
# endif
# ifndef __noreturn
#  define __noreturn __attribute__((__noreturn__))
# endif
# ifndef __used
#  define __used __attribute__((__used__))
# endif
# ifndef static_assert
#  define static_assert _Static_assert
# endif

# define likely(expr) __builtin_expect(!!(expr), 1)
# define unlikely(expr) __builtin_expect(!!(expr), 0)

# define panic(msg) __panic(msg, __FILE__, __LINE__)
# define warn(msg) __warn(msg, __FILE__, __LINE__)
# define panic_on(expr, msg) do {	\
	if (unlikely(expr)) {		\
		panic(msg);		\
	}				\
} while (false)

__cold __noreturn void __panic(const char *msg, const char *file, unsigned long line);
__cold void __warn(const char *msg, const char *file, unsigned long line);

#endif /* _COMMON_H */
