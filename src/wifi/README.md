# WiFi Module

This folder contains all WiFi-related components for the ESP32-S3 PowerBoard project using a header-based configuration system that keeps your credentials safe and out of version control.

## Files

- **wifi_manager.h/cpp** - WiFi connection management class
- **wifi_config_template.h** - Template for WiFi configuration (committed to git)
- **wifi_config.h** - Your actual WiFi credentials (ignored by git)

## Quick Setup

1. **Copy the template file:**
   ```bash
   cp src/wifi/wifi_config_template.h src/wifi/wifi_config.h
   ```

2. **Edit your WiFi credentials:**
   Open `src/wifi/wifi_config.h` and replace the placeholders:
   ```c
   #define WIFI_SSID "YOUR_ACTUAL_WIFI_NAME"
   #define WIFI_PASSWORD "YOUR_ACTUAL_PASSWORD"
   ```

3. **Build and upload firmware:**
   ```bash
   pio run -t upload
   ```

## File Structure

- `src/wifi/wifi_config_template.h` - Template file with examples (committed to git)
- `src/wifi/wifi_config.h` - Your actual credentials (ignored by git)
- `.gitignore` - Ensures wifi_config.h is never committed

## Security Features

✅ **Credentials are never committed to git**
✅ **Template shows proper structure**
✅ **Clear separation between template and actual config**
✅ **Simple C preprocessor defines**
✅ **Configurable timeouts and display settings**

## Configuration Format

```c
// WiFi Credentials
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Connection settings  
#define WIFI_TIMEOUT_MS 10000
#define WIFI_RETRY_ATTEMPTS 3

// Backup WiFi credentials (optional)
#define WIFI_BACKUP_SSID "BACKUP_WIFI"
#define WIFI_BACKUP_PASSWORD "BACKUP_PASSWORD"

// Display settings
#define WIFI_SHOW_STATUS true
#define WIFI_UPDATE_INTERVAL_MS 2000
```

## Usage in Code

```cpp
#include "wifi/wifi_manager.h"

void setup() {
    wifiManager.begin();  // Loads wifi_config.yaml automatically
}

void loop() {
    wifiManager.update(); // Maintain connection
    
    // Check connection status
    if (wifiManager.isConnected()) {
        Serial.println("WiFi connected: " + wifiManager.getLocalIP());
    }
}
```

## WiFi Manager API

### Basic Methods
- `begin()` - Load config from YAML and connect
- `begin(ssid, password)` - Connect with custom credentials
- `isConnected()` - Check connection status
- `disconnect()` - Disconnect from WiFi
- `update()` - Call in loop to maintain connection

### Status Information
- `getStatus()` - Get WiFiStatus enum
- `getStatusString()` - Get human-readable status
- `getSSID()` - Get connected network name
- `getLocalIP()` - Get assigned IP address
- `getRSSI()` - Get signal strength

## Troubleshooting

- **Config file not found**: Ensure `src/wifi/wifi_config.h` exists by copying the template
- **WiFi not connecting**: Check your SSID and password in `wifi_config.h`
- **File committed by accident**: Add `wifi_config.h` to `.gitignore` and remove from git history
- **Build errors**: Verify the include path `wifi/wifi_manager.h` is correct

## Advanced Features

The WiFi manager includes:
- Automatic reconnection on connection loss
- Configurable timeouts and retry attempts
- Signal strength monitoring
- Connection status reporting
- Support for backup WiFi credentials
