/*
 * fizmo_bridge.h
 *
 * Bridge layer between Qt for MCUs and the fizmo Z-machine interpreter.
 * Runs fizmo in a background thread, provides thread-safe communication.
 *
 * This is the desktop implementation using std::thread.
 * For FreeRTOS, use fizmo_rtos_bridge.h instead.
 */

#ifndef FIZMO_BRIDGE_H
#define FIZMO_BRIDGE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialize the fizmo bridge with a story file path.
 * Must be called before fizmo_start_interpreter().
 * Returns 0 on success, -1 on error.
 */
int fizmo_bridge_init(const char *story_path);

/*
 * Start the fizmo interpreter in a background thread.
 * fizmo_bridge_init() must have been called first.
 * Returns 0 on success, -1 on error.
 */
int fizmo_start_interpreter(void);

/*
 * Shutdown the fizmo bridge and stop the interpreter thread.
 */
void fizmo_bridge_shutdown(void);

/*
 * Output interface - called by Qt to poll for output
 */

/* Returns number of characters available in output buffer */
size_t fizmo_output_available(void);

/* Read characters from output buffer. Returns number actually read.
 * Characters are UTF-32 (z_ucs) code points. */
size_t fizmo_output_read(uint32_t *buffer, size_t max_chars);

/*
 * Status interface
 */

/* Returns true if fizmo is waiting for line input */
bool fizmo_waiting_for_input(void);

/* Returns true if fizmo is waiting for single character input */
bool fizmo_waiting_for_char(void);

/* Returns true if the game has exited */
bool fizmo_has_exited(void);

/* Get current status line. Returns true if status is available.
 * room and score_or_time are output buffers. */
bool fizmo_get_status_line(char *room, size_t room_size,
                           char *score_or_time, size_t score_size);

/*
 * Input interface - called by Qt to submit user input
 */

/* Submit a line of input. Wakes up fizmo if it was waiting. */
void fizmo_submit_line(const char *line);

/* Submit a single character. Wakes up fizmo if it was waiting. */
void fizmo_submit_char(uint32_t ch);

#ifdef __cplusplus
}
#endif

#endif /* FIZMO_BRIDGE_H */
