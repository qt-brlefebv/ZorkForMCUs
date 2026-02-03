/*
 * ffconf.h
 *
 * FatFs configuration for ZorkUI RT1050 port.
 * Based on NXP SDK sdcard_fatfs_freertos example.
 *
 * Configuration:
 * - Read/write mode (for save games)
 * - FreeRTOS re-entrant mode
 * - Long filename support (LFN) with stack buffer
 * - Single volume (SD card)
 */

#ifndef _FFCONF_H_
#define _FFCONF_H_

#include "FreeRTOS.h"
#include "semphr.h"

/*---------------------------------------------------------------------------/
/  FatFs Functional Configurations
/---------------------------------------------------------------------------*/

#define FFCONF_DEF	80286	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY	0
/* Read/write mode - we need to save games */

#define FF_FS_MINIMIZE	0
/* Full API - we need mkdir, unlink for save management */

#define FF_USE_FIND		0
/* Not needed for simple save file access */

#define FF_USE_MKFS		0
/* Not needed - SD card will be pre-formatted */

#define FF_USE_FASTSEEK	0
/* Not needed for small save files */

#define FF_USE_EXPAND	0
/* Not needed */

#define FF_USE_CHMOD	0
/* Not needed */

#define FF_USE_LABEL	0
/* Not needed */

#define FF_USE_FORWARD	0
/* Not needed */

#define FF_USE_STRFUNC	0
/* Not using f_printf/f_gets - save bandwidth */

/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define FF_CODE_PAGE	437
/* U.S. (ASCII) - sufficient for save filenames */

#define FF_USE_LFN		2
#define FF_MAX_LFN		64
/* Long filename support enabled (uses stack buffer)
 * Max 64 chars is plenty for save filenames like "zork1_westhouse.sav" */

#define FF_LFN_UNICODE	0
/* ANSI/OEM encoding for LFN */

#define FF_LFN_BUF		64
#define FF_SFN_BUF		12
/* LFN working buffer on stack (~130 bytes per open file operation) */

#define FF_FS_RPATH		2
/* Support relative paths and getcwd() */

/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#define FF_VOLUMES		1
/* Single volume - SD card only */

#define FF_STR_VOLUME_ID	0
/* Use numeric drive numbers (0:, 1:, etc.) */

#define FF_MULTI_PARTITION	0
/* Single partition per drive */

#define FF_MIN_SS		512
#define FF_MAX_SS		512
/* Standard 512-byte sectors */

#define FF_LBA64		0
/* 32-bit LBA is sufficient */

#define FF_MIN_GPT		0x10000000
/* Not using GPT */

#define FF_USE_TRIM		0
/* Not needed */

/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_TINY		0
/* Normal buffer mode - we have enough RAM */

#define FF_FS_EXFAT		0
/* Don't need exFAT - standard FAT is fine */

#define FF_FS_NORTC		1
#define FF_NORTC_MON	1
#define FF_NORTC_MDAY	1
#define FF_NORTC_YEAR	2025
/* No RTC - use fixed timestamp for saves */

#define FF_FS_NOFSINFO	0
/* Trust FSINFO for faster operation */

#define FF_FS_LOCK		0
/* File lock disabled - single-threaded file access
 * (only fizmo task accesses SD card) */

#define FF_FS_REENTRANT	1
#define FF_FS_TIMEOUT	1000
#define FF_SYNC_t		SemaphoreHandle_t
/* FreeRTOS re-entrant mode enabled for safety
 * Timeout: 1000 ticks (1 second with typical 1ms tick) */

/*---------------------------------------------------------------------------/
/ Additional Configuration (NXP SDK specific)
/---------------------------------------------------------------------------*/

#define SD_DISK_ENABLE 1
/* Enable SD card disk */

/*---------------------------------------------------------------------------/
/ Compatibility defines for driver framework
/---------------------------------------------------------------------------*/

/* Map old-style macros to new names for ff_gen_drv.h compatibility */
#define _USE_WRITE  (FF_FS_READONLY == 0)
#define _USE_IOCTL  1
#define _VOLUMES    FF_VOLUMES

#endif /* _FFCONF_H_ */
