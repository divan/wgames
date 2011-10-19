#ifndef _OVERRIDE_H_
#define _OVERRIDE_H_

#include <dlfcn.h>

#define STRINGIFY(s) str(s)
#define str(s) #s

#define OVERRIDES(func, args, return_value) \
	char * dl_error; \
	static return_value (* super_func) args = NULL; \
	if (!SUPER) { \
		dlerror(); \
		super_func = dlsym(RTLD_NEXT, STRINGIFY(func)); \
		if ((dl_error = dlerror()) != NULL) { \
			fprintf(stderr, "LINKER: %s\n", dl_error); \
			exit(EXIT_FAILURE); \
		} \
	}

#define SUPER (*super_func)

#endif
