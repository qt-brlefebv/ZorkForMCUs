/*
 * main_freertos.cpp
 *
 * FreeRTOS entry point for ZorkUI on NXP RT1050-EVK.
 * Creates two tasks:
 *   1. Qul_Thread - Qt for MCUs UI event loop (higher priority)
 *   2. Fizmo_Thread - libfizmo interpreter (lower priority)
 *
 * Communication between tasks happens via the fizmo_rtos_bridge API.
 */

#include <cstdio>

#ifdef DISPLAY_RT1050
#include "ZorkUI_RT1050.h"
#else
#include "ZorkUI.h"
#endif
#include "story_data.h"
#include "fizmo_rtos_bridge.h"

extern "C" {
#include "fizmo_filesys_hybrid.h"
#include "sd_init.h"
}

#include <qul/application.h>
#include <qul/qul.h>
#include <platforminterface/log.h>

#include <FreeRTOS.h>
#include <task.h>

// Task configuration
// Note: QUL_STACK_SIZE is defined by platform's FreeRTOSConfig.h
#ifndef QUL_STACK_SIZE
#define QUL_STACK_SIZE     (6144)       // Qt task stack (6KB) - in words on ARM
#endif
#define FIZMO_STACK_SIZE   (8192)       // Fizmo task stack (8KB) - in words on ARM

#define FIZMO_TASK_PRIORITY (configMAX_PRIORITIES - 1)  // 4 - highest valid
#define QUL_TASK_PRIORITY   (configMAX_PRIORITIES - 2)  // 3 - one below Fizmo

static void Qul_Thread(void *argument);
static void Fizmo_Thread(void *argument);

int main()
{
    // Initialize hardware and platform interfaces
    Qul::initHardware();
    Qul::initPlatform();

    // Initialize fizmo bridge (creates queues/semaphores)
    if (fizmo_bridge_init() != 0) {
        Qul::PlatformInterface::log("ERROR: Fizmo bridge init failed!\r\n");
        configASSERT(false);
    }

    // Initialize hybrid filesystem with embedded story
    // Story is in flash, saves go to SD card under /saves directory
    // NOTE: SD card init/mount happens in Fizmo_Thread (requires scheduler running)
    if (fizmo_filesys_hybrid_init(story_data_start, STORY_DATA_SIZE, "/saves") != 0) {
        Qul::PlatformInterface::log("ERROR: Fizmo filesystem init failed!\r\n");
        configASSERT(false);
    }

    Qul::PlatformInterface::log("ZorkUI: Starting FreeRTOS tasks...\r\n");

    // Create Qt UI task (higher priority for responsive UI)
    if (xTaskCreate(Qul_Thread, "Qul_Thread", QUL_STACK_SIZE,
                    NULL, QUL_TASK_PRIORITY, NULL) != pdPASS) {
        Qul::PlatformInterface::log("ERROR: Qt task creation failed!\r\n");
        configASSERT(false);
    }

    // Create fizmo interpreter task (lower priority, can be preempted by UI)
    if (xTaskCreate(Fizmo_Thread, "Fizmo_Thread", FIZMO_STACK_SIZE,
                    NULL, FIZMO_TASK_PRIORITY, NULL) != pdPASS) {
        Qul::PlatformInterface::log("ERROR: Fizmo task creation failed!\r\n");
        configASSERT(false);
    }

    Qul::PlatformInterface::log("ZorkUI: Starting FreeRTOS scheduler...\r\n");

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Should never reach here
    Qul::PlatformInterface::log("ERROR: Scheduler failed to start!\r\n");
    return 1;
}

static void Qul_Thread(void *argument)
{
    (void)argument;

    Qul::PlatformInterface::log("Qul_Thread: Starting Qt application...\r\n");

    // Create and run Qt application
    Qul::Application app;
#ifdef DISPLAY_RT1050
    static struct ::ZorkUI_RT1050 item;
#else
    static struct ::ZorkUI item;
#endif
    app.setRootItem(&item);

    // Run the Qt event loop (blocks until application exits)
    app.exec();

    Qul::PlatformInterface::log("Qul_Thread: Qt application exited\r\n");
}

static void Fizmo_Thread(void *argument)
{
    (void)argument;

    // Initialize SD card for save/restore (must happen after scheduler starts)
    int sd_ret = sd_filesystem_init();
    if (sd_ret == 0) {
        fizmo_filesys_mount_sd();
        fizmo_load_saved_filename();  // Restore last used filename from SD
    }
    // Continue anyway if SD init fails - game works without save/restore

    Qul::PlatformInterface::log("Fizmo_Thread: Starting interpreter...\r\n");

    // Start the Z-machine interpreter
    // This blocks until the game ends or an error occurs
    int result = fizmo_bridge_run(story_data_start, STORY_DATA_SIZE);

    if (result == 0) {
        Qul::PlatformInterface::log("Fizmo_Thread: Game ended normally\r\n");
    } else {
        Qul::PlatformInterface::log("Fizmo_Thread: Interpreter error!\r\n");
    }

    // Task ends here - could implement restart logic if needed
    vTaskDelete(NULL);
}


// FreeRTOS hook functions for debugging

extern "C" {

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;

    Qul::PlatformInterface::log("FATAL: Stack overflow in task: ");
    Qul::PlatformInterface::log(pcTaskName);
    Qul::PlatformInterface::log("\r\n");

    // Halt execution
    configASSERT(false);
}

void vApplicationMallocFailedHook(void)
{
    Qul::PlatformInterface::log("FATAL: Malloc failed - out of heap memory!\r\n");

    // Halt execution
    configASSERT(false);
}

} // extern "C"
