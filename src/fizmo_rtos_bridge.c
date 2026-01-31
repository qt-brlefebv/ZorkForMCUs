/*
 * fizmo_rtos_bridge.c
 *
 * FreeRTOS integration layer for libfizmo Z-machine interpreter.
 * See fizmo_rtos_bridge.h for architecture overview.
 */

#include "fizmo_rtos_bridge.h"

#include <string.h>
#include <stdio.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* libfizmo includes */
#include "interpreter/fizmo.h"
#include "tools/filesys.h"
#include "tools/types.h"
#include "tools/z_ucs.h"
#include "screen_interface/screen_interface.h"
#include "filesys_interface/filesys_interface.h"

/* Forward declarations for screen interface */
static char* rtos_get_interface_name(void);
static bool rtos_is_status_line_available(void);
static bool rtos_is_split_screen_available(void);
static bool rtos_is_variable_pitch_font_default(void);
static bool rtos_is_colour_available(void);
static bool rtos_is_picture_displaying_available(void);
static bool rtos_is_bold_face_available(void);
static bool rtos_is_italic_available(void);
static bool rtos_is_fixed_space_font_available(void);
static bool rtos_is_timed_keyboard_input_available(void);
static bool rtos_is_preloaded_input_available(void);
static bool rtos_is_character_graphics_font_availiable(void);
static bool rtos_is_picture_font_availiable(void);
static uint16_t rtos_get_screen_height_in_lines(void);
static uint16_t rtos_get_screen_width_in_characters(void);
static uint16_t rtos_get_screen_width_in_units(void);
static uint16_t rtos_get_screen_height_in_units(void);
static uint8_t rtos_get_font_width_in_units(void);
static uint8_t rtos_get_font_height_in_units(void);
static z_colour rtos_get_default_foreground_colour(void);
static z_colour rtos_get_default_background_colour(void);
static uint8_t rtos_get_total_width_in_pixels_of_text_sent_to_output_stream_3(void);
static int rtos_parse_config_parameter(char *key, char *value);
static char* rtos_get_config_value(char *key);
static char** rtos_get_config_option_names(void);
static void rtos_link_interface_to_story(struct z_story *story);
static void rtos_reset_interface(void);
static int rtos_close_interface(z_ucs *error_message);
static void rtos_set_buffer_mode(uint8_t new_buffer_mode);
static void rtos_z_ucs_output(z_ucs *z_ucs_output);
static int16_t rtos_read_line(zscii *dest, uint16_t maximum_length,
    uint16_t tenth_seconds, uint32_t verification_routine,
    uint8_t preloaded_input, int *tenth_seconds_elapsed,
    bool disable_command_history, bool return_on_escape);
static int rtos_read_char(uint16_t tenth_seconds, uint32_t verification_routine,
    int *tenth_seconds_elapsed);
static void rtos_show_status(z_ucs *room_description, int status_line_mode,
    int16_t parameter1, int16_t parameter2);
static void rtos_set_text_style(z_style text_style);
static void rtos_set_colour(z_colour foreground, z_colour background, int16_t window);
static void rtos_set_font(z_font font_type);
static void rtos_split_window(int16_t nof_lines);
static void rtos_set_window(int16_t window_number);
static void rtos_erase_window(int16_t window_number);
static void rtos_set_cursor(int16_t line, int16_t column, int16_t window);
static uint16_t rtos_get_cursor_row(void);
static uint16_t rtos_get_cursor_column(void);
static void rtos_erase_line_value(uint16_t start_position);
static void rtos_erase_line_pixels(uint16_t start_position);
static void rtos_output_interface_info(void);
static bool rtos_input_must_be_repeated_by_story(void);
static void rtos_game_was_restored_and_history_modified(void);

