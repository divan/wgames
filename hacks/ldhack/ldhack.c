/*
 * This is LD_PRELOAD hack that changes fopen path from
 * LD_HACK_FROM to LD_HACK_TO directories.
 */
// vim: tw=4 et
#define _GNU_SOURCE 1
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

// This is special case, so let's handle it
// in hardcoded way
#define POWERVR_DIR "/etc/powervr.d/"

FILE * (*orig_fopen)(const char *, const char * mode);

char * get_replaced(const char * path,
                    const char * from_str,
                    const char * to_str);
FILE * run_fopen(const char * path,
                 const char * from_str,
                 const char * to_str,
                 const char * mode);
FILE * fopen(const char * path, const char * mode);

/*
 * Main fake function
 */
FILE * fopen(const char * path, const char * mode)
{
    orig_fopen = dlsym(RTLD_NEXT, "fopen");

    char * powervr_str = getenv("LD_POWERVR_DIR");
    if (NULL != powervr_str &&
        NULL != strstr(path, POWERVR_DIR))
    {
        return run_fopen(path, POWERVR_DIR,
                         powervr_str, mode);
    }

    char * from_str = getenv("LD_HACK_FROM");
    char * to_str = getenv("LD_HACK_TO");
    if (NULL != from_str && NULL != to_str &&
        NULL != strstr(path, from_str))
    {
        return run_fopen(path, from_str, to_str, mode);
    }

    // Run fopen without changes
    return orig_fopen(path, mode);
}

/*
 * Wrapper for fopen call
 */
FILE * run_fopen(const char * path,
                 const char * from_str,
                 const char * to_str,
                 const char * mode)
{
    char * new = get_replaced(path,
                             from_str,
                             to_str);
    FILE * ret = orig_fopen(new, mode);
    free(new);
    return ret;
}


/*
 * Replace base directory from 'from_str'
 * to 'to_str'.
 * Note: memory must be freed by caller.
 */
char * get_replaced(const char * path,
                    const char * from_str,
                    const char * to_str)
{
    size_t from_len = strlen(from_str);
    size_t to_len = strlen(to_str);
    size_t tail_len = strlen(path) - from_len;

    char * tailp = (char *)(path + from_len);
    char * new = calloc(to_len + tail_len + 1, 1);
    strncpy(new, to_str, to_len);
    strncpy(new + to_len, tailp, tail_len);

    printf("LD_HACK: %s -> %s\n", from_str, new);
    return new;
}
