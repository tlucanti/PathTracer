
#ifndef _CLLIB_COMMON_H
#define _CLLIB_COMMON_H

#define cl_panic_on(expr, msg, error)         \
	do {                                  \
		if (unlikely(expr)) {         \
			cl_panic(msg, error); \
		}                             \
	} while (false)

#define cl_panic(msg, error) __cl_panic(msg, error, __FILE__, __LINE__)

const char *cl_strerror(cl_int error);
void __cl_panic(const char *msg, cl_int error, const char *file,
		unsigned long line);

#endif /* _CLLIB_COMMON_H */
