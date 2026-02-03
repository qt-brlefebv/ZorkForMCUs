/*
 * sd_init.c
 * SD card initialization for Zork save/restore
 */

#include "sd_init.h"
#include "sd_diskio.h"
#include "ff.h"
#include "ff_gen_drv.h"

#include <stdio.h>

/* FatFS objects */
static FATFS s_fatfs;
static char s_drive_path[4];
static int s_initialized = 0;

int sd_filesystem_init(void)
{
    int ret;

    if (s_initialized) {
        return 0;  /* Already initialized */
    }

    /* Initialize SD card hardware */
    ret = SD_Card_Init();
    if (ret != 0) {
        printf("SD card init failed: %d\r\n", ret);
        return -1;
    }

    /* Check card is inserted */
    if (!SD_Card_IsInserted()) {
        printf("SD card not detected\r\n");
        return -2;
    }

    /* Register driver with FatFS */
    if (FATFS_LinkDriver(&SD_Driver, s_drive_path) != 0) {
        printf("FATFS_LinkDriver failed\r\n");
        return -3;
    }

    /* Mount filesystem */
    FRESULT fres = f_mount(&s_fatfs, s_drive_path, 1);
    if (fres != FR_OK) {
        printf("f_mount failed: %d\r\n", fres);
        FATFS_UnLinkDriver(s_drive_path);
        return -4;
    }

    printf("SD card mounted at %s\r\n", s_drive_path);
    s_initialized = 1;

    return 0;
}

int sd_filesystem_available(void)
{
    return s_initialized;
}

void sd_filesystem_deinit(void)
{
    if (!s_initialized) {
        return;
    }

    f_mount(NULL, s_drive_path, 0);
    FATFS_UnLinkDriver(s_drive_path);
    s_initialized = 0;
}
