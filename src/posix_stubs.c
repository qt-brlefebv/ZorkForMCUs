/*
 * posix_stubs.c
 *
 * Stub implementations of POSIX functions for embedded ARM builds.
 * These allow libfizmo code to compile but will fail at runtime if called.
 */

#ifdef __ARM_EABI__

#include <sys/types.h>
#include <pwd.h>
#include <signal.h>

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <FreeRTOS.h>

static char newlib_heap[4096];  // Small region for stdio buffers, etc.
static char *newlib_heap_end = newlib_heap;

void* _sbrk(ptrdiff_t incr) {
    char *prev_end = newlib_heap_end;
    if (newlib_heap_end + incr > newlib_heap + sizeof(newlib_heap)) {
        // Out of newlib heap space
        return (void*)-1;
    }
    newlib_heap_end += incr;
    return prev_end;
}

  /*
   * Heap allocation wrappers - redirect all malloc/free to FreeRTOS heap.
   * Using --wrap linker option, so these are named __wrap_*.
   * This catches ALL allocations including those inside newlib (strdup, etc).
   *
   * __attribute__((used)) prevents LTO from discarding these symbols.
   */

  __attribute__((used))
  void* __wrap_malloc(size_t size) {
      return pvPortMalloc(size);
  }

  __attribute__((used))
  void __wrap_free(void* ptr) {
      vPortFree(ptr);
  }

  __attribute__((used))
  void* __wrap_calloc(size_t num, size_t size) {
      size_t total = num * size;
      void* ptr = pvPortMalloc(total);
      if (ptr) {
          memset(ptr, 0, total);
      }
      return ptr;
  }

  __attribute__((used))
  void* __wrap_realloc(void* ptr, size_t size) {
      // Simple implementation - allocate new, copy, free old
      if (size == 0) {
          vPortFree(ptr);
          return NULL;
      }
      if (ptr == NULL) {
          return pvPortMalloc(size);
      }
      void* new_ptr = pvPortMalloc(size);
      if (new_ptr) {
          memcpy(new_ptr, ptr, size);  // Note: may copy too much, but safe
          vPortFree(ptr);
      }
      return new_ptr;
  }

  /*
   * strdup/strndup wrappers - newlib's versions call malloc internally
   * which bypasses --wrap, so we need explicit implementations.
   */

  __attribute__((used))
  char* __wrap_strdup(const char* s) {
      if (s == NULL) return NULL;
      size_t len = strlen(s) + 1;
      char* dup = pvPortMalloc(len);
      if (dup) {
          memcpy(dup, s, len);
      }
      return dup;
  }

  __attribute__((used))
  char* __wrap_strndup(const char* s, size_t n) {
      if (s == NULL) return NULL;
      size_t len = strnlen(s, n);
      char* dup = pvPortMalloc(len + 1);
      if (dup) {
          memcpy(dup, s, len);
          dup[len] = '\0';
      }
      return dup;
  }

char *getenv(const char *name) {
    (void)name;
    return NULL;
}

/* Stub: get user ID - always return 0 (root) */
uid_t getuid(void)
{
    return 0;
}

/* Stub: get password entry - return valid pointer with NULL pw_dir
 * libfizmo dereferences the return value, so we can't return NULL */
struct passwd *getpwuid(uid_t uid)
{
    static struct passwd pw_stub = {
        .pw_name = "embedded",
        .pw_passwd = "",
        .pw_uid = 0,
        .pw_gid = 0,
        .pw_gecos = "",
        .pw_dir = NULL,     /* NULL home directory - this is what libfizmo checks */
        .pw_shell = ""
    };
    (void)uid;
    return &pw_stub;
}

/* Stub: set signal action - always succeed (do nothing) */
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
    (void)signum;
    (void)act;
    (void)oldact;
    return 0;
}

#include <reent.h>

__attribute__((used))
void* __wrap__malloc_r(struct _reent *r, size_t size) {
    (void)r;
    return pvPortMalloc(size);
}

__attribute__((used))
void __wrap__free_r(struct _reent *r, void *ptr) {
    (void)r;
    vPortFree(ptr);
}

__attribute__((used))
void* __wrap__calloc_r(struct _reent *r, size_t num, size_t size) {
    (void)r;
    size_t total = num * size;
    void* ptr = pvPortMalloc(total);
    if (ptr) memset(ptr, 0, total);
    return ptr;
}

__attribute__((used))
void* __wrap__realloc_r(struct _reent *r, void *ptr, size_t size) {
    (void)r;
    if (size == 0) { vPortFree(ptr); return NULL; }
    if (ptr == NULL) return pvPortMalloc(size);
    void* new_ptr = pvPortMalloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, size);
        vPortFree(ptr);
    }
    return new_ptr;
}

#endif /* __ARM_EABI__ */
