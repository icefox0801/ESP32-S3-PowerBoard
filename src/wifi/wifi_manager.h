/*******************************************************************************
 * WiFi Manager for ESP32-S3 PowerBoard
 *
 * This module handles WiFi connection and provides status information
 * WiFi credentials are stored in wifi_config.h (not committed to git)
 ******************************************************************************/

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <Arduino.h>

// WiFi connection status
enum WiFiStatus
{
  WIFI_DISCONNECTED = 0,
  WIFI_CONNECTING = 1,
  WIFI_CONNECTED = 2,
  WIFI_CONNECTION_FAILED = 3
};

class WiFiManager
{
public:
  WiFiManager();

  // Initialize and connect to WiFi
  void begin();
  void begin(const char *ssid, const char *password);

  // Connection management
  bool connect();
  bool isConnected();
  void disconnect();

  // Status information
  WiFiStatus getStatus();
  String getStatusString();
  String getSSID();
  String getLocalIP();
  int getRSSI();

  // Update loop - call this regularly to maintain connection
  void update();

private:
  const char *_ssid;
  const char *_password;
  WiFiStatus _status;
  unsigned long _lastConnectionAttempt;
  unsigned long _connectionTimeout;
  int _reconnectAttempts;
  const int _maxReconnectAttempts = 5;

  void setStatus(WiFiStatus status);
  void printConnectionInfo();
};

// Global WiFi manager instance
extern WiFiManager wifiManager;

#endif // WIFI_MANAGER_H
