/*
 * locale_data.h
 *
 * Minimal stub header for libfizmo locale support.
 * This replaces the generated locale_data.h from libfizmo's build system.
 */

#ifndef locale_data_h_INCLUDED
#define locale_data_h_INCLUDED

#include "../tools/i18n.h"

/* Declare the locale module for libfizmo */
extern locale_module locale_module_libfizmo;

/* Initialize libfizmo locales */
void init_libfizmo_locales(void);

#endif /* locale_data_h_INCLUDED */
