/*
 * fizmo_locale_stubs.c
 *
 * Minimal stub implementations for libfizmo locale, hyphenation, and blorb support.
 * These allow fizmo to compile without the full locale generation system and
 * optional features we don't need for the MVP.
 */

#include "tools/i18n.h"
#include "tools/types.h"
#include "tools/stringmap.h"
#include "blorb_interface/blorb_interface.h"

#include <stddef.h>
#include <stdio.h>

/* Module name for libfizmo */
z_ucs libfizmo_module_name[] = { 'l', 'i', 'b', 'f', 'i', 'z', 'm', 'o', 0 };

/* Default locale name */
z_ucs default_locale_name[] = { 'e', 'n', '_', 'U', 'S', 0 };

/* The locale module struct */
locale_module locale_module_libfizmo;

/* Dummy locale messages - empty but non-null */
static locale_messages dummy_locale_messages = { 0, NULL };

/* Initialize libfizmo locales - minimal stub */
void init_libfizmo_locales(void) {
    /* Create stringmap and add at least one entry to avoid NULL return from get_names_in_stringmap */
    locale_module_libfizmo.messages_by_localcode = create_stringmap();

    /* Add dummy "en_US" locale to prevent crash when iterating empty stringmap */
    add_stringmap_element(locale_module_libfizmo.messages_by_localcode,
                          default_locale_name, &dummy_locale_messages);

    locale_module_libfizmo.module_name = libfizmo_module_name;

    register_locale_module(
        locale_module_libfizmo.module_name,
        &locale_module_libfizmo);
}

/*
 * Hyphenation stubs - we don't need hyphenation for the MVP
 */
void init_patterns(void) {
    /* No hyphenation patterns */
}

void free_hyphenation_memory(void) {
    /* Nothing to free */
}

z_ucs *hyphenate(z_ucs *word) {
    (void)word;
    return NULL; /* No hyphenation */
}

/*
 * Blorb interface stub - we don't use blorb resources
 */
struct z_blorb_interface *active_blorb_interface = NULL;