/* Screen interface structure */
static struct z_screen_interface rtos_screen_interface = {
    .get_interface_name = rtos_get_interface_name,
    .is_status_line_available = rtos_is_status_line_available,
    .is_split_screen_available = rtos_is_split_screen_available,
    .is_variable_pitch_font_default = rtos_is_variable_pitch_font_default,
    .is_colour_available = rtos_is_colour_available,
    .is_picture_displaying_available = rtos_is_picture_displaying_available,
    .is_bold_face_available = rtos_is_bold_face_available,
    .is_italic_available = rtos_is_italic_available,
    .is_fixed_space_font_available = rtos_is_fixed_space_font_available,
    .is_timed_keyboard_input_available = rtos_is_timed_keyboard_input_available,
    .is_preloaded_input_available = rtos_is_preloaded_input_available,
    .is_character_graphics_font_availiable = rtos_is_character_graphics_font_availiable,
    .is_picture_font_availiable = rtos_is_picture_font_availiable,
    .get_screen_height_in_lines = rtos_get_screen_height_in_lines,
    .get_screen_width_in_characters = rtos_get_screen_width_in_characters,
    .get_screen_width_in_units = rtos_get_screen_width_in_units,
    .get_screen_height_in_units = rtos_get_screen_height_in_units,
    .get_font_width_in_units = rtos_get_font_width_in_units,
    .get_font_height_in_units = rtos_get_font_height_in_units,
    .get_default_foreground_colour = rtos_get_default_foreground_colour,
    .get_default_background_colour = rtos_get_default_background_colour,
    .get_total_width_in_pixels_of_text_sent_to_output_stream_3 =
        rtos_get_total_width_in_pixels_of_text_sent_to_output_stream_3,
    .parse_config_parameter = rtos_parse_config_parameter,
    .get_config_value = rtos_get_config_value,
    .get_config_option_names = rtos_get_config_option_names,
    .link_interface_to_story = rtos_link_interface_to_story,
    .reset_interface = rtos_reset_interface,
    .close_interface = rtos_close_interface,
    .set_buffer_mode = rtos_set_buffer_mode,
    .z_ucs_output = rtos_z_ucs_output,
    .read_line = rtos_read_line,
    .read_char = rtos_read_char,
    .show_status = rtos_show_status,
    .set_text_style = rtos_set_text_style,
    .set_colour = rtos_set_colour,
    .set_font = rtos_set_font,
    .split_window = rtos_split_window,
    .set_window = rtos_set_window,
    .erase_window = rtos_erase_window,
    .set_cursor = rtos_set_cursor,
    .get_cursor_row = rtos_get_cursor_row,
    .get_cursor_column = rtos_get_cursor_column,
    .erase_line_value = rtos_erase_line_value,
    .erase_line_pixels = rtos_erase_line_pixels,
    .output_interface_info = rtos_output_interface_info,
    .input_must_be_repeated_by_story = rtos_input_must_be_repeated_by_story,
    .game_was_restored_and_history_modified = rtos_game_was_restored_and_history_modified,
    .prompt_for_filename = NULL,
    .do_autosave = NULL,
    .restore_autosave = NULL
};

/* RTOS synchronization primitives */
static QueueHandle_t s_output_queue = NULL;
static SemaphoreHandle_t s_input_ready_sem = NULL;
static SemaphoreHandle_t s_state_mutex = NULL;

/* State variables (protected by s_state_mutex) */
static volatile bool s_waiting_for_line = false;
static volatile bool s_waiting_for_char = false;
static volatile bool s_fizmo_exited = false;

/* Input buffer */
static char s_input_buffer[FIZMO_INPUT_BUFFER_SIZE];
static volatile size_t s_input_length = 0;
static volatile uint32_t s_input_char = 0;

/* Status line (for V1-V3 games) */
static char s_status_room[64];
static char s_status_score[32];
static volatile bool s_status_valid = false;

/* Current cursor position */
static uint16_t s_cursor_row = 1;
static uint16_t s_cursor_column = 1;

/*
 * Helper: send a single z_ucs character to the output queue
 */
static void output_char(z_ucs ch)
{
    uint32_t ch32 = (uint32_t)ch;
    /* Non-blocking send; if queue is full, character is dropped */
    xQueueSend(s_output_queue, &ch32, 0);
}

/*
 * Public API implementation
 */

int fizmo_bridge_init(void)
{
    /* Create output queue */
    s_output_queue = xQueueCreate(FIZMO_OUTPUT_QUEUE_SIZE, sizeof(uint32_t));
    if (s_output_queue == NULL) {
        return -1;
    }

    /* Create input semaphore (binary) */
    s_input_ready_sem = xSemaphoreCreateBinary();
    if (s_input_ready_sem == NULL) {
        vQueueDelete(s_output_queue);
        return -1;
    }

    /* Create state mutex */
    s_state_mutex = xSemaphoreCreateMutex();
    if (s_state_mutex == NULL) {
        vQueueDelete(s_output_queue);
        vSemaphoreDelete(s_input_ready_sem);
        return -1;
    }

    /* Initialize state */
    s_waiting_for_line = false;
    s_waiting_for_char = false;
    s_fizmo_exited = false;
    s_input_length = 0;
    s_status_valid = false;

    /* Register screen interface with libfizmo */
    int result = fizmo_register_screen_interface(&rtos_screen_interface);
    if (result != 0) {
        return -1;
    }

    return 0;
}

