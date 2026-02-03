/*
 * sd_diskio.c
 * SD card disk I/O driver for FatFS on RT1050
 * Based on Qt for MCUs fileloading example
 */

#include "sd_diskio.h"
#include "ff.h"
#include "diskio.h"

#include "fsl_sd.h"
#include "sdmmc_config.h"

#include <string.h>

/* SD card handle */
static sd_card_t s_sd_card;
static volatile int s_card_initialized = 0;

/* Forward declarations */
static DSTATUS SD_disk_initialize(BYTE lun);
static DSTATUS SD_disk_status(BYTE lun);
static DRESULT SD_disk_read(BYTE lun, BYTE *buff, DWORD sector, UINT count);
static DRESULT SD_disk_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count);
static DRESULT SD_disk_ioctl(BYTE lun, BYTE cmd, void *buff);

/* Driver structure for FatFS */
const Diskio_drvTypeDef SD_Driver = {
    SD_disk_initialize,
    SD_disk_status,
    SD_disk_read,
    SD_disk_write,
    SD_disk_ioctl
};

/* IRQ priority for SDMMC - must be >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
#define SDMMC_IRQ_PRIORITY (5U)

int SD_Card_Init(void)
{
    printf("[SD] SD_Card_Init entry\r\n");

    /* Configure SD card with board-specific settings */
    printf("[SD] Calling BOARD_SD_Config...\r\n");
    BOARD_SD_Config(&s_sd_card, NULL, SDMMC_IRQ_PRIORITY, NULL);
    printf("[SD] BOARD_SD_Config done\r\n");

    /* Initialize host controller */
    printf("[SD] Calling SD_HostInit...\r\n");
    if (SD_HostInit(&s_sd_card) != kStatus_Success) {
        printf("[SD] Host init failed\r\n");
        return -1;
    }
    printf("[SD] SD_HostInit done\r\n");

    /* Check if card is present (non-blocking) */
    printf("[SD] Calling SD_IsCardPresent...\r\n");
    if (!SD_IsCardPresent(&s_sd_card)) {
        printf("[SD] No card detected\r\n");
        return -3;  /* No card - not an error, just unavailable */
    }
    printf("[SD] Card detected\r\n");

    /* Skip power cycle - card is already powered if inserted at boot.
     * Power cycling was causing crashes, likely due to GPIO/HAL issues. */
    printf("[SD] Skipping power cycle (card already powered)\r\n");

    /* Initialize the card */
    printf("[SD] Calling SD_CardInit...\r\n");
    if (SD_CardInit(&s_sd_card) != kStatus_Success) {
        printf("[SD] Card init failed\r\n");
        return -2;
    }

    s_card_initialized = 1;
    printf("[SD] Card initialized successfully\r\n");
    return 0;
}

int SD_Card_IsInserted(void)
{
    return s_card_initialized;
}

static DSTATUS SD_disk_initialize(BYTE lun)
{
    (void)lun;
    /* Card init is done separately in SD_Card_Init() */
    return s_card_initialized ? 0 : STA_NOINIT;
}

static DSTATUS SD_disk_status(BYTE lun)
{
    (void)lun;
    return s_card_initialized ? 0 : STA_NOINIT;
}

static DRESULT SD_disk_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    (void)lun;

    if (!s_card_initialized) {
        return RES_NOTRDY;
    }

    if (SD_ReadBlocks(&s_sd_card, buff, sector, count) != kStatus_Success) {
        return RES_ERROR;
    }

    return RES_OK;
}

static DRESULT SD_disk_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    (void)lun;

    if (!s_card_initialized) {
        return RES_NOTRDY;
    }

    if (SD_WriteBlocks(&s_sd_card, (uint8_t *)buff, sector, count) != kStatus_Success) {
        return RES_ERROR;
    }

    return RES_OK;
}

static DRESULT SD_disk_ioctl(BYTE lun, BYTE cmd, void *buff)
{
    (void)lun;

    if (!s_card_initialized && cmd != CTRL_POWER) {
        return RES_NOTRDY;
    }

    switch (cmd) {
        case CTRL_SYNC:
            /* No buffering in our implementation */
            return RES_OK;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = s_sd_card.blockCount;
            return RES_OK;

        case GET_SECTOR_SIZE:
            *(WORD *)buff = s_sd_card.blockSize;
            return RES_OK;

        case GET_BLOCK_SIZE:
            /* Erase block size in sectors (1 for SD cards) */
            *(DWORD *)buff = 1;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}
