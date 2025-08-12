/*******************************************************************************
 * WiFi Manager Implementation for ESP32-S3 PowerBoard
 ******************************************************************************/

#include "wifi_manager.h"
#include "wifi_config.h" // Contains WiFi credentials

// Global instance
WiFiManager wifiManager;

WiFiManager::WiFiManager()
{
  _ssid = nullptr;
  _password = nullptr;
  _status = WIFI_DISCONNECTED;
  _lastConnectionAttempt = 0;
  _connectionTimeout = 10000; // 10 seconds timeout
  _reconnectAttempts = 0;
}

void WiFiManager::begin()
{
  // Use credentials from wifi_config.h
  begin(WIFI_SSID, WIFI_PASSWORD);
}

void WiFiManager::begin(const char *ssid, const char *password)
{
  _ssid = ssid;
  _password = password;

  Serial.println("=== WiFi Manager Initializing ===");
  Serial.print("SSID: ");
  Serial.println(_ssid);

  // Set WiFi mode to station
  WiFi.mode(WIFI_STA);

  // Disconnect any previous connection
  WiFi.disconnect();
  delay(100);

  // Start connection attempt
  connect();
}

bool WiFiManager::connect()
{
  if (_status == WIFI_CONNECTING)
  {
    return false; // Already trying to connect
  }

  setStatus(WIFI_CONNECTING);
  _lastConnectionAttempt = millis();
  _reconnectAttempts++;

  Serial.print("WiFi connecting to: ");
  Serial.print(_ssid);
  Serial.print(" (attempt ");
  Serial.print(_reconnectAttempts);
  Serial.println(")");

  WiFi.begin(_ssid, _password);

  return true;
}

bool WiFiManager::isConnected()
{
  return WiFi.status() == WL_CONNECTED && _status == WIFI_CONNECTED;
}

void WiFiManager::disconnect()
{
  WiFi.disconnect();
  setStatus(WIFI_DISCONNECTED);
  _reconnectAttempts = 0;
  Serial.println("WiFi disconnected");
}

WiFiStatus WiFiManager::getStatus()
{
  return _status;
}

String WiFiManager::getStatusString()
{
  switch (_status)
  {
  case WIFI_DISCONNECTED:
    return "Disconnected";
  case WIFI_CONNECTING:
    return "Connecting...";
  case WIFI_CONNECTED:
    return "Connected";
  case WIFI_CONNECTION_FAILED:
    return "Connection Failed";
  default:
    return "Unknown";
  }
}

String WiFiManager::getSSID()
{
  if (isConnected())
  {
    return WiFi.SSID();
  }
  return String(_ssid ? _ssid : "N/A");
}

String WiFiManager::getLocalIP()
{
  if (isConnected())
  {
    return WiFi.localIP().toString();
  }
  return "0.0.0.0";
}

int WiFiManager::getRSSI()
{
  if (isConnected())
  {
    return WiFi.RSSI();
  }
  return 0;
}

void WiFiManager::update()
{
  wl_status_t wifiStatus = WiFi.status();

  switch (_status)
  {
  case WIFI_CONNECTING:
    if (wifiStatus == WL_CONNECTED)
    {
      setStatus(WIFI_CONNECTED);
      printConnectionInfo();
    }
    else if (millis() - _lastConnectionAttempt > _connectionTimeout)
    {
      // Connection timeout
      if (_reconnectAttempts >= _maxReconnectAttempts)
      {
        setStatus(WIFI_CONNECTION_FAILED);
        Serial.println("WiFi connection failed after maximum attempts");
      }
      else
      {
        // Try again
        Serial.println("WiFi connection timeout, retrying...");
        connect();
      }
    }
    break;

  case WIFI_CONNECTED:
    if (wifiStatus != WL_CONNECTED)
    {
      Serial.println("WiFi connection lost, attempting to reconnect...");
      setStatus(WIFI_DISCONNECTED);
      _reconnectAttempts = 0;
      connect();
    }
    break;

  case WIFI_CONNECTION_FAILED:
    // Wait a bit before trying again
    if (millis() - _lastConnectionAttempt > 30000)
    { // Wait 30 seconds
      Serial.println("Retrying WiFi connection after failure...");
      _reconnectAttempts = 0;
      connect();
    }
    break;

  case WIFI_DISCONNECTED:
    // Auto-reconnect if we have credentials
    if (_ssid && _password)
    {
      connect();
    }
    break;
  }
}

void WiFiManager::setStatus(WiFiStatus status)
{
  if (_status != status)
  {
    _status = status;
    Serial.print("WiFi status changed to: ");
    Serial.println(getStatusString());
  }
}

void WiFiManager::printConnectionInfo()
{
  Serial.println("=== WiFi Connected Successfully ===");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.println("=====================================");
}
