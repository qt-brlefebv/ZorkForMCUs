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

/*
 * Locale message strings (en_US)
 * These match libfizmo_locales.h string codes 0-83
 */
static z_ucs msg_00[] = { 'C','o','u','l','d',' ','n','o','t',' ','o','p','e','n',' ','t','r','a','c','e',' ','f','i','l','e','.',0 };
static z_ucs msg_01[] = { 'I','n','v','a','l','i','d',' ','p','a','r','a','m','e','t','e','r',' ','t','y','p','e','.',0 };
static z_ucs msg_02[] = { 'F','u','n','c','t','i','o','n',' ','c','a','l','l',' ','a','b','o','r','t','e','d','.',0 };
static z_ucs msg_03[] = { 'I','n','v','a','l','i','d',' ','b','a','c','k','s','l','a','s','h',' ','s','e','q','u','e','n','c','e','.',0 };
static z_ucs msg_04[] = { 'W','a','r','n','i','n','g','.',0 };
static z_ucs msg_05[] = { 'F','u','n','c','t','i','o','n',' ','e','r','r','o','r','.',0 };
static z_ucs msg_06[] = { 'E','r','r','o','r',' ','r','e','a','d','i','n','g',' ','f','i','l','e','.',0 };
static z_ucs msg_07[] = { 'C','o','u','l','d',' ','n','o','t',' ','o','p','e','n',' ','f','i','l','e','.',0 };
static z_ucs msg_08[] = { 'E','r','r','o','r',' ','r','e','a','d','i','n','g',' ','s','t','o','r','y','.',0 };
static z_ucs msg_09[] = { 'U','n','k','n','o','w','n',' ','s','t','o','r','y',' ','v','e','r','s','i','o','n','.',0 };
static z_ucs msg_10[] = { 'E','r','r','o','r',' ','c','l','o','s','i','n','g',' ','f','i','l','e','.',0 };
static z_ucs msg_11[] = { 'O','u','t',' ','o','f',' ','m','e','m','o','r','y','.',0 };
static z_ucs msg_12[] = { 'O','u','t',' ','o','f',' ','m','e','m','o','r','y','.',0 };
static z_ucs msg_13[] = { 'F','a','t','a','l',' ','e','r','r','o','r',' ','r','e','a','d','i','n','g',' ','s','t','o','r','y','.',0 };
static z_ucs msg_14[] = { 'N','o','t',' ','y','e','t',' ','i','m','p','l','e','m','e','n','t','e','d','.',0 };
static z_ucs msg_15[] = { 'I','n','s','t','r','u','c','t','i','o','n',' ','n','o','t',' ','i','n','i','t','i','a','l','i','z','e','d','.',0 };
static z_ucs msg_16[] = { 'U','n','k','n','o','w','n',' ','o','p','e','r','a','n','d',' ','t','y','p','e','.',0 };
static z_ucs msg_17[] = { 'O','p','c','o','d','e',' ','n','o','t',' ','i','m','p','l','e','m','e','n','t','e','d','.',0 };
static z_ucs msg_18[] = { 'C','a','n','n','o','t',' ','p','u','l','l',' ','f','r','o','m',' ','e','m','p','t','y',' ','s','t','a','c','k','.',0 };
static z_ucs msg_19[] = { 'C','a','n','n','o','t',' ','d','r','o','p',' ','f','r','o','m',' ','s','t','a','c','k','.',0 };
static z_ucs msg_20[] = { 'P','l','e','a','s','e',' ','e','n','t','e','r',' ','c','o','m','m','a','n','d',' ','f','i','l','e','n','a','m','e',':',0 };
static z_ucs msg_21[] = { 'F','i','l','e','n','a','m','e',' ','m','u','s','t',' ','n','o','t',' ','b','e',' ','e','m','p','t','y','.',0 };
static z_ucs msg_22[] = { 'P','l','e','a','s','e',' ','e','n','t','e','r',' ','t','r','a','n','s','c','r','i','p','t',' ','f','i','l','e','n','a','m','e',':',0 };
static z_ucs msg_23[] = { 'I','n','v','a','l','i','d',' ','o','u','t','p','u','t',' ','s','t','r','e','a','m','.',0 };
static z_ucs msg_24[] = { 'M','a','x','i','m','u','m',' ','s','t','r','e','a','m',' ','d','e','p','t','h',' ','e','x','c','e','e','d','e','d','.',0 };
static z_ucs msg_25[] = { 'T','h','i','s',' ','f','u','n','c','t','i','o','n',' ','i','s',' ','d','i','s','a','b','l','e','d','.',0 };
static z_ucs msg_26[] = { 'T','o','o',' ','m','a','n','y',' ','l','o','c','a','l','s','.',0 };
static z_ucs msg_27[] = { 'I','n','v','a','l','i','d',' ','t','h','r','o','w',' ','d','e','s','t','i','n','a','t','i','o','n','.',0 };
static z_ucs msg_28[] = { 'S','t','a','c','k',' ','o','v','e','r','f','l','o','w','.',0 };
static z_ucs msg_29[] = { 'I','n','v','a','l','i','d',' ','v','a','r','i','a','b','l','e',' ','s','t','o','r','e','.',0 };
static z_ucs msg_30[] = { 'S','t','a','c','k',' ','u','n','d','e','r','f','l','o','w','.',0 };
static z_ucs msg_31[] = { 'N','u','l','l',' ','p','o','i','n','t','e','r','.',0 };
static z_ucs msg_32[] = { 'I','n','v','a','l','i','d',' ','p','r','o','p','e','r','t','y','.',0 };
static z_ucs msg_33[] = { 'I','n','v','a','l','i','d',' ','p','r','o','p','e','r','t','y',' ','n','u','m','b','e','r','.',0 };
static z_ucs msg_34[] = { 'I','n','v','a','l','i','d',' ','o','b','j','e','c','t','.',0 };
static z_ucs msg_35[] = { 'I','n','v','a','l','i','d',' ','o','b','j','e','c','t',' ','n','u','m','b','e','r','.',0 };
static z_ucs msg_36[] = { 'N','o',' ','s','u','c','h',' ','p','r','o','p','e','r','t','y','.',0 };
static z_ucs msg_37[] = { 'P','r','o','p','e','r','t','y',' ','t','o','o',' ','l','o','n','g','.',0 };
static z_ucs msg_38[] = { 'I','n','v','a','l','i','d',' ','a','t','t','r','i','b','u','t','e','.',0 };
static z_ucs msg_39[] = { 'I','n','v','a','l','i','d',' ','n','o','d','e',' ','t','y','p','e','.',0 };
static z_ucs msg_40[] = { 'U','n','k','n','o','w','n',' ','c','h','a','r',' ','c','o','d','e','.',0 };
static z_ucs msg_41[] = { 'M','a','x','i','m','u','m',' ','a','b','b','r','e','v','i','a','t','i','o','n',' ','d','e','p','t','h','.',0 };
static z_ucs msg_42[] = { 'U','n','k','n','o','w','n',' ','e','r','r','o','r','.',0 };
static z_ucs msg_43[] = { 'I','n','v','a','l','i','d',' ','Z','S','C','I','I',' ','i','n','p','u','t',' ','c','o','d','e','.',0 };
static z_ucs msg_44[] = { 'I','n','v','a','l','i','d',' ','Z','S','C','I','I',' ','o','u','t','p','u','t',' ','c','o','d','e','.',0 };
static z_ucs msg_45[] = { 'V','a','l','i','d',' ','c','o','m','m','a','n','d','s',' ','a','r','e',':',0 };
static z_ucs msg_46[] = { 'R','a','n','d','o','m',' ','m','o','d','e',':',' ','p','r','e','d','i','c','t','a','b','l','e','.',0 };
static z_ucs msg_47[] = { 'R','a','n','d','o','m',' ','m','o','d','e',':',' ','r','a','n','d','o','m','.',0 };
static z_ucs msg_48[] = { 'l','i','b','f','i','z','m','o',' ','v','e','r','s','i','o','n','.',0 };
static z_ucs msg_49[] = { 'S','t','o','r','y',' ','r','e','l','e','a','s','e',' ','n','u','m','b','e','r','.',0 };
static z_ucs msg_50[] = { 'S','t','o','r','y',' ','s','e','r','i','a','l',' ','n','u','m','b','e','r','.',0 };
static z_ucs msg_51[] = { 'Z','-','S','t','a','c','k',' ','s','i','z','e','.',0 };
static z_ucs msg_52[] = { 'Z','-','S','t','a','c','k',' ','e','n','t','r','i','e','s',' ','i','n',' ','u','s','e','.',0 };
static z_ucs msg_53[] = { 'R','o','u','t','i','n','e',' ','s','t','a','c','k',' ','c','h','e','c','k',' ','d','i','s','a','b','l','e','d','.',0 };
static z_ucs msg_54[] = { 'B','y','t','e','s',' ','f','o','r',' ','u','n','d','o','.',0 };
static z_ucs msg_55[] = { 'B','y','t','e','s',' ','f','o','r',' ','h','i','s','t','o','r','y','.',0 };
static z_ucs msg_56[] = { 'B','y','t','e','s',' ','f','o','r',' ','b','l','o','c','k',' ','b','u','f','f','e','r','.',0 };
static z_ucs msg_57[] = { 'P','r','e','l','o','a','d','e','d',' ','i','n','p','u','t',' ','n','o','t',' ','a','v','a','i','l','a','b','l','e','.',0 };
static z_ucs msg_58[] = { 'T','i','m','e','d',' ','i','n','p','u','t',' ','n','o','t',' ','i','m','p','l','e','m','e','n','t','e','d','.',0 };
static z_ucs msg_59[] = { 'P','l','e','a','s','e',' ','e','n','t','e','r',' ','s','a','v','e','g','a','m','e',' ','f','i','l','e','n','a','m','e',':',0 };
static z_ucs msg_60[] = { 'E','r','r','o','r',' ','w','r','i','t','i','n','g',' ','s','a','v','e',' ','f','i','l','e','.',0 };
static z_ucs msg_61[] = { 'C','a','n','n','o','t',' ','f','i','n','d',' ','I','F','h','d',' ','c','h','u','n','k','.',0 };
static z_ucs msg_62[] = { 'E','r','r','o','r',' ','r','e','a','d','i','n','g',' ','s','a','v','e',' ','f','i','l','e','.',0 };
static z_ucs msg_63[] = { 'C','o','u','l','d',' ','n','o','t',' ','r','e','a','d',' ','r','e','l','e','a','s','e',' ','n','u','m','b','e','r','.',0 };
static z_ucs msg_64[] = { 'C','o','u','l','d',' ','n','o','t',' ','r','e','a','d',' ','s','e','r','i','a','l',' ','n','u','m','b','e','r','.',0 };
static z_ucs msg_65[] = { 'C','o','u','l','d',' ','n','o','t',' ','r','e','a','d',' ','c','h','e','c','k','s','u','m','.',0 };
static z_ucs msg_66[] = { 'S','a','v','e',' ','f','i','l','e',' ','d','o','e','s',' ','n','o','t',' ','m','a','t','c','h',' ','s','t','o','r','y','.',0 };
static z_ucs msg_67[] = { 'C','o','u','l','d',' ','n','o','t',' ','r','e','a','d',' ','r','e','s','t','o','r','e',' ','P','C','.',0 };
static z_ucs msg_68[] = { 'C','a','n','n','o','t',' ','r','e','a','d',' ','c','h','u','n','k',' ','l','e','n','g','t','h','.',0 };
static z_ucs msg_69[] = { 'C','o','u','l','d',' ','n','o','t',' ','f','i','n','d',' ','o','r','i','g','i','n','a','l',' ','s','t','o','r','y',' ','f','i','l','e','.',0 };
static z_ucs msg_70[] = { 'C','a','n','n','o','t',' ','f','i','n','d',' ','m','e','m','o','r','y',' ','c','h','u','n','k','.',0 };
static z_ucs msg_71[] = { 'C','a','n','n','o','t',' ','f','i','n','d',' ','s','t','a','c','k',' ','c','h','u','n','k','.',0 };
static z_ucs msg_72[] = { 'I','n','v','a','l','i','d',' ','I','F','F',' ','a','c','c','e','s','s',' ','m','o','d','e','.',0 };
static z_ucs msg_73[] = { 'C','a','u','g','h','t',' ','s','i','g','n','a','l',',',' ','a','b','o','r','t','i','n','g','.',0 };
static z_ucs msg_74[] = { 'C','a','n','n','o','t',' ','d','i','v','i','d','e',' ','b','y',' ','z','e','r','o','.',0 };
static z_ucs msg_75[] = { 'J','E',' ','o','p','c','o','d','e',' ','n','e','e','d','s',' ','m','o','r','e',' ','o','p','e','r','a','n','d','s','.',0 };
static z_ucs msg_76[] = { 't','i','m','e','(',')',' ','f','a','i','l','e','d','.',0 };
static z_ucs msg_77[] = { 'U','n','k','n','o','w','n',' ','c','o','n','f','i','g',' ','o','p','t','i','o','n','.',0 };
static z_ucs msg_78[] = { 'I','n','v','a','l','i','d',' ','c','o','n','f','i','g',' ','v','a','l','u','e','.',0 };
static z_ucs msg_79[] = { 'Z','-','V','e','r','s','i','o','n',':',0 };
static z_ucs msg_80[] = { 'H','i','s','t','o','r','y',' ','o','b','j','e','c','t',' ','i','n','v','a','l','i','d','.',0 };
static z_ucs msg_81[] = { 'f','i','z','m','o',' ','v','e','r','s','i','o','n','.',0 };
static z_ucs msg_82[] = { 'B','l','o','r','b',' ','h','a','s',' ','n','o',' ','Z','C','O','D',' ','c','h','u','n','k','.',0 };
static z_ucs msg_83[] = { 'N','o','t',' ','a',' ','v','a','l','i','d',' ','Z','-','M','a','c','h','i','n','e',' ','f','i','l','e','.',0 };

