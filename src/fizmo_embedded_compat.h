/*
 * fizmo_embedded_compat.h
 *
 * Force-included compatibility header for embedded libfizmo builds.
 * This header is included via -include compiler flag to:
 * 1. Provide locale declarations that libfizmo's locale_data.h placeholder doesn't provide
 * 2. Provide weak stub for z_filesys_interface_c (we use hybrid instead)
 */

#ifndef FIZMO_EMBEDDED_COMPAT_H
#define FIZMO_EMBEDDED_COMPAT_H

#include "tools/i18n.h"

/* Declare the locale module for libfizmo */
extern locale_module locale_module_libfizmo;

/* Initialize libfizmo locales */
void init_libfizmo_locales(void);

/* Provide weak stub for z_filesys_interface_c since we don't compile filesys_c.c */
#ifdef __ARM_EABI__
#include "filesys_interface/filesys_interface.h"
struct z_filesys_interface z_filesys_interface_c __attribute__((weak)) = {0};
#endif

#endif /* FIZMO_EMBEDDED_COMPAT_H */
