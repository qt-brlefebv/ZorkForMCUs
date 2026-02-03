#!/bin/bash
# Desktop build script for Qt for MCUs emulator
#
# Usage: mkdir build-temp && cd build-temp && bash ../ui/qul/ZorkUI/claude-build-run.sh

set -e  # Exit on error

# =============================================================================
# Path Detection
# =============================================================================

# Source directory (auto-detect from script location)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="${SOURCE_DIR:-${SCRIPT_DIR}}"

# Source common detection functions
source "${SCRIPT_DIR}/build-common.sh"

# Detect paths automatically
QUL_ROOT="$(detect_qul_root)"
MINGW_DIR="$(detect_mingw_dir)"

[[ -z "${QUL_ROOT}" ]] && die "Qt for MCUs not found" \
    "Set QUL_ROOT environment variable or install Qt for MCUs"

[[ -z "${MINGW_DIR}" ]] && die "MinGW not found" \
    "Set MINGW_DIR environment variable or install MinGW via Qt Maintenance Tool"

# Display profile for desktop emulator
DISPLAY_PROFILE="${DISPLAY_PROFILE:-RT1170_SCALED}"

# Build type
BUILD_TYPE="${BUILD_TYPE:-Debug}"

# Build with fizmo interpreter (ON or OFF)
BUILD_WITH_FIZMO="${BUILD_WITH_FIZMO:-ON}"

# =============================================================================
# Helper Functions
# =============================================================================

prepend_path() {
    if [[ ":$PATH:" != *":$1:"* ]]; then
        PATH="$1:$PATH"
    fi
}

# =============================================================================
# Validate Environment
# =============================================================================

# Prevent running in source directory
if [[ -f "CMakeLists.txt" ]]; then
    die "CMakeLists.txt detected in current directory" \
        "Run this script from a build folder, not the source directory.

Example:
  mkdir -p build-temp && cd build-temp
  bash ../ui/qul/ZorkUI/claude-build-run.sh"
fi

# =============================================================================
# Configure CMake
# =============================================================================

prepend_path "${QUL_ROOT}/lib/gnu"
prepend_path "${MINGW_DIR}/bin"

echo "=== Desktop Build Configuration ==="
echo "Qt for MCUs:     ${QUL_ROOT}"
echo "MinGW:           ${MINGW_DIR}"
echo "Source dir:      ${SOURCE_DIR}"
echo "Display profile: ${DISPLAY_PROFILE}"
echo "Build type:      ${BUILD_TYPE}"
echo "With fizmo:      ${BUILD_WITH_FIZMO}"
echo ""

# Configure if not already configured
if [[ ! -f "build.ninja" ]]; then
    echo "=== Configuring CMake ==="

    cmake "${SOURCE_DIR}" \
        -GNinja \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
        -DCMAKE_C_COMPILER="${MINGW_DIR}/bin/gcc.exe" \
        -DCMAKE_C_OUTPUT_EXTENSION=".obj" \
        -DCMAKE_CXX_COMPILER="${MINGW_DIR}/bin/g++.exe" \
        -DCMAKE_ASM_COMPILER="${MINGW_DIR}/bin/gcc.exe" \
        -DCMAKE_RC_COMPILER="${MINGW_DIR}/bin/windres.exe" \
        -DBUILD_WITH_FIZMO="${BUILD_WITH_FIZMO}" \
        -DBUILD_WITH_FREERTOS="OFF" \
        -DDISPLAY_PROFILE="${DISPLAY_PROFILE}" \
        -DQul_ROOT="${QUL_ROOT}" \
        -DQUL_PLATFORM_BOARDS_DIR="${QUL_ROOT}/platform/boards" \
        -DQUL_GENERATORS="${QUL_ROOT}/lib/cmake/Qul/QulGenerators.cmake" \
        -DQUL_PLATFORM="qt" \
        -DQUL_OS="baremetal" \
        -DAPP_COMMON_DIR="${QUL_ROOT}/src/app_common" \
        -DQUL_COMPILER_NAME="gnu" \
        -DQUL_COLOR_DEPTH="32"

    if [[ $? -ne 0 ]]; then
        echo ""
        echo "ERROR: CMake configuration failed!"
        exit 1
    fi
fi

# =============================================================================
# Build
# =============================================================================

echo "=== Building ==="
cmake --build . || exit 1

# =============================================================================
# Run
# =============================================================================

echo ""
echo "=== Running ZorkUI ==="
PATH="${QUL_ROOT}/lib/gnu:$PATH" \
QT_QPA_PLATFORM_PLUGIN_PATH="${QUL_ROOT}/lib/gnu" \
./ZorkUI.exe