int fizmo_bridge_run(const uint8_t *story_data, size_t story_size)
{
    /*
     * Note: The filesystem interface should already be registered
     * by fizmo_filesys_hybrid.c before calling this function.
     * The story z_file is opened via that interface.
     */

    printf("[fizmo_bridge_run] Opening story file...\n");
    fflush(stdout);

    /* Open story from embedded data via the registered filesys interface */
    z_file *story_file = fsi->openfile("@embedded", FILETYPE_DATA, FILEACCESS_READ);
    if (story_file == NULL) {
        printf("[fizmo_bridge_run] FAILED to open story\n");
        return -1;
    }
    printf("[fizmo_bridge_run] Story opened, calling fizmo_start...\n");
    fflush(stdout);

    /* Start the interpreter - this blocks until game ends */
    fizmo_start(story_file, NULL, NULL);
    printf("[fizmo_bridge_run] fizmo_start returned\n");

    /* Mark as exited */
    xSemaphoreTake(s_state_mutex, portMAX_DELAY);
    s_fizmo_exited = true;
    xSemaphoreGive(s_state_mutex);

    return 0;
}

size_t fizmo_output_available(void)
{
    if (s_output_queue == NULL) {
        return 0;
    }
    return (size_t)uxQueueMessagesWaiting(s_output_queue);
}

size_t fizmo_output_read(uint32_t *buffer, size_t max_chars)
{
    if (s_output_queue == NULL || buffer == NULL || max_chars == 0) {
        return 0;
    }

    size_t count = 0;
    uint32_t ch;

    while (count < max_chars && xQueueReceive(s_output_queue, &ch, 0) == pdTRUE) {
        buffer[count++] = ch;
    }

    return count;
}

bool fizmo_waiting_for_input(void)
{
    bool waiting = false;
    if (xSemaphoreTake(s_state_mutex, portMAX_DELAY) == pdTRUE) {
        waiting = s_waiting_for_line;
        xSemaphoreGive(s_state_mutex);
    }
    return waiting;
}

bool fizmo_waiting_for_char(void)
{
    bool waiting = false;
    if (xSemaphoreTake(s_state_mutex, portMAX_DELAY) == pdTRUE) {
        waiting = s_waiting_for_char;
        xSemaphoreGive(s_state_mutex);
    }
    return waiting;
}

void fizmo_submit_line(const char *line)
{
    if (line == NULL) {
        return;
    }

    xSemaphoreTake(s_state_mutex, portMAX_DELAY);

    /* Copy input to buffer */
    size_t len = strlen(line);
    if (len >= FIZMO_INPUT_BUFFER_SIZE) {
        len = FIZMO_INPUT_BUFFER_SIZE - 1;
    }
    memcpy(s_input_buffer, line, len);
    s_input_buffer[len] = '\0';
    s_input_length = len;

    xSemaphoreGive(s_state_mutex);

    /* Signal the fizmo task that input is ready */
    xSemaphoreGive(s_input_ready_sem);
}

void fizmo_submit_char(uint32_t ch)
{
    xSemaphoreTake(s_state_mutex, portMAX_DELAY);
    s_input_char = ch;
    xSemaphoreGive(s_state_mutex);

    /* Signal the fizmo task that input is ready */
    xSemaphoreGive(s_input_ready_sem);
}

bool fizmo_get_status_line(char *room, size_t room_size,
                           char *score_or_time, size_t score_size)
{
    bool valid = false;

    xSemaphoreTake(s_state_mutex, portMAX_DELAY);

    if (s_status_valid) {
        if (room && room_size > 0) {
            strncpy(room, s_status_room, room_size - 1);
            room[room_size - 1] = '\0';
        }
        if (score_or_time && score_size > 0) {
            strncpy(score_or_time, s_status_score, score_size - 1);
            score_or_time[score_size - 1] = '\0';
        }
        valid = true;
    }

    xSemaphoreGive(s_state_mutex);

    return valid;
}

bool fizmo_has_exited(void)
{
    bool exited = false;
    if (xSemaphoreTake(s_state_mutex, portMAX_DELAY) == pdTRUE) {
        exited = s_fizmo_exited;
        xSemaphoreGive(s_state_mutex);
    }
    return exited;
}

uint16_t fizmo_get_screen_width(void)
{
    return FIZMO_SCREEN_WIDTH;
}

uint16_t fizmo_get_screen_height(void)
{
    return FIZMO_SCREEN_HEIGHT;
}

/*
 * Screen interface implementation
 */

