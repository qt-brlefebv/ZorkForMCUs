cmake_minimum_required (VERSION 3.21.1)

project(ZorkUI VERSION 0.0.1 LANGUAGES C CXX ASM)

# Display profile selection
# Must be set before find_package(Qul) to affect board defaults
set(DISPLAY_PROFILE "RT1050" CACHE STRING "Display profile (RT1050, RT1170, or RT1170_SCALED)")
set_property(CACHE DISPLAY_PROFILE PROPERTY STRINGS "RT1050" "RT1170" "RT1170_SCALED")

# Use custom board defaults for virtual keyboard width
if(DISPLAY_PROFILE STREQUAL "RT1170")
    set(QUL_PLATFORM_BOARD_DEFAULTS_QMLPROJECTCONFIG
        "${CMAKE_CURRENT_SOURCE_DIR}/BoardDefaults_720vkb.qmlprojectconfig")
elseif(DISPLAY_PROFILE STREQUAL "RT1170_SCALED")
    set(QUL_PLATFORM_BOARD_DEFAULTS_QMLPROJECTCONFIG
        "${CMAKE_CURRENT_SOURCE_DIR}/BoardDefaults_540vkb.qmlprojectconfig")
elseif(DISPLAY_PROFILE STREQUAL "RT1050")
    set(QUL_PLATFORM_BOARD_DEFAULTS_QMLPROJECTCONFIG
        "${CMAKE_CURRENT_SOURCE_DIR}/BoardDefaults_480vkb.qmlprojectconfig")
endif()

find_package(Qul)

# Path to project root (for libfizmo and src/)
set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../..")

# =============================================================================
# Build Mode Auto-Detection
# =============================================================================
# Automatically detect build mode from platform for OOTB experience.
# Override with -DBUILD_WITH_FREERTOS=OFF or -DBUILD_WITH_FIZMO=OFF if needed.

# Auto-detect FreeRTOS from platform name (e.g., mimxrt1050-evk-freertos)
if(QUL_PLATFORM MATCHES "freertos")
    option(BUILD_WITH_FREERTOS "Build with FreeRTOS support (embedded target)" ON)
else()
    option(BUILD_WITH_FREERTOS "Build with FreeRTOS support (embedded target)" OFF)
endif()

# For desktop builds, default to real fizmo interpreter (not stub)
if(NOT BUILD_WITH_FREERTOS)
    option(BUILD_WITH_FIZMO "Build with real fizmo interpreter (desktop)" ON)
else()
    option(BUILD_WITH_FIZMO "Build with real fizmo interpreter (desktop)" OFF)
endif()

# Diagnostic output - helps users verify correct configuration
if(BUILD_WITH_FREERTOS)
    message(STATUS "Build mode: FreeRTOS with embedded fizmo interpreter")
elseif(BUILD_WITH_FIZMO)
    message(STATUS "Build mode: Desktop with fizmo interpreter (std::thread)")
else()
    message(STATUS "Build mode: Desktop STUB (UI testing only, no game logic)")
endif()

# Verify libfizmo submodule exists (required for non-stub builds)
if(BUILD_WITH_FREERTOS OR BUILD_WITH_FIZMO)
    if(NOT EXISTS "${PROJECT_ROOT}/external/libfizmo/src/interpreter/fizmo.c")
        message(FATAL_ERROR
            "libfizmo not found at ${PROJECT_ROOT}/external/libfizmo\n"
            "Run: git submodule update --init --recursive")
    endif()
endif()

# Determine which qmlproject to use based on display profile
# This applies to all build types (FreeRTOS, desktop with fizmo, desktop stub)
if(DISPLAY_PROFILE STREQUAL "RT1050")
    set(QML_PROJECT_FILE "ZorkUI_RT1050.qmlproject")
    message(STATUS "Using RT1050-optimized qmlproject (ZorkKeyboard, Static fonts)")
else()
    set(QML_PROJECT_FILE "ZorkUI.qmlproject")
    message(STATUS "Using standard qmlproject (VirtualKeyboard, Spark fonts)")
endif()

