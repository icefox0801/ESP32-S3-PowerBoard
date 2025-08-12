# CLAUDE.md - Development Guidelines

## Commit Message Convention

Your task is to help the user to generate a commit message and commit the changes using git.

### Guidelines

- DO NOT add any ads such as "Generated with [Claude Code](https://claude.ai/code)"
- Only generate the message for staged files/changes
- Don't add any files using `git add`. The user will decide what to add. 
- Follow the rules below for the commit message.

### Format

```
<type>:<space><message title>

<bullet points summarizing what was updated>
```

### Example Titles

```
feat(auth): add JWT login flow
fix(ui): handle null pointer in sidebar
refactor(api): split user controller logic
docs(readme): add usage section
```

### Example with Title and Body

```
feat(auth): add JWT login flow

- Implemented JWT token validation logic
- Added documentation for the validation component
```

### Rules

* title is lowercase, no period at the end.
* Title should be a clear summary, max 50 characters.
* Use the body (optional) to explain *why*, not just *what*.
* Bullet points should be concise and high-level.

### Avoid

* Vague titles like: "update", "fix stuff"
* Overly long or unfocused titles
* Excessive detail in bullet points

### Allowed Types

| Type     | Description                           |
| -------- | ------------------------------------- |
| feat     | New feature                           |
| fix      | Bug fix                               |
| chore    | Maintenance (e.g., tooling, deps)     |
| docs     | Documentation changes                 |
| refactor | Code restructure (no behavior change) |
| test     | Adding or refactoring tests           |
| style    | Code formatting (no logic change)     |
| perf     | Performance improvements              |

## Development Workflow

### Setup
```bash
git clone <repository>
cd PowerBoard
```

### Build & Upload
```bash
pio run --target upload --upload-port COM3
```

### Monitor
```bash
pio device monitor --port COM3 --baud 115200
```

### Pre-commit Checklist
- [ ] Code compiles without errors
- [ ] Serial output tested on hardware
- [ ] PSRAM functionality verified (if applicable)
- [ ] Commit message follows convention
- [ ] Documentation updated (if needed)

## Project Structure

```
PowerBoard/
├── src/
│   └── main.cpp          # Main application code
├── include/              # Header files
├── lib/                  # Local libraries
├── test/                 # Test files
├── platformio.ini        # PlatformIO configuration
├── .gitignore           # Git ignore rules
└── CLAUDE.md            # This file
```

## Hardware Configuration

### ESP32-8048S050 Development Board
This project uses the ESP32-8048S050 5.0" development board with integrated display and touch capabilities.

#### Key Features
- **MCU**: ESP32-S3 with dual-core processor
- **Display**: 5.0" TFT LCD with 800x480 resolution
- **Touch**: Capacitive touch screen (GT911)
- **PSRAM**: 8MB external PSRAM
- **Flash**: 16MB external flash memory
- **Audio**: Built-in audio amplifier (AX98357A)
- **Connectivity**: WiFi, Bluetooth
- **Power**: USB-C power input

#### Current Configuration
- **Board**: ESP32-S3 DevKit (esp32-s3-devkitc-1)
- **PSRAM**: Enabled and functional
- **USB**: CDC on boot enabled
- **Serial**: Dual output (USB + UART)
- **Port**: COM3 @ 115200 baud
- **Status**: Serial communication working, PSRAM detected

#### Development Resources
- Complete documentation available in `5.0inch_ESP32-8048S050/` folder
- Specifications, schematics, user manual, and example code included
- Flash download tools and development utilities provided

## Critical Display Configuration

### Arduino GFX Library Setup
**IMPORTANT**: The ESP32-S3 5-inch display requires specific Arduino GFX library configuration.

#### Required Library Version
```ini
lib_deps = 
    moononournation/GFX Library for Arduino @ ^1.4.7
```

#### Display Initialization Code
```cpp
#include <Arduino_GFX_Library.h>

// Backlight pin for ESP32-S3 5-inch display
#define TFT_BL 2

// Manual RGB panel configuration for ESP32-S3 5-inch display
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
    0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 43 /* hsync_back_porch */,
    0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 12 /* vsync_back_porch */,
    1 /* pclk_active_neg */, 16000000 /* prefer_speed */);

// Create the display object
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(800 /* width */, 480 /* height */, rgbpanel);

void setup() {
    // Initialize display backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH); // Turn on backlight
    
    // Initialize display
    gfx->begin();
    gfx->fillScreen(BLACK);
    
    // Your display code here...
}
```

#### Key Points
1. **RGB Parallel Interface**: This display uses RGB parallel, NOT SPI
2. **Pin Mapping**: All RGB pins must be correctly mapped as shown above
3. **Library Version**: Version 1.4.7+ is required for proper RGB panel support
4. **Resolution**: 800x480 pixels
5. **Backlight**: Pin 2 controls the backlight (active HIGH)

#### Common Issues
- **Black screen with backlight**: Incorrect pin mapping or library version
- **Compilation errors**: Wrong Arduino GFX library version
- **No backlight**: Pin 2 not configured as output
- **Garbled display**: Incorrect timing parameters

#### Serial Configuration for Debug
```ini
# platformio.ini
build_flags = 
    -DARDUINO_USB_CDC_ON_BOOT=0
    -DARDUINO_USB_MODE=0
monitor_speed = 115200
monitor_dtr = 1
monitor_rts = 1
```
