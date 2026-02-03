/*
 * fizmo_filesys_hybrid.c
 *
 * Hybrid filesystem interface for libfizmo.
 * See fizmo_filesys_hybrid.h for details.
 */

#include "fizmo_filesys_hybrid.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

/* FatFS includes - adjust path as needed for your SDK */
#include "ff.h"

/* libfizmo includes */
#include "tools/filesys.h"
#include "tools/types.h"
#include "filesys_interface/filesys_interface.h"

/* Embedded story data */
static const uint8_t *s_story_data = NULL;
static size_t s_story_size = 0;

/* Save file path prefix */
static char s_save_path[64] = "";

/* FatFS filesystem object */
static FATFS s_fatfs;
static int s_sd_mounted = 0;

/*
 * File handle structure for tracking open files.
 * For embedded story: uses memory pointer
 * For SD card files: uses FatFS FIL
 */
typedef struct {
    int is_embedded;       /* 1 if this is the embedded story file */
    union {
        struct {
            const uint8_t *data;
            size_t size;
            size_t pos;
        } mem;
        FIL fil;
    } u;
} hybrid_file_t;

/* Forward declarations for filesys interface */
static z_file* hybrid_openfile(char *filename, int filetype, int fileaccess);
static int hybrid_closefile(z_file *file_to_close);
static int hybrid_readchar(z_file *fileref);
static size_t hybrid_readchars(void *ptr, size_t len, z_file *fileref);
static int hybrid_writechar(int ch, z_file *fileref);
static size_t hybrid_writechars(void *ptr, size_t len, z_file *fileref);
static int hybrid_writestring(char *s, z_file *fileref);
static int hybrid_writeucsstring(z_ucs *s, z_file *fileref);
static int hybrid_fileprintf(z_file *fileref, char *format, ...);
static int hybrid_vfileprintf(z_file *fileref, char *format, va_list ap);
static int hybrid_filescanf(z_file *fileref, char *format, ...);
static int hybrid_vfilescanf(z_file *fileref, char *format, va_list ap);
static long hybrid_getfilepos(z_file *fileref);
static int hybrid_setfilepos(z_file *fileref, long seek, int whence);
static int hybrid_unreadchar(int c, z_file *fileref);
static int hybrid_flushfile(z_file *fileref);
static time_t hybrid_get_last_file_mod_timestamp(z_file *fileref);
static int hybrid_get_fileno(z_file *fileref);
static FILE* hybrid_get_stdio_stream(z_file *fileref);
static char* hybrid_get_cwd(void);
static int hybrid_ch_dir(char *dirname);
static z_dir* hybrid_open_dir(char *dirname);
static int hybrid_close_dir(z_dir *dirref);
static int hybrid_read_dir(struct z_dir_ent *dir_ent, z_dir *dirref);
static int hybrid_make_dir(char *path);
static bool hybrid_is_filename_directory(char *filename);

/* Filesystem interface structure */
static struct z_filesys_interface hybrid_filesys_interface = {
    .openfile = hybrid_openfile,
    .closefile = hybrid_closefile,
    .readchar = hybrid_readchar,
    .readchars = hybrid_readchars,
    .writechar = hybrid_writechar,
    .writechars = hybrid_writechars,
    .writestring = hybrid_writestring,
    .writeucsstring = hybrid_writeucsstring,
    .fileprintf = hybrid_fileprintf,
    .vfileprintf = hybrid_vfileprintf,
    .filescanf = hybrid_filescanf,
    .vfilescanf = hybrid_vfilescanf,
    .getfilepos = hybrid_getfilepos,
    .setfilepos = hybrid_setfilepos,
    .unreadchar = hybrid_unreadchar,
    .flushfile = hybrid_flushfile,
    .get_last_file_mod_timestamp = hybrid_get_last_file_mod_timestamp,
    .get_fileno = hybrid_get_fileno,
    .get_stdio_stream = hybrid_get_stdio_stream,
    .get_cwd = hybrid_get_cwd,
    .ch_dir = hybrid_ch_dir,
    .open_dir = hybrid_open_dir,
    .close_dir = hybrid_close_dir,
    .read_dir = hybrid_read_dir,
    .make_dir = hybrid_make_dir,
    .is_filename_directory = hybrid_is_filename_directory
};

