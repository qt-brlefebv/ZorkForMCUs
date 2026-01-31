/*
 * fizmo_rtos_bridge.h
 *
 * FreeRTOS integration layer for libfizmo Z-machine interpreter.
 * Provides screen interface implementation and inter-task communication
 * for Qt for MCUs + FreeRTOS on NXP RT1050.
 *
 * Architecture:
 *   - Fizmo task: runs fizmo_start(), blocks on read_line/read_char
 *   - Qt task: runs event loop, polls for output, submits input
 *   - Communication: FreeRTOS queue (output), semaphores (input sync)
 */

#ifndef FIZMO_RTOS_BRIDGE_H
#define FIZMO_RTOS_BRIDGE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Configuration */
#ifndef FIZMO_OUTPUT_QUEUE_SIZE
#define FIZMO_OUTPUT_QUEUE_SIZE     2048
#endif

#ifndef FIZMO_INPUT_BUFFER_SIZE
#define FIZMO_INPUT_BUFFER_SIZE     256
#endif

#ifndef FIZMO_SCREEN_WIDTH
#define FIZMO_SCREEN_WIDTH          80
#endif

#ifndef FIZMO_SCREEN_HEIGHT
#define FIZMO_SCREEN_HEIGHT         24
#endif

/*
 * Initialize the RTOS bridge.
 * Must be called before starting the fizmo task.
 * Creates queues, semaphores, and registers interfaces with libfizmo.
 *
 * Returns: 0 on success, -1 on failure
 */
int fizmo_bridge_init(void);

/*
 * Start the fizmo interpreter with embedded story.
 * This function blocks and should be called from the fizmo FreeRTOS task.
 * It will not return until the game ends or an error occurs.
 *
 * story_data: pointer to story file data in flash
 * story_size: size of story data in bytes
 *
 * Returns: 0 on normal exit, non-zero on error
 */
int fizmo_bridge_run(const uint8_t *story_data, size_t story_size);

/*
 * Check if output is available from fizmo.
 * Non-blocking, safe to call from Qt task.
 *
 * Returns: number of characters available in output queue
 */
size_t fizmo_output_available(void);

/*
 * Read output characters from fizmo.
 * Non-blocking, safe to call from Qt task.
 *
 * buffer: destination buffer for UTF-32 characters
 * max_chars: maximum number of characters to read
 *
 * Returns: number of characters actually read
 */
size_t fizmo_output_read(uint32_t *buffer, size_t max_chars);

/*
 * Check if fizmo is waiting for line input.
 * Non-blocking, safe to call from Qt task.
 *
 * Returns: true if fizmo is blocked waiting for input
 */
bool fizmo_waiting_for_input(void);

/*
 * Check if fizmo is waiting for single character input.
 * Non-blocking, safe to call from Qt task.
 *
 * Returns: true if fizmo is blocked waiting for a single keypress
 */
bool fizmo_waiting_for_char(void);

/*
 * Submit a line of input to fizmo.
 * Safe to call from Qt task. Unblocks fizmo's read_line.
 *
 * line: null-terminated UTF-8 string
 */
void fizmo_submit_line(const char *line);

/*
 * Submit a single character to fizmo.
 * Safe to call from Qt task. Unblocks fizmo's read_char.
 *
 * ch: Unicode code point
 */
void fizmo_submit_char(uint32_t ch);

/*
 * Get the current status line text (for V1-V3 games).
 * Safe to call from Qt task.
 *
 * room: buffer for room name (should be at least 64 chars)
 * score_or_time: buffer for score/time string (should be at least 32 chars)
 *
 * Returns: true if status line data is available
 */
bool fizmo_get_status_line(char *room, size_t room_size,
                           char *score_or_time, size_t score_size);

/*
 * Check if the interpreter has finished (game quit or error).
 *
 * Returns: true if fizmo has exited
 */
bool fizmo_has_exited(void);

/*
 * Get screen dimensions.
 */
uint16_t fizmo_get_screen_width(void);
uint16_t fizmo_get_screen_height(void);

#ifdef __cplusplus
}
#endif

#endif /* FIZMO_RTOS_BRIDGE_H */