static char* rtos_get_interface_name(void)
{
    return "fizmo-rtos-bridge";
}

static bool rtos_is_status_line_available(void)
{
    return true;  /* V1-V3 games use status line */
}

static bool rtos_is_split_screen_available(void)
{
    return false;  /* Not implementing split screen for simplicity */
}

static bool rtos_is_variable_pitch_font_default(void)
{
    return false;  /* Fixed-pitch for text adventure */
}

static bool rtos_is_colour_available(void)
{
    return false;  /* Monochrome for now */
}

static bool rtos_is_picture_displaying_available(void)
{
    return false;
}

static bool rtos_is_bold_face_available(void)
{
    return false;
}

static bool rtos_is_italic_available(void)
{
    return false;
}

static bool rtos_is_fixed_space_font_available(void)
{
    return true;
}

static bool rtos_is_timed_keyboard_input_available(void)
{
    return false;  /* Not implementing timed input */
}

static bool rtos_is_preloaded_input_available(void)
{
    return false;
}

static bool rtos_is_character_graphics_font_availiable(void)
{
    return false;
}

static bool rtos_is_picture_font_availiable(void)
{
    return false;
}

static uint16_t rtos_get_screen_height_in_lines(void)
{
    return FIZMO_SCREEN_HEIGHT;
}

static uint16_t rtos_get_screen_width_in_characters(void)
{
    return FIZMO_SCREEN_WIDTH;
}

static uint16_t rtos_get_screen_width_in_units(void)
{
    return FIZMO_SCREEN_WIDTH;
}

static uint16_t rtos_get_screen_height_in_units(void)
{
    return FIZMO_SCREEN_HEIGHT;
}

static uint8_t rtos_get_font_width_in_units(void)
{
    return 1;
}

static uint8_t rtos_get_font_height_in_units(void)
{
    return 1;
}

static z_colour rtos_get_default_foreground_colour(void)
{
    return Z_COLOUR_WHITE;
}

static z_colour rtos_get_default_background_colour(void)
{
    return Z_COLOUR_BLACK;
}

static uint8_t rtos_get_total_width_in_pixels_of_text_sent_to_output_stream_3(void)
{
    return 0;
}

static int rtos_parse_config_parameter(char *key, char *value)
{
    (void)key;
    (void)value;
    return 1;  /* Parameter not recognized */
}

static char* rtos_get_config_value(char *key)
{
    (void)key;
    return NULL;
}

static char** rtos_get_config_option_names(void)
{
    return NULL;
}

static void rtos_link_interface_to_story(struct z_story *story)
{
    (void)story;
    /* Could inspect story->version here if needed */
}

static void rtos_reset_interface(void)
{
    /* Clear output queue on restart */
    if (s_output_queue != NULL) {
        xQueueReset(s_output_queue);
    }

    xSemaphoreTake(s_state_mutex, portMAX_DELAY);
    s_status_valid = false;
    s_cursor_row = 1;
    s_cursor_column = 1;
    xSemaphoreGive(s_state_mutex);
}

static int rtos_close_interface(z_ucs *error_message)
{
    if (error_message != NULL) {
        /* Output error message before closing */
        rtos_z_ucs_output(error_message);
    }
    return 0;
}

static void rtos_set_buffer_mode(uint8_t new_buffer_mode)
{
    (void)new_buffer_mode;
    /* Buffering handled by output queue */
}

static void rtos_z_ucs_output(z_ucs *z_ucs_output)
{
    if (z_ucs_output == NULL) {
        return;
    }

    /* Send each character to the output queue */
    while (*z_ucs_output != 0) {
        output_char(*z_ucs_output);
        z_ucs_output++;
    }
}

static int16_t rtos_read_line(zscii *dest, uint16_t maximum_length,
    uint16_t tenth_seconds, uint32_t verification_routine,
    uint8_t preloaded_input, int *tenth_seconds_elapsed,
    bool disable_command_history, bool return_on_escape)
{
    (void)tenth_seconds;
    (void)verification_routine;
    (void)preloaded_input;
    (void)disable_command_history;
    (void)return_on_escape;

    if (tenth_seconds_elapsed != NULL) {
        *tenth_seconds_elapsed = 0;
    }

    /* Signal that we're waiting for input */
    xSemaphoreTake(s_state_mutex, portMAX_DELAY);
    s_waiting_for_line = true;
    s_input_length = 0;
    xSemaphoreGive(s_state_mutex);

    /* Block until input is submitted */
    xSemaphoreTake(s_input_ready_sem, portMAX_DELAY);

    /* Copy input to destination */
    xSemaphoreTake(s_state_mutex, portMAX_DELAY);
    s_waiting_for_line = false;

    size_t len = s_input_length;
    if (len > maximum_length) {
        len = maximum_length;
    }

    /* Convert from UTF-8 to ZSCII (simplified: assumes ASCII subset) */
    for (size_t i = 0; i < len; i++) {
        dest[i] = (zscii)s_input_buffer[i];
    }

    xSemaphoreGive(s_state_mutex);

    return (int16_t)len;
}