/*
 * Public API implementation
 */

int fizmo_filesys_hybrid_init(const uint8_t *story_data, size_t story_size,
                               const char *save_path)
{
    if (story_data == NULL || story_size == 0) {
        return -1;
    }

    s_story_data = story_data;
    s_story_size = story_size;

    if (save_path != NULL) {
        strncpy(s_save_path, save_path, sizeof(s_save_path) - 1);
        s_save_path[sizeof(s_save_path) - 1] = '\0';
    } else {
        s_save_path[0] = '\0';
    }

    /* Register our filesystem interface with libfizmo */
    fizmo_register_filesys_interface(&hybrid_filesys_interface);

    return 0;
}

int fizmo_filesys_mount_sd(void)
{
    if (s_sd_mounted) {
        return 0;  /* Already mounted */
    }

    FRESULT res = f_mount(&s_fatfs, "", 1);  /* Mount with immediate mount */
    if (res != FR_OK) {
        return -1;
    }

    /* Ensure saves directory exists (f_mkdir fails silently if it already exists) */
    f_mkdir("/saves");

    s_sd_mounted = 1;
    return 0;
}

int fizmo_filesys_unmount_sd(void)
{
    if (!s_sd_mounted) {
        return 0;
    }

    f_mount(NULL, "", 0);  /* Unmount */
    s_sd_mounted = 0;
    return 0;
}

int fizmo_filesys_sd_available(void)
{
    return s_sd_mounted;
}

/*
 * Helper: allocate and initialize a z_file structure
 */
static z_file* alloc_zfile(const char *filename, int filetype, int fileaccess)
{
    z_file *zf = pvPortMalloc(sizeof(z_file));
    if (zf == NULL) {
        return NULL;
    }

    memset(zf, 0, sizeof(z_file));

    if (filename != NULL) {
        zf->filename = pvPortMalloc(strlen(filename) + 1);
        if (zf->filename != NULL) {
            strcpy(zf->filename, filename);
        }
    }

    zf->filetype = filetype;
    zf->fileaccess = fileaccess;

    return zf;
}

/*
 * Helper: free a z_file structure
 */
static void free_zfile(z_file *zf)
{
    if (zf == NULL) {
        return;
    }

    if (zf->file_object != NULL) {
        vPortFree(zf->file_object);
    }

    if (zf->filename != NULL) {
        vPortFree(zf->filename);
    }

    vPortFree(zf);
}

/*
 * Helper: build full path for save file
 */
static void build_save_path(char *dest, size_t dest_size, const char *filename)
{
    if (s_save_path[0] != '\0') {
        snprintf(dest, dest_size, "%s%s", s_save_path, filename);
    } else {
        strncpy(dest, filename, dest_size - 1);
        dest[dest_size - 1] = '\0';
    }
}

/*
 * Filesystem interface implementation
 */

