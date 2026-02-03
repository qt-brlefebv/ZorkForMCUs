#!/bin/bash
# build-flash-rt1170.sh
# Cross-compilation build and flash script for NXP RT1170-EVKB with FreeRTOS
#
# Prerequisites:
#   - ARM GCC toolchain installed
#   - NXP MCUXpresso SDK for RT1170-EVKB installed
#   - Qt for MCUs 2.12.0+ installed
#   - RT1170-EVKB connected via debug probe
#
# Usage:
#   mkdir -p build-rt1170 && cd build-rt1170
#   bash ../build-flash-rt1170.sh

set -e  # Exit on error

# =============================================================================
# Path Detection
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/build-common.sh"

# Detect paths automatically
QUL_ROOT="$(detect_qul_root)"
TOOLCHAIN_DIR="$(detect_arm_toolchain)"
BOARD_SDK_DIR="$(detect_rt1170_sdk)"

[[ -z "${QUL_ROOT}" ]] && die "Qt for MCUs not found" \
    "Set QUL_ROOT environment variable or install Qt for MCUs"

[[ -z "${TOOLCHAIN_DIR}" ]] && die "ARM GCC toolchain not found" \
    "Set TOOLCHAIN_DIR environment variable or install via Qt Maintenance Tool"

[[ -z "${BOARD_SDK_DIR}" ]] && die "NXP RT1170 SDK not found" \
    "Set BOARD_SDK_DIR environment variable or install via Qt Maintenance Tool"

# =============================================================================
# Configuration
# =============================================================================

# MCUXpresso IDE (optional - only needed for some debug/flash operations)
MCUXPRESSO_IDE_PATH="${MCUXPRESSO_IDE_PATH:-}"

# Display profile: RT1170 (720x1280 portrait)
DISPLAY_PROFILE="${DISPLAY_PROFILE:-RT1170}"

# Build type: MinSizeRel for smallest binary, Debug for debugging
BUILD_TYPE="${BUILD_TYPE:-MinSizeRel}"

# =============================================================================
# Show Configuration
# =============================================================================

echo "=== RT1170 FreeRTOS Build Configuration ==="
echo "QUL Root:        ${QUL_ROOT}"
echo "Toolchain:       ${TOOLCHAIN_DIR}"
echo "Board SDK:       ${BOARD_SDK_DIR}"
echo "MCUXpresso:      ${MCUXPRESSO_IDE_PATH:-(not set, flash disabled)}"
echo "Display Profile: ${DISPLAY_PROFILE}"
echo "Build Type:      ${BUILD_TYPE}"
echo ""

# =============================================================================
# CMake Configuration
# =============================================================================

echo "=== Configuring CMake for RT1170 ==="

# Build cmake command with optional parameters
CMAKE_ARGS=(
    -GNinja
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    -DCMAKE_PREFIX_PATH="${QUL_ROOT}"
    -DCMAKE_TOOLCHAIN_FILE="${QUL_ROOT}/lib/cmake/Qul/toolchain/armgcc.cmake"
    -DQUL_PLATFORM=mimxrt1170-evkb-freertos
    -DQUL_TARGET_TOOLCHAIN_DIR="${TOOLCHAIN_DIR}"
    -DQUL_BOARD_SDK_DIR="${BOARD_SDK_DIR}"
    -DQUL_COLOR_DEPTH=32
    -DFREERTOS_DIR="${BOARD_SDK_DIR}/rtos/freertos/freertos-kernel"
    -DBUILD_WITH_FREERTOS=ON
    -DBUILD_WITH_FIZMO=OFF
    -DDISPLAY_PROFILE="${DISPLAY_PROFILE}"
    -DQul_DIR="${QUL_ROOT}/lib/cmake/Qul"
)

# Add MCUXpresso IDE path if set (optional)
if [[ -n "${MCUXPRESSO_IDE_PATH}" ]]; then
    CMAKE_ARGS+=(-DMCUXPRESSO_IDE_PATH="${MCUXPRESSO_IDE_PATH}")
fi

cmake "${CMAKE_ARGS[@]}" ..

if [[ $? -ne 0 ]]; then
    echo ""
    echo "ERROR: CMake configuration failed!"
    echo ""
    echo "Common issues:"
    echo "  1. Platform not found: Verify mimxrt1170-evkb-freertos is supported"
    echo "     in ${QUL_ROOT}/platform/boards/"
    echo "  2. Toolchain error: Verify ARM GCC 12.3.1 is at ${TOOLCHAIN_DIR}"
    echo "  3. SDK error: Verify SDK is at ${BOARD_SDK_DIR}"
    echo "  4. FreeRTOS missing: Check if FreeRTOS sources are in SDK or Qt kit"
    exit 1
fi

echo ""
echo "=== Configuration successful ==="
echo ""

# =============================================================================
# Build
# =============================================================================

echo "=== Building ZorkUI for RT1170 ==="
ninja

if [[ $? -ne 0 ]]; then
    echo ""
    echo "ERROR: Build failed!"
    echo ""
    echo "Check the error messages above for details."
    echo "Common issues:"
    echo "  1. Missing source files"
    echo "  2. libfizmo compilation errors"
    echo "  3. FreeRTOS API incompatibilities"
    echo "  4. Memory/stack size issues"
    exit 1
fi

echo ""
echo "=== Build successful ==="
echo ""

# =============================================================================
# Flash to Device
# =============================================================================

echo "=== Flashing to RT1170-EVKB ==="
ninja flash_ZorkUI

if [[ $? -ne 0 ]]; then
    echo ""
    echo "ERROR: Flash failed!"
    echo ""
    echo "Common issues:"
    echo "  1. Device not connected or not powered"
    echo "  2. Debug probe not recognized"
    echo "  3. Missing flash programming tool"
    exit 1
fi

echo ""
echo "=== Flash successful ==="
echo ""

# =============================================================================
# Post-Build Information
# =============================================================================

ELF_FILE="${BUILD_TYPE}/ZorkUI.elf"
HEX_FILE="${BUILD_TYPE}/ZorkUI.hex"

# Check if binary exists
if [[ -f "${ELF_FILE}" ]]; then
    BINARY_SIZE=$(stat -c%s "${ELF_FILE}" 2>/dev/null || stat -f%z "${ELF_FILE}" 2>/dev/null)
    BINARY_SIZE_KB=$((BINARY_SIZE / 1024))
    echo "Binary: ${ELF_FILE} (${BINARY_SIZE_KB} KB)"

    # RT1170 has 2MB flash
    if [[ ${BINARY_SIZE_KB} -gt 1800 ]]; then
        echo "WARNING: Binary size is close to flash capacity (2MB)"
    fi

    if [[ -f "${HEX_FILE}" ]]; then
        echo "Hex file: ${HEX_FILE} (ready for flashing)"
    fi
else
    echo "WARNING: ${ELF_FILE} not found - check build output"
fi

# Show embedded story data if nm is available
if command -v arm-none-eabi-nm &> /dev/null && [[ -f "${ELF_FILE}" ]]; then
    echo ""
    echo "=== Embedded Story Data ==="
    arm-none-eabi-nm "${ELF_FILE}" | grep story_data || echo "Story data symbols not found"
fi

# Show memory usage if size is available
if command -v arm-none-eabi-size &> /dev/null && [[ -f "${ELF_FILE}" ]]; then
    echo ""
    echo "=== Memory Usage ==="
    arm-none-eabi-size "${ELF_FILE}"
fi

echo ""
echo "=== Device Ready ==="
echo "Connect a serial terminal to UART (115200 baud) for debug output."
echo ""
