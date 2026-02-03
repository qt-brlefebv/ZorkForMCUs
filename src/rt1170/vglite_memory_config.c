/*
 * vglite_memory_config.c
 *
 * Custom VGLite memory configuration for RT1170.
 * Provides larger command buffer for text rendering.
 *
 * To use: define CUSTOM_VGLITE_MEMORY_CONFIG=1 in CMake build.
 */

#include "fsl_common.h"
#include <stdint.h>

/* VGLite heap size - 2MB (default) */
#define VGLITE_HEAP_SIZE 0x200000

/* VGLite command buffer size - 256KB (default) */
#define VGLITE_CMD_BUFFER_SIZE (256 << 10)

/* Memory alignment required by VGLite */
#define VGLITE_MEMORY_ALIGN 32

/* VGLite heap in non-cacheable memory region */
AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t vglite_contiguous_mem[VGLITE_HEAP_SIZE], VGLITE_MEMORY_ALIGN);

/* Global variables used by VGLite driver */
void *vglite_heap_base        = &vglite_contiguous_mem;
uint32_t vglite_heap_size     = VGLITE_HEAP_SIZE;
uint32_t vglite_cmd_buff_size = VGLITE_CMD_BUFFER_SIZE;
