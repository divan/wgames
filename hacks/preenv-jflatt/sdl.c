#include <string.h>

/* So why do I have to implement these? Weren't those supposed to be inlined? */

void * SDL_memcpy(void *dst, const void *src, size_t len)
{
	return memcpy(dst, src, len);
}

int SDL_memcmp(const void *s1, const void *s2, size_t len)
{
	return memcmp(s1, s2, len);
}