static z_file* hybrid_openfile(char *filename, int filetype, int fileaccess)
{
    if (filename == NULL) {
        return NULL;
    }

    /* Check for special embedded story filename */
    if (strcmp(filename, "@embedded") == 0) {
        if (s_story_data == NULL) {
            return NULL;
        }

        z_file *zf = alloc_zfile(filename, filetype, fileaccess);
        if (zf == NULL) {
            return NULL;
        }

        hybrid_file_t *hf = pvPortMalloc(sizeof(hybrid_file_t));
        if (hf == NULL) {
            free_zfile(zf);
            return NULL;
        }

        hf->is_embedded = 1;
        hf->u.mem.data = s_story_data;
        hf->u.mem.size = s_story_size;
        hf->u.mem.pos = 0;

        zf->file_object = hf;
        return zf;
    }

    /* Regular file on SD card */
    if (!s_sd_mounted) {
        return NULL;
    }

    char full_path[128];
    build_save_path(full_path, sizeof(full_path), filename);

    BYTE mode;
    switch (fileaccess) {
        case FILEACCESS_READ:
            mode = FA_READ | FA_OPEN_EXISTING;
            break;
        case FILEACCESS_WRITE:
            mode = FA_WRITE | FA_CREATE_ALWAYS;
            break;
        case FILEACCESS_APPEND:
            mode = FA_WRITE | FA_OPEN_APPEND;
            break;
        default:
            return NULL;
    }

    z_file *zf = alloc_zfile(filename, filetype, fileaccess);
    if (zf == NULL) {
        return NULL;
    }

    hybrid_file_t *hf = pvPortMalloc(sizeof(hybrid_file_t));
    if (hf == NULL) {
        free_zfile(zf);
        return NULL;
    }

    hf->is_embedded = 0;

    FRESULT res = f_open(&hf->u.fil, full_path, mode);
    if (res != FR_OK) {
        vPortFree(hf);
        free_zfile(zf);
        return NULL;
    }

    zf->file_object = hf;
    return zf;
}

static int hybrid_closefile(z_file *file_to_close)
{
    if (file_to_close == NULL || file_to_close->file_object == NULL) {
        return -1;
    }

    hybrid_file_t *hf = (hybrid_file_t *)file_to_close->file_object;

    if (!hf->is_embedded) {
        f_close(&hf->u.fil);
    }

    free_zfile(file_to_close);
    return 0;
}

static int hybrid_readchar(z_file *fileref)
{
    if (fileref == NULL || fileref->file_object == NULL) {
        return -1;
    }

    hybrid_file_t *hf = (hybrid_file_t *)fileref->file_object;

    if (hf->is_embedded) {
        if (hf->u.mem.pos >= hf->u.mem.size) {
            return -1;  /* EOF */
        }
        return hf->u.mem.data[hf->u.mem.pos++];
    } else {
        UINT br;
        uint8_t ch;
        FRESULT res = f_read(&hf->u.fil, &ch, 1, &br);
        if (res != FR_OK || br == 0) {
            return -1;
        }
        return ch;
    }
}

static size_t hybrid_readchars(void *ptr, size_t len, z_file *fileref)
{
    if (fileref == NULL || fileref->file_object == NULL || ptr == NULL) {
        return 0;
    }

    hybrid_file_t *hf = (hybrid_file_t *)fileref->file_object;

    if (hf->is_embedded) {
        size_t available = hf->u.mem.size - hf->u.mem.pos;
        size_t to_read = (len < available) ? len : available;
        memcpy(ptr, hf->u.mem.data + hf->u.mem.pos, to_read);
        hf->u.mem.pos += to_read;
        return to_read;
    } else {
        UINT br;
        FRESULT res = f_read(&hf->u.fil, ptr, len, &br);
        if (res != FR_OK) {
            return 0;
        }
        return br;
    }
}

static int hybrid_writechar(int ch, z_file *fileref)
{
    if (fileref == NULL || fileref->file_object == NULL) {
        return -1;
    }

    hybrid_file_t *hf = (hybrid_file_t *)fileref->file_object;

    if (hf->is_embedded) {
        return -1;  /* Can't write to embedded story */
    }

    UINT bw;
    uint8_t byte = (uint8_t)ch;
    FRESULT res = f_write(&hf->u.fil, &byte, 1, &bw);
    if (res != FR_OK || bw == 0) {
        return -1;
    }
    return ch;
}