/* Array of message pointers */
static z_ucs *locale_message_ptrs[] = {
    msg_00, msg_01, msg_02, msg_03, msg_04, msg_05, msg_06, msg_07,
    msg_08, msg_09, msg_10, msg_11, msg_12, msg_13, msg_14, msg_15,
    msg_16, msg_17, msg_18, msg_19, msg_20, msg_21, msg_22, msg_23,
    msg_24, msg_25, msg_26, msg_27, msg_28, msg_29, msg_30, msg_31,
    msg_32, msg_33, msg_34, msg_35, msg_36, msg_37, msg_38, msg_39,
    msg_40, msg_41, msg_42, msg_43, msg_44, msg_45, msg_46, msg_47,
    msg_48, msg_49, msg_50, msg_51, msg_52, msg_53, msg_54, msg_55,
    msg_56, msg_57, msg_58, msg_59, msg_60, msg_61, msg_62, msg_63,
    msg_64, msg_65, msg_66, msg_67, msg_68, msg_69, msg_70, msg_71,
    msg_72, msg_73, msg_74, msg_75, msg_76, msg_77, msg_78, msg_79,
    msg_80, msg_81, msg_82, msg_83
};

/* Locale messages structure with actual messages */
static locale_messages en_us_locale_messages = {
    84,                      /* nof_messages */
    locale_message_ptrs      /* messages array */
};

/* Initialize libfizmo locales */
void init_libfizmo_locales(void) {
    /* Create stringmap and add the en_US locale with real messages */
    locale_module_libfizmo.messages_by_localcode = create_stringmap();

    /* Add "en_US" locale with actual messages */
    add_stringmap_element(locale_module_libfizmo.messages_by_localcode,
                          default_locale_name, &en_us_locale_messages);

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
