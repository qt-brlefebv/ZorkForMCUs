/*
 * sd_diskio.h
 * SD card disk I/O driver for FatFS on RT1050
 */

#ifndef SD_DISKIO_H
#define SD_DISKIO_H

#include "ff_gen_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SD card driver structure for FatFS */
extern const Diskio_drvTypeDef SD_Driver;

/* Initialize SD card hardware (call before FATFS_LinkDriver) */
int SD_Card_Init(void);

/* Check if card is inserted */
int SD_Card_IsInserted(void);

#ifdef __cplusplus
}
#endif

#endif /* SD_DISKIO_H */
