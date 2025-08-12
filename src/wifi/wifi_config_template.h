#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

// WiFi Configuration Template
// 
// INSTRUCTIONS:
// 1. Copy this file to 'wifi_config.h' in the same directory
// 2. Edit 'wifi_config.h' with your actual WiFi credentials
// 3. The 'wifi_config.h' file is ignored by git for security
//
// This template shows all available configuration options

// WiFi Credentials - REPLACE WITH YOUR ACTUAL VALUES
#define WIFI_SSID "YOUR_WIFI_NAME"           // Replace with your WiFi network name
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"   // Replace with your WiFi password

// Connection settings
#define WIFI_TIMEOUT_MS 10000                // WiFi connection timeout (default: 10 seconds)
#define WIFI_RETRY_ATTEMPTS 3                // Number of connection retry attempts

// Backup WiFi credentials (optional - useful for mobile hotspot fallback)
#define WIFI_BACKUP_SSID "BACKUP_WIFI_NAME"       // Optional backup WiFi network
#define WIFI_BACKUP_PASSWORD "BACKUP_PASSWORD"    // Optional backup WiFi password

// Display settings
#define WIFI_SHOW_STATUS true                     // Show WiFi connection status on display
#define WIFI_UPDATE_INTERVAL_MS 2000              // How often to update display (milliseconds)

// SECURITY NOTE:
// Never commit the actual 'wifi_config.h' file to version control!
// Only this template file should be tracked by git.

#endif // WIFI_CONFIG_H
