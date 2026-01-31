/*
 * dirent.h - Minimal stub for embedded ARM builds
 * 
 * This stub header prevents compilation errors when libfizmo includes <dirent.h>.
 * The directory functions are not actually used when DISABLE_CONFIGFILES=1.
 */

#ifndef _DIRENT_H_STUB
#define _DIRENT_H_STUB

#ifdef __ARM_EABI__

/* Minimal type definitions to satisfy compilation */
typedef struct {
    void *__opaque;
} DIR;

struct dirent {
    char *d_name;
};

/* Function declarations (implementations not needed - unused when DISABLE_CONFIGFILES=1) */
DIR *opendir(const char *dirname);
int closedir(DIR *dirp);
struct dirent *readdir(DIR *dirp);
void rewinddir(DIR *dirp);

#else
/* On non-ARM platforms, use the real dirent.h */
#include_next <dirent.h>
#endif

#endif /* _DIRENT_H_STUB */
