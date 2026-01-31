/*
 * diskio_stub.c
 *
 * Minimal FatFS disk I/O stubs for linking.
 * These stubs allow the build to complete and binary size to be assessed.
 * Real SD card driver implementation can be added later if space permits.
 */

#include "ff.h"
#include "diskio.h"

/* Disk status */
DSTATUS disk_status(BYTE pdrv)
{
    (void)pdrv;
    return STA_NOINIT;  /* Not initialized */
}

/* Initialize disk drive */
DSTATUS disk_initialize(BYTE pdrv)
{
    (void)pdrv;
    return STA_NOINIT;  /* Fail - not implemented */
}

/* Read sector(s) */
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    (void)pdrv;
    (void)buff;
    (void)sector;
    (void)count;
    return RES_ERROR;  /* Fail - not implemented */
}

/* Write sector(s) */
#if FF_FS_READONLY == 0
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    (void)pdrv;
    (void)buff;
    (void)sector;
    (void)count;
    return RES_ERROR;  /* Fail - not implemented */
}
#endif

/* Miscellaneous control functions */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    (void)pdrv;
    (void)cmd;
    (void)buff;
    return RES_ERROR;  /* Fail - not implemented */
}
