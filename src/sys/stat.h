/*
 * sys/stat.h - Minimal stub for embedded ARM builds
 * 
 * This stub header prevents compilation errors when libfizmo includes <sys/stat.h>.
 * The stat functions are not actually used when DISABLE_CONFIGFILES=1.
 */

#ifndef _SYS_STAT_H_STUB
#define _SYS_STAT_H_STUB

#ifdef __ARM_EABI__

/* Minimal type/constant definitions */
struct stat {
    long st_mode;
    long st_ctime;
};

#define S_IFDIR  0040000
#define S_IRWXU  00700

/* Function declarations (implementations not needed - unused when DISABLE_CONFIGFILES=1) */
int stat(const char *path, struct stat *buf);
int fstat(int fd, struct stat *buf);
int mkdir(const char *path, int mode);

#else
/* On non-ARM platforms, use the real sys/stat.h */
#include_next <sys/stat.h>
#endif

#endif /* _SYS_STAT_H_STUB */
