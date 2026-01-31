# ZorkForMCUs

Play classic Infocom text adventures on NXP microcontrollers using Qt for MCUs.

![Platform](https://img.shields.io/badge/platform-RT1170%20|%20RT1050%20|%20Desktop-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## Supported Platforms

| Platform | Status | Display |
|----------|--------|---------|
| NXP MIMXRT1170-EVKB | Builds | 720x1280 portrait, touch |
| NXP MIMXRT1050-EVK | Tested | 480x272 landscape, touch |
| Desktop Emulator | Tested | Scalable window |

## Requirements

- **Qt for MCUs 2.12.0+** with appropriate platform kit
- **For hardware builds:** ARM GCC toolchain, NXP MCUXpresso SDK
- **Story file:** `zork1.z3` embedded in firmware

## Quick Start

### Using Qt Creator (Recommended)

1. Clone with submodules:
   ```bash
   git clone --recursive https://github.com/qt-brlefebv/ZorkForMCUs.git
   ```

2. Open `ui/qul/ZorkUI/CMakeLists.txt` in Qt Creator

3. Select a Qt for MCUs kit:
   - **Desktop:** Any Qt for MCUs desktop kit
   - **RT1170:** `mimxrt1170-evkb-freertos` kit
   - **RT1050:** `mimxrt1050-evk-freertos` kit

4. Build and run (desktop) or flash (hardware)

### Using Build Scripts

For CI/automation or command-line builds:

```bash
# Desktop
mkdir -p build && cd build
bash ../ui/qul/ZorkUI/build-run.sh

# RT1170
mkdir -p ui/qul/ZorkUI/build-rt1170 && cd ui/qul/ZorkUI/build-rt1170
bash ../build-rt1170.sh
ninja flash_ZorkUI

# RT1050
mkdir -p ui/qul/ZorkUI/build-rt1050 && cd ui/qul/ZorkUI/build-rt1050
bash ../build-rt1050.sh
ninja flash_ZorkUI
```

## Project Structure

```
ZorkForMCUs/
├── ui/qul/ZorkUI/       # Qt for MCUs application
│   ├── CMakeLists.txt   # Main build configuration
│   ├── ZorkUI.qml       # UI definition
│   └── FizmoBackend.*   # Game logic interface
├── src/                 # Platform integration code
├── external/libfizmo/   # Z-machine interpreter (submodule)
└── zork1.z3             # Zork I story file
```

## Configuration

Build mode is auto-detected from your Qt for MCUs kit:
- FreeRTOS platforms -> embedded interpreter
- Desktop platform -> threaded interpreter

To force UI-only stub mode (no game logic):
```bash
cmake -DBUILD_WITH_FIZMO=OFF ...
```

## Troubleshooting

**"libfizmo not found"**
```bash
git submodule update --init --recursive
```

**Build fails with path errors**

Set environment variables for your installation:
```bash
export QUL_ROOT=/path/to/QtMCUs/2.12.0
export TOOLCHAIN_DIR=/path/to/arm_gcc     # Hardware only
export BOARD_SDK_DIR=/path/to/NXP/SDK     # Hardware only
```

**Touch input not working on hardware**

Verify display cable connection and check serial output (115200 baud) for diagnostics.

## License

MIT License - See [LICENSE.txt](LICENSE.txt) file.

## Acknowledgments

- [libfizmo](https://github.com/chrender/libfizmo) - Z-machine interpreter by Christoph Ender
- [Qt for MCUs](https://www.qt.io/product/qt-for-mcus) - UI framework by Qt Company
