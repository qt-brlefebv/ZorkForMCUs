/*
 * story_data.S
 *
 * Embeds the Z-machine story file into the firmware binary.
 * Uses .incbin directive to include raw binary data.
 *
 * The story file is placed in .rodata section, which the linker
 * will place in flash memory (typically at 0x60000000 on RT1050).
 *
 * To use:
 *   1. Place your story file (e.g., ZORK1.DAT) in the same directory
 *      as this file, or update the path in .incbin
 *   2. Build the project - the story will be embedded in flash
 *
 * Note: Story file path is relative to this .S file location.
 *       Adjust STORY_FILE_PATH as needed for your build setup.
 */

/*
 * Configuration: path to story file
 * Can be overridden at build time with -DSTORY_FILE_PATH="path/to/story.z3"
 */
#ifndef STORY_FILE_PATH
#define STORY_FILE_PATH "ZORK1.DAT"
#endif

    .section .rodata
    .balign 4

    .global story_data_start
    .global story_data_end

story_data_start:
    .incbin STORY_FILE_PATH
story_data_end:

    /* Ensure alignment after story data */
    .balign 4

/*
 * Alternative: if .incbin doesn't work with your assembler,
 * you can use objcopy to convert the binary to an object file:
 *
 *   arm-none-eabi-objcopy -I binary -O elf32-littlearm \
 *       --rename-section .data=.rodata,alloc,load,readonly,data,contents \
 *       ZORK1.DAT story_data.o
 *
 * This creates symbols:
 *   _binary_ZORK1_DAT_start
 *   _binary_ZORK1_DAT_end
 *   _binary_ZORK1_DAT_size
 */
