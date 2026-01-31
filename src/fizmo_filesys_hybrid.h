/*
 * fizmo_filesys_hybrid.h
 *
 * Hybrid filesystem interface for libfizmo:
 *   - Story file: read from flash (embedded via story_data.S)
 *   - Save files: read/write to SD card via FatFS
 *
 * This approach is recommended for NXP RT1050 EVK because:
 *   - No need to have story file on SD card
 *   - Save games persist across power cycles
 *   - Simpler than writing to flash while executing (XIP complications)
 */

#ifndef FIZMO_FILESYS_HYBRID_H
#define FIZMO_FILESYS_HYBRID_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialize the hybrid filesystem interface.
 * Must be called before fizmo_bridge_init() or fizmo_start().
 *
 * story_data: pointer to embedded story file in flash
 * story_size: size of story file in bytes
 * save_path:  path prefix for save files on SD card (e.g., "/saves/")
 *             If NULL, saves go to root directory.
 *
 * Returns: 0 on success, -1 on failure
 */
int fizmo_filesys_hybrid_init(const uint8_t *story_data, size_t story_size,
                               const char *save_path);

/*
 * Mount/unmount the SD card filesystem.
 * The filesystem must be mounted before any save/restore operations.
 */
int fizmo_filesys_mount_sd(void);
int fizmo_filesys_unmount_sd(void);

/*
 * Check if SD card is available for saves.
 */
int fizmo_filesys_sd_available(void);

#ifdef __cplusplus
}
#endif

#endif /* FIZMO_FILESYS_HYBRID_H */
