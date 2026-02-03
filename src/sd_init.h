/*
 * sd_init.h
 * SD card initialization for Zork save/restore
 */

#ifndef SD_INIT_H
#define SD_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialize SD card and mount filesystem.
 * Returns 0 on success, negative on failure:
 *   -1: SD card init failed
 *   -2: Card not detected
 *   -3: FatFS link driver failed
 *   -4: FatFS mount failed
 */
int sd_filesystem_init(void);

/*
 * Check if SD filesystem is available for save/restore.
 */
int sd_filesystem_available(void);

/*
 * Unmount and deinitialize (optional, for clean shutdown).
 */
void sd_filesystem_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* SD_INIT_H */