if(BUILD_WITH_FREERTOS)

    # FreeRTOS build - provide custom main.cpp

    add_compile_definitions(configTOTAL_HEAP_SIZE=\(1024*1024\))

    qul_add_target(ZorkUI
        FizmoBackend.cpp
        main_freertos.cpp
        ${PROJECT_ROOT}/src/fizmo_rtos_bridge.c
        ${PROJECT_ROOT}/src/fizmo_filesys_hybrid.c
        ${PROJECT_ROOT}/src/fizmo_locale_stubs.c
        ${PROJECT_ROOT}/src/diskio_stub.c
        ${PROJECT_ROOT}/src/posix_stubs.c
        ${PROJECT_ROOT}/src/story_data.S
        QML_PROJECT "${QML_PROJECT_FILE}"
        SELECTORS "zork"
    )

    # Include directories - src first so our locale stubs override libfizmo's placeholders
    # src also provides our custom ffconf.h for FatFS
    target_include_directories(ZorkUI BEFORE PRIVATE
        ${PROJECT_ROOT}/src
    )
    target_include_directories(ZorkUI PRIVATE
        ${PROJECT_ROOT}/external/libfizmo/src
        ${QUL_BOARD_SDK_DIR}/middleware/fatfs/source  # For ff.h (FatFS)
    )

    # Add libfizmo sources - explicitly list to exclude problematic files
    set(LIBFIZMO_INTERPRETER_SOURCES
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/blockbuf.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/config.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/fizmo.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/mathemat.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/misc.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/mt19937ar.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/object.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/output.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/property.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/routine.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/savegame.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/sound.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/stack.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/streams.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/table.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/text.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/undo.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/variable.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/wordwrap.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/zpu.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/iff.c"
        # Excluded: babel.c, blorb.c, cmd_hst.c, debugger.c, filelist.c,
        #           history.c, hyphenation.c
    )
    set(LIBFIZMO_TOOLS_SOURCES
        "${PROJECT_ROOT}/external/libfizmo/src/tools/filesys.c"
        # Excluded filesys_c.c - using fizmo_filesys_hybrid.c instead for embedded
        "${PROJECT_ROOT}/external/libfizmo/src/tools/i18n.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/list.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/stringmap.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/tracelog.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/types.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/z_ucs.c"
    )
    target_sources(ZorkUI PRIVATE
        ${LIBFIZMO_INTERPRETER_SOURCES}
        ${LIBFIZMO_TOOLS_SOURCES}
    )

    # Add FatFS sources from SDK
    set(FATFS_SOURCES
        "${QUL_BOARD_SDK_DIR}/middleware/fatfs/source/ff.c"
        "${QUL_BOARD_SDK_DIR}/middleware/fatfs/source/ffsystem.c"
        "${QUL_BOARD_SDK_DIR}/middleware/fatfs/source/ffunicode.c"
    )
    target_sources(ZorkUI PRIVATE ${FATFS_SOURCES})

    # libfizmo compile definitions (disable optional features)
    target_compile_definitions(ZorkUI PRIVATE
        DISABLE_BABEL=1
        DISABLE_FILELIST=1
        DISABLE_CONFIGFILES=1
        DISABLE_COMMAND_HISTORY=1
        DISABLE_OUTPUT_HISTORY=1
        DISABLE_PREFIX_COMMANDS=1
        DISABLE_BLOCKBUFFER=1
        STORY_FILE_PATH="${PROJECT_ROOT}/zork1.z3"
    )

    # Force-include our embedded compatibility header for C/C++ files only (not assembly)
    # This provides locale declarations and blocks unsupported POSIX headers
    target_compile_options(ZorkUI PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-include "${PROJECT_ROOT}/src/fizmo_embedded_compat.h" >
        $<$<COMPILE_LANGUAGE:CXX>:-include "${PROJECT_ROOT}/src/fizmo_embedded_compat.h" >
    )

    # Enable Link-Time Optimization for smaller binary size
    # This allows cross-file inlining and dead code elimination
    set_property(TARGET ZorkUI PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

target_link_options(ZorkUI PRIVATE
    -Wl,--wrap=malloc
    -Wl,--wrap=free
    -Wl,--wrap=calloc
    -Wl,--wrap=realloc
    -Wl,--wrap=strdup
    -Wl,--wrap=strndup
    -Wl,--wrap=_malloc_r
    -Wl,--wrap=_free_r
    -Wl,--wrap=_calloc_r
    -Wl,--wrap=_realloc_r
)

    # Link with FreeRTOS and FatFS (provided by platform)
    # target_link_libraries(ZorkUI PRIVATE freertos_kernel fatfs)

elseif(BUILD_WITH_FIZMO)
    # Desktop build with real fizmo interpreter via std::thread
    qul_add_target(ZorkUI
        FizmoBackend.cpp
        ${PROJECT_ROOT}/src/fizmo_bridge.cpp
        QML_PROJECT "${QML_PROJECT_FILE}"
        SELECTORS "zork"
        GENERATE_ENTRYPOINT
    )

    # Include directories - src first so our locale stubs override libfizmo's placeholders
    target_include_directories(ZorkUI BEFORE PRIVATE
        ${PROJECT_ROOT}/src
    )
    target_include_directories(ZorkUI PRIVATE
        ${PROJECT_ROOT}/external/libfizmo/src
    )

    # Add libfizmo sources - explicitly list to exclude problematic files
    set(LIBFIZMO_INTERPRETER_SOURCES
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/blockbuf.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/config.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/fizmo.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/mathemat.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/misc.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/mt19937ar.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/object.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/output.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/property.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/routine.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/savegame.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/sound.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/stack.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/streams.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/table.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/text.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/undo.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/variable.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/wordwrap.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/zpu.c"
        "${PROJECT_ROOT}/external/libfizmo/src/interpreter/iff.c"
        # Excluded: babel.c, blorb.c, cmd_hst.c, debugger.c, filelist.c,
        #           history.c, hyphenation.c
    )
    set(LIBFIZMO_TOOLS_SOURCES
        "${PROJECT_ROOT}/external/libfizmo/src/tools/filesys.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/filesys_c.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/i18n.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/list.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/stringmap.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/tracelog.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/types.c"
        "${PROJECT_ROOT}/external/libfizmo/src/tools/z_ucs.c"
    )
    # Locale stubs (instead of full locale generation system)
    set(LIBFIZMO_LOCALE_STUBS
        "${PROJECT_ROOT}/src/fizmo_locale_stubs.c"
    )
    target_sources(ZorkUI PRIVATE
        ${LIBFIZMO_INTERPRETER_SOURCES}
        ${LIBFIZMO_TOOLS_SOURCES}
        ${LIBFIZMO_LOCALE_STUBS}
    )

    # libfizmo compile definitions (disable optional features)
    target_compile_definitions(ZorkUI PRIVATE
        USE_FIZMO_BRIDGE=1
        DISABLE_BABEL=1
        DISABLE_FILELIST=1
        DISABLE_CONFIGFILES=1
        DISABLE_COMMAND_HISTORY=1
        DISABLE_OUTPUT_HISTORY=1
        DISABLE_PREFIX_COMMANDS=1
        DISABLE_BLOCKBUFFER=1
    )

    # Force-include our embedded compatibility header to provide declarations
    # that libfizmo's placeholder locale_data.h doesn't have
    target_compile_options(ZorkUI PRIVATE
        -include "${PROJECT_ROOT}/src/fizmo_embedded_compat.h"
    )

    # Threading support
    find_package(Threads REQUIRED)
    target_link_libraries(ZorkUI PRIVATE Threads::Threads)

    # Set story path at compile time (relative to build directory)
    target_compile_definitions(ZorkUI PRIVATE
        ZORK_STORY_PATH="${PROJECT_ROOT}/zork1.z3"
    )

else()
    # Desktop build - use generated entrypoint for testing UI
    qul_add_target(ZorkUI
        FizmoBackend.cpp
        QML_PROJECT "${QML_PROJECT_FILE}"
        SELECTORS "zork"
        GENERATE_ENTRYPOINT
    )

    # Include directories (for header compilation)
    target_include_directories(ZorkUI PRIVATE
        ${PROJECT_ROOT}/src
    )

    # Desktop stub - fizmo functions return dummy values
    target_compile_definitions(ZorkUI PRIVATE
        DESKTOP_STUB=1
    )
endif()

# Display profile compile definition (applies to all build types)
if(DISPLAY_PROFILE STREQUAL "RT1170")
    target_compile_definitions(ZorkUI PRIVATE DISPLAY_RT1170=1)
    message(STATUS "Display profile: RT1170 (720x1280 portrait)")
elseif(DISPLAY_PROFILE STREQUAL "RT1170_SCALED")
    target_compile_definitions(ZorkUI PRIVATE DISPLAY_RT1170_SCALED=1)
    message(STATUS "Display profile: RT1170_SCALED (540x960 portrait, 75% scale)")
else()
    target_compile_definitions(ZorkUI PRIVATE DISPLAY_RT1050=1)
    message(STATUS "Display profile: RT1050 (480x272 landscape)")
endif()

app_target_setup_os(ZorkUI)
