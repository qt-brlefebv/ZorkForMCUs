/*
 * Copyright 2020-2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SDMMC_CONFIG_H_
#define _SDMMC_CONFIG_H_

#include "fsl_common.h"

#ifdef SD_ENABLED
#include "fsl_sd.h"
#endif
#ifdef MMC_ENABLED
#include "fsl_mmc.h"
#endif
#ifdef SDIO_ENABLED
#include "fsl_sdio.h"
#endif

#include "fsl_adapter_gpio.h"
#include "fsl_sdmmc_host.h"
#include "fsl_sdmmc_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* @brief host basic configuration */
#define BOARD_SDMMC_SD_HOST_BASEADDR   USDHC1
#define BOARD_SDMMC_SD_HOST_IRQ        USDHC1_IRQn
#define BOARD_SDMMC_MMC_HOST_BASEADDR  USDHC1
#define BOARD_SDMMC_MMC_HOST_IRQ       USDHC1_IRQn
#define BOARD_SDMMC_SDIO_HOST_BASEADDR USDHC1
#define BOARD_SDMMC_SDIO_HOST_IRQ      USDHC1_IRQn
/* @brief card detect configuration - RT1170 uses GPIO3 pin 31 */
#define BOARD_SDMMC_SD_CD_GPIO_BASE      GPIO3
#define BOARD_SDMMC_SD_CD_GPIO_PORT      3
#define BOARD_SDMMC_SD_CD_GPIO_PIN       31U
#define BOARD_SDMMC_SD_CD_INTTERUPT_TYPE kHAL_GpioInterruptEitherEdge
#define BOARD_SDMMC_SD_CD_INSERT_LEVEL   (0U)
/* @brief card detect type */
#ifndef BOARD_SDMMC_SD_CD_TYPE
#define BOARD_SDMMC_SD_CD_TYPE kSD_DetectCardByGpioCD
#endif
#define BOARD_SDMMC_SD_CARD_DETECT_DEBOUNCE_DELAY_MS (100U)
/*! @brief SD power reset - RT1170 uses GPIO10 pin 2 */
#define BOARD_SDMMC_SD_POWER_RESET_GPIO_BASE GPIO10
#define BOARD_SDMMC_SD_POWER_RESET_GPIO_PORT 10
#define BOARD_SDMMC_SD_POWER_RESET_GPIO_PIN  2U
/*! @brief SD IO voltage */
#define BOARD_SDMMC_SD_IO_VOLTAGE_CONTROL_TYPE kSD_IOVoltageCtrlByHost

#define BOARD_SDMMC_SD_HOST_SUPPORT_SDR104_FREQ (200000000U)
#define BOARD_SDMMC_MMC_HOST_SUPPORT_HS200_FREQ (200000000U)
/*! @brief mmc configuration */
#define BOARD_SDMMC_MMC_VCC_SUPPLY  kMMC_VoltageWindows270to360
#define BOARD_SDMMC_MMC_VCCQ_SUPPLY kMMC_VoltageWindows270to360
/*! @brief align with cache line size */
#define BOARD_SDMMC_DATA_BUFFER_ALIGN_SIZE       (32U)
#define BOARD_SDMMC_MMC_SUPPORT_8_BIT_DATA_WIDTH 1U
#define BOARD_SDMMC_MMC_TUNING_TYPE              0
/*!@ brief host interrupt priority*/
#define BOARD_SDMMC_SD_HOST_IRQ_PRIORITY   (5U)
#define BOARD_SDMMC_MMC_HOST_IRQ_PRIORITY  (5U)
#define BOARD_SDMMC_SDIO_HOST_IRQ_PRIORITY (5U)
/*!@brief dma descriptor buffer size */
#define BOARD_SDMMC_HOST_DMA_DESCRIPTOR_BUFFER_SIZE (32U)
/*! @brief cache maintain function enabled for RW buffer */
#define BOARD_SDMMC_HOST_CACHE_CONTROL kSDMMCHOST_CacheControlRWBuffer

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/
#ifdef SD_ENABLED
void BOARD_SD_Config(void *card, sd_cd_t cd, uint32_t hostIRQPriority, void *userData);
#endif

#ifdef SDIO_ENABLED
void BOARD_SDIO_Config(void *card, sd_cd_t cd, uint32_t hostIRQPriority, sdio_int_t cardInt);
#endif

#ifdef MMC_ENABLED
void BOARD_MMC_Config(void *card, uint32_t hostIRQPriority);
#endif

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SDMMC_CONFIG_H_ */
