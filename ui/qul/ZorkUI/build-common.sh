#!/bin/bash
# build-common.sh - Shared path detection functions for build scripts
#
# This file is sourced by the individual build scripts to provide
# cross-platform path detection for Qt for MCUs toolchains and SDKs.

# Detect Qt for MCUs installation
detect_qul_root() {
    if [[ -n "${QUL_ROOT}" && -d "${QUL_ROOT}" ]]; then
        echo "${QUL_ROOT}"
        return
    fi

    # Search for any 2.x version in common locations
    local search_paths=("/c/Qt/QtMCUs" "C:/Qt/QtMCUs" "$HOME/Qt/QtMCUs" "/opt/Qt/QtMCUs")
    for base in "${search_paths[@]}"; do
        if [[ -d "$base" ]]; then
            local found=$(ls -d "$base"/2.* 2>/dev/null | sort -V | tail -1)
            if [[ -n "$found" ]]; then
                echo "$found"
                return
            fi
        fi
    done

    echo ""
}

# Detect MinGW for desktop builds
detect_mingw_dir() {
    if [[ -n "${MINGW_DIR}" && -d "${MINGW_DIR}" ]]; then
        echo "${MINGW_DIR}"
        return
    fi

    local search_paths=("/c/Qt/Tools" "C:/Qt/Tools" "$HOME/Qt/Tools")
    for base in "${search_paths[@]}"; do
        if [[ -d "$base" ]]; then
            local found=$(ls -d "$base"/mingw*_64 2>/dev/null | sort -V | tail -1)
            if [[ -n "$found" ]]; then
                echo "$found"
                return
            fi
        fi
    done

    echo ""
}

# Detect ARM GCC toolchain
detect_arm_toolchain() {
    if [[ -n "${TOOLCHAIN_DIR}" && -d "${TOOLCHAIN_DIR}" ]]; then
        echo "${TOOLCHAIN_DIR}"
        return
    fi

    local search_paths=("/c/Qt/Tools/QtMCUs" "C:/Qt/Tools/QtMCUs" "$HOME/Qt/Tools/QtMCUs" "/opt/Qt/Tools/QtMCUs")
    for base in "${search_paths[@]}"; do
        if [[ -d "$base" ]]; then
            local found=$(ls -d "$base"/arm_gcc_* 2>/dev/null | sort -V | tail -1)
            if [[ -n "$found" ]]; then
                echo "$found"
                return
            fi
        fi
    done

    # Check system PATH as fallback
    if command -v arm-none-eabi-gcc &>/dev/null; then
        local gcc_path=$(which arm-none-eabi-gcc)
        echo "$(dirname "$(dirname "$gcc_path")")"
        return
    fi

    echo ""
}

# Detect NXP SDK for RT1050
detect_rt1050_sdk() {
    if [[ -n "${BOARD_SDK_DIR}" && -d "${BOARD_SDK_DIR}" ]]; then
        echo "${BOARD_SDK_DIR}"
        return
    fi

    local search_paths=("/c/Qt/Tools/QtMCUs/NXP" "C:/Qt/Tools/QtMCUs/NXP" "$HOME/Qt/Tools/QtMCUs/NXP" "/opt/Qt/Tools/QtMCUs/NXP")
    for base in "${search_paths[@]}"; do
        if [[ -d "$base" ]]; then
            local found=$(ls -d "$base"/SDK_*IMXRT1050* 2>/dev/null | sort -V | tail -1)
            if [[ -n "$found" ]]; then
                echo "$found"
                return
            fi
        fi
    done

    echo ""
}

# Detect NXP SDK for RT1170
detect_rt1170_sdk() {
    if [[ -n "${BOARD_SDK_DIR}" && -d "${BOARD_SDK_DIR}" ]]; then
        echo "${BOARD_SDK_DIR}"
        return
    fi

    local search_paths=("/c/Qt/Tools/QtMCUs/NXP" "C:/Qt/Tools/QtMCUs/NXP" "$HOME/Qt/Tools/QtMCUs/NXP" "/opt/Qt/Tools/QtMCUs/NXP")
    for base in "${search_paths[@]}"; do
        if [[ -d "$base" ]]; then
            local found=$(ls -d "$base"/SDK_*IMXRT1170* 2>/dev/null | sort -V | tail -1)
            if [[ -n "$found" ]]; then
                echo "$found"
                return
            fi
        fi
    done

    echo ""
}

# Print error and exit
die() {
    echo "ERROR: $1" >&2
    echo "" >&2
    if [[ -n "$2" ]]; then
        echo "$2" >&2
    fi
    exit 1
}
