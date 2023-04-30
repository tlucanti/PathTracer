
#include <common.h>

__cold __noreturn
void __panic(const char *msg, const char *file, unsigned long line)
{
	printf("%s:%ld\npanic: %s\n", file, line, msg);
	abort();
}

__cold
void __warn(const char *msg, const char *file, unsigned long line)
{
	printf("%s:%lu\nwarn: %s\n", file, line, msg);
}
