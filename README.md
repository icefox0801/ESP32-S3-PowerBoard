# ESP32-S3 PowerBoard Project

A PlatformIO project for the ESP32-S3 PowerBoard with 5.0" TFT display development board.

## Hardware Overview

### ESP32-8048S050 Development Board
- **MCU**: ESP32-S3 dual-core processor
- **Display**: 5.0" TFT LCD with 800x480 resolution (RGB parallel interface)
- **Touch**: Capacitive touch screen (GT911)
- **Memory**: 8MB PSRAM + 16MB Flash
- **Audio**: Built-in amplifier (AX98357A)
- **Connectivity**: WiFi, Bluetooth
- **Power**: USB-C input

## Quick Start

### Prerequisites
- [PlatformIO](https://platformio.org/) installed in VS Code
- ESP32-S3 PowerBoard connected via USB-C
- Windows driver for COM port communication

### Setup
```bash
git clone <repository-url>
cd PowerBoard
```

### Build & Upload
```bash
pio run --target upload --upload-port COM3
```

### Monitor Serial Output
```bash
pio device monitor --port COM3 --baud 115200
```

## Features

### Current Implementation
- ✅ **Display Integration**: Full RGB parallel display support
- ✅ **Serial Communication**: USB Serial debugging
- ✅ **PSRAM Support**: 8MB external PSRAM detection and testing
- ✅ **Graphics Demo**: Text rendering and basic shapes
- ✅ **System Monitoring**: Real-time uptime display

### Display Capabilities
- 800x480 pixel resolution
- RGB parallel interface (NOT SPI)
- Hardware backlight control
- Multiple text sizes and colors
- Basic graphics primitives

## Project Structure

```
PowerBoard/
├── src/
│   └── main.cpp              # Main application code
├── include/                  # Header files
├── lib/                      # Local libraries  
├── test/                     # Test files
├── .vscode/                  # VS Code configuration
├── platformio.ini            # PlatformIO configuration
├── CLAUDE.md                 # Development guidelines
└── README.md                 # This file
```

## Display Configuration

### Critical Setup Information

The ESP32-S3 5-inch display requires specific configuration:

```cpp
#include <Arduino_GFX_Library.h>

// Backlight control
#define TFT_BL 2

// RGB parallel panel configuration  
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
    0, 8, 4, 43, 0, 8, 4, 12, 1, 16000000);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(800, 480, rgbpanel);
```

### Library Dependencies

```ini
lib_deps = 
    moononournation/GFX Library for Arduino @ ^1.4.7
```

⚠️ **Important**: Version 1.4.7+ is required for proper RGB panel support.

## Development

### Building
Use PlatformIO's build system:
- **Command Line**: `pio run`
- **VS Code**: Ctrl+Shift+P → "Tasks: Run Task" → "PlatformIO Build"

### Debugging
- Serial output via USB at 115200 baud
- PSRAM status and system information displayed
- Real-time uptime monitoring on display

### Configuration
- **Board**: `esp32-s3-devkitc-1`
- **Framework**: Arduino
- **Upload Port**: COM3 (Windows)
- **Monitor Speed**: 115200

## Troubleshooting

### Display Issues
| Problem | Solution |
|---------|----------|
| Black screen with backlight | Check pin mapping and library version |
| Compilation errors | Ensure Arduino GFX library @ 1.4.7+ |
| No backlight | Verify pin 2 is configured as output |
| Garbled display | Check timing parameters |

### Serial Issues
| Problem | Solution |
|---------|----------|
| No serial output | Check USB CDC settings in platformio.ini |
| Connection issues | Verify COM port and baud rate |
| Upload failures | Check upload port configuration |

## Documentation

- **Development Guidelines**: See [CLAUDE.md](CLAUDE.md)
- **Hardware Documentation**: Available in `5.0inch_ESP32-8048S050/` folder
- **Library Reference**: [Arduino GFX Library](https://github.com/moononournation/Arduino_GFX)

## Contributing

1. Follow the commit message convention in [CLAUDE.md](CLAUDE.md)
2. Test on hardware before committing
3. Update documentation for new features
4. Verify PSRAM functionality when applicable

## License

This project is for development and educational purposes.

## Hardware Resources

Complete documentation, schematics, and example code available in the `5.0inch_ESP32-8048S050/` directory.