static size_t hybrid_writechars(void *ptr, size_t len, z_file *fileref)
{
    if (fileref == NULL || fileref->file_object == NULL || ptr == NULL) {
        return 0;
    }

    hybrid_file_t *hf = (hybrid_file_t *)fileref->file_object;

    if (hf->is_embedded) {
        return 0;  /* Can't write to embedded story */
    }

    UINT bw;
    FRESULT res = f_write(&hf->u.fil, ptr, len, &bw);
    if (res != FR_OK) {
        return 0;
    }
    return bw;
}

static int hybrid_writestring(char *s, z_file *fileref)
{
    if (s == NULL) {
        return 0;
    }
    size_t len = strlen(s);
    return (hybrid_writechars(s, len, fileref) == len) ? 0 : -1;
}

static int hybrid_writeucsstring(z_ucs *s, z_file *fileref)
{
    if (s == NULL || fileref == NULL) {
        return -1;
    }

    /* Convert z_ucs to UTF-8 and write */
    while (*s != 0) {
        z_ucs ch = *s++;
        if (ch < 0x80) {
            if (hybrid_writechar((int)ch, fileref) < 0) return -1;
        } else if (ch < 0x800) {
            if (hybrid_writechar(0xC0 | (ch >> 6), fileref) < 0) return -1;
            if (hybrid_writechar(0x80 | (ch & 0x3F), fileref) < 0) return -1;
        } else if (ch < 0x10000) {
            if (hybrid_writechar(0xE0 | (ch >> 12), fileref) < 0) return -1;
            if (hybrid_writechar(0x80 | ((ch >> 6) & 0x3F), fileref) < 0) return -1;
            if (hybrid_writechar(0x80 | (ch & 0x3F), fileref) < 0) return -1;
        } else {
            if (hybrid_writechar(0xF0 | (ch >> 18), fileref) < 0) return -1;
            if (hybrid_writechar(0x80 | ((ch >> 12) & 0x3F), fileref) < 0) return -1;
            if (hybrid_writechar(0x80 | ((ch >> 6) & 0x3F), fileref) < 0) return -1;
            if (hybrid_writechar(0x80 | (ch & 0x3F), fileref) < 0) return -1;
        }
    }
    return 0;
}

static int hybrid_fileprintf(z_file *fileref, char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = hybrid_vfileprintf(fileref, format, ap);
    va_end(ap);
    return result;
}

static int hybrid_vfileprintf(z_file *fileref, char *format, va_list ap)
{
    if (fileref == NULL || format == NULL) {
        return -1;
    }

    char buffer[256];
    int len = vsnprintf(buffer, sizeof(buffer), format, ap);
    if (len < 0) {
        return -1;
    }

    size_t to_write = (size_t)len;
    if (to_write >= sizeof(buffer)) {
        to_write = sizeof(buffer) - 1;
    }

    return (hybrid_writechars(buffer, to_write, fileref) == to_write) ? len : -1;
}

static int hybrid_filescanf(z_file *fileref, char *format, ...)
{
    /* Not implemented - save games use binary format */
    (void)fileref;
    (void)format;
    return -1;
}

static int hybrid_vfilescanf(z_file *fileref, char *format, va_list ap)
{
    /* Not implemented */
    (void)fileref;
    (void)format;
    (void)ap;
    return -1;
}

static long hybrid_getfilepos(z_file *fileref)
{
    if (fileref == NULL || fileref->file_object == NULL) {
        return -1;
    }

    hybrid_file_t *hf = (hybrid_file_t *)fileref->file_object;

    if (hf->is_embedded) {
        return (long)hf->u.mem.pos;
    } else {
        return (long)f_tell(&hf->u.fil);
    }
}