static int rtos_read_char(uint16_t tenth_seconds, uint32_t verification_routine,
    int *tenth_seconds_elapsed)
{
    (void)tenth_seconds;
    (void)verification_routine;

    if (tenth_seconds_elapsed != NULL) {
        *tenth_seconds_elapsed = 0;
    }

    /* Signal that we're waiting for a character */
    xSemaphoreTake(s_state_mutex, portMAX_DELAY);
    s_waiting_for_char = true;
    s_input_char = 0;
    xSemaphoreGive(s_state_mutex);

    /* Block until input is submitted */
    xSemaphoreTake(s_input_ready_sem, portMAX_DELAY);

    /* Get the input character */
    xSemaphoreTake(s_state_mutex, portMAX_DELAY);
    s_waiting_for_char = false;
    uint32_t ch = s_input_char;
    xSemaphoreGive(s_state_mutex);

    /* Convert to ZSCII (simplified) */
    if (ch > 255) {
        ch = '?';  /* Replace unsupported characters */
    }

    return (int)ch;
}

static void rtos_show_status(z_ucs *room_description, int status_line_mode,
    int16_t parameter1, int16_t parameter2)
{
    xSemaphoreTake(s_state_mutex, portMAX_DELAY);

    /* Convert room description from z_ucs to UTF-8 */
    if (room_description != NULL) {
        size_t i = 0;
        while (room_description[i] != 0 && i < sizeof(s_status_room) - 1) {
            /* Simplified: assume ASCII range */
            s_status_room[i] = (char)(room_description[i] & 0x7F);
            i++;
        }
        s_status_room[i] = '\0';
    } else {
        s_status_room[0] = '\0';
    }

    /* Format score/time based on mode */
    if (status_line_mode == SCORE_MODE_TIME) {
        snprintf(s_status_score, sizeof(s_status_score), "%02d:%02d",
                 parameter1, parameter2);
    } else {
        snprintf(s_status_score, sizeof(s_status_score), "Score: %d  Moves: %d",
                 parameter1, parameter2);
    }

    s_status_valid = true;

    xSemaphoreGive(s_state_mutex);
}

static void rtos_set_text_style(z_style text_style)
{
    (void)text_style;
    /* Text styling not implemented */
}

static void rtos_set_colour(z_colour foreground, z_colour background, int16_t window)
{
    (void)foreground;
    (void)background;
    (void)window;
    /* Color not implemented */
}

static void rtos_set_font(z_font font_type)
{
    (void)font_type;
    /* Font changes not implemented */
}

static void rtos_split_window(int16_t nof_lines)
{
    (void)nof_lines;
    /* Split window not implemented */
}

static void rtos_set_window(int16_t window_number)
{
    (void)window_number;
    /* Window switching not implemented */
}

static void rtos_erase_window(int16_t window_number)
{
    (void)window_number;
    /* Window erasing not implemented */
}

static void rtos_set_cursor(int16_t line, int16_t column, int16_t window)
{
    (void)window;
    s_cursor_row = (uint16_t)line;
    s_cursor_column = (uint16_t)column;
}

static uint16_t rtos_get_cursor_row(void)
{
    return s_cursor_row;
}

static uint16_t rtos_get_cursor_column(void)
{
    return s_cursor_column;
}

static void rtos_erase_line_value(uint16_t start_position)
{
    (void)start_position;
    /* Line erasing not implemented */
}

static void rtos_erase_line_pixels(uint16_t start_position)
{
    (void)start_position;
    /* Line erasing not implemented */
}

static void rtos_output_interface_info(void)
{
    /* Output interface identification */
    z_ucs info[] = {'[', 'f', 'i', 'z', 'm', 'o', '-', 'r', 't', 'o', 's', ']', '\n', 0};
    rtos_z_ucs_output(info);
}

static bool rtos_input_must_be_repeated_by_story(void)
{
    return false;  /* We handle input echo in FizmoBackend::submitCommand() */
}

static void rtos_game_was_restored_and_history_modified(void)
{
    /* Could trigger UI refresh here */
}
