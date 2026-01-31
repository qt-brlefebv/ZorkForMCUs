/*
 * story_data.h
 *
 * C declarations for embedded story file symbols.
 * The actual data is defined in story_data.S using .incbin directive.
 *
 * Usage:
 *   #include "story_data.h"
 *   fizmo_filesys_hybrid_init(story_data_start,
 *                             (size_t)(story_data_end - story_data_start),
 *                             "/saves/");
 */

#ifndef STORY_DATA_H
#define STORY_DATA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Symbols defined in story_data.S
 *
 * story_data_start: pointer to first byte of embedded story file
 * story_data_end:   pointer to byte after last byte of story file
 *
 * Size can be calculated as: (story_data_end - story_data_start)
 */
extern const uint8_t story_data_start[];
extern const uint8_t story_data_end[];

/*
 * Convenience macro to get story size
 */
#define STORY_DATA_SIZE ((size_t)(story_data_end - story_data_start))

#ifdef __cplusplus
}
#endif

#endif /* STORY_DATA_H */