static int hybrid_setfilepos(z_file *fileref, long seek, int whence)
{
    if (fileref == NULL || fileref->file_object == NULL) {
        return -1;
    }

    hybrid_file_t *hf = (hybrid_file_t *)fileref->file_object;

    if (hf->is_embedded) {
        long new_pos;
        switch (whence) {
            case SEEK_SET:
                new_pos = seek;
                break;
            case SEEK_CUR:
                new_pos = (long)hf->u.mem.pos + seek;
                break;
            case SEEK_END:
                new_pos = (long)hf->u.mem.size + seek;
                break;
            default:
                return -1;
        }

        if (new_pos < 0 || (size_t)new_pos > hf->u.mem.size) {
            return -1;
        }

        hf->u.mem.pos = (size_t)new_pos;
        return 0;
    } else {
        FSIZE_t new_pos;
        switch (whence) {
            case SEEK_SET:
                new_pos = (FSIZE_t)seek;
                break;
            case SEEK_CUR:
                new_pos = f_tell(&hf->u.fil) + seek;
                break;
            case SEEK_END:
                new_pos = f_size(&hf->u.fil) + seek;
                break;
            default:
                return -1;
        }

        FRESULT res = f_lseek(&hf->u.fil, new_pos);
        return (res == FR_OK) ? 0 : -1;
    }
}

static int hybrid_unreadchar(int c, z_file *fileref)
{
    if (fileref == NULL || fileref->file_object == NULL) {
        return -1;
    }

    hybrid_file_t *hf = (hybrid_file_t *)fileref->file_object;

    if (hf->is_embedded) {
        if (hf->u.mem.pos > 0) {
            hf->u.mem.pos--;
            return c;
        }
        return -1;
    } else {
        /* FatFS doesn't have ungetc, so seek back one byte */
        FSIZE_t pos = f_tell(&hf->u.fil);
        if (pos > 0) {
            f_lseek(&hf->u.fil, pos - 1);
            return c;
        }
        return -1;
    }
}

static int hybrid_flushfile(z_file *fileref)
{
    if (fileref == NULL || fileref->file_object == NULL) {
        return -1;
    }

    hybrid_file_t *hf = (hybrid_file_t *)fileref->file_object;

    if (hf->is_embedded) {
        return 0;  /* Nothing to flush */
    } else {
        return (f_sync(&hf->u.fil) == FR_OK) ? 0 : -1;
    }
}

static time_t hybrid_get_last_file_mod_timestamp(z_file *fileref)
{
    (void)fileref;
    return 0;  /* Not implemented */
}

static int hybrid_get_fileno(z_file *fileref)
{
    (void)fileref;
    return -1;  /* Not applicable */
}

static FILE* hybrid_get_stdio_stream(z_file *fileref)
{
    (void)fileref;
    return NULL;  /* Not applicable */
}

/*
static char* hybrid_get_cwd(void)
{
    static char cwd[2] = "/";
    return cwd;
}
*/

  static char* hybrid_get_cwd(void)
  {
      char* cwd = malloc(2);
      if (cwd) {
          cwd[0] = '/';
          cwd[1] = '\0';
      }
      return cwd;
  }

static int hybrid_ch_dir(char *dirname)
{
    (void)dirname;
    return -1;  /* Not implemented */
}

static z_dir* hybrid_open_dir(char *dirname)
{
    (void)dirname;
    return NULL;  /* Not implemented */
}

static int hybrid_close_dir(z_dir *dirref)
{
    (void)dirref;
    return -1;  /* Not implemented */
}

static int hybrid_read_dir(struct z_dir_ent *dir_ent, z_dir *dirref)
{
    (void)dir_ent;
    (void)dirref;
    return -1;  /* Not implemented */
}

static int hybrid_make_dir(char *path)
{
    if (!s_sd_mounted || path == NULL) {
        return -1;
    }

    char full_path[128];
    build_save_path(full_path, sizeof(full_path), path);

    return (f_mkdir(full_path) == FR_OK) ? 0 : -1;
}

static bool hybrid_is_filename_directory(char *filename)
{
    if (!s_sd_mounted || filename == NULL) {
        return false;
    }

    char full_path[128];
    build_save_path(full_path, sizeof(full_path), filename);

    FILINFO fno;
    if (f_stat(full_path, &fno) != FR_OK) {
        return false;
    }

    return (fno.fattrib & AM_DIR) != 0;
}
