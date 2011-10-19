#ifndef _DEBUG_H_ 
#define _DEBUG_H_

#include <stdio.h>

#define TRACE(format, ...) printf("%s" format "\n", DEBUG_DOMAIN ": ", ##__VA_ARGS__)
#define WARN(format, ...) \
	fprintf(stderr, "%s" format "\n", DEBUG_DOMAIN ": ", ##__VA_ARGS__)

#endif
