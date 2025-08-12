#include <Arduino.h>

void setup() {
  // Initialize both USB Serial and UART Serial
  Serial.begin(115200);    // USB Serial
  Serial0.begin(115200);   // UART Serial (if available)
  
  // Wait for initialization
  delay(3000);
  
  // Send to both interfaces
  Serial.println("=== ESP32-S3 PowerBoard Test ===");
  Serial.println("Hello World from USB Serial!");
  Serial0.println("=== ESP32-S3 PowerBoard Test ===");
  Serial0.println("Hello World from UART Serial!");
  
  // Print system info
  Serial.print("Chip: ");
  Serial.println(ESP.getChipModel());
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
  
  // PSRAM Debug Information
  Serial.println("=== PSRAM Debug Info ===");
  Serial.print("PSRAM Size: ");
  Serial.print(ESP.getPsramSize() / 1024);
  Serial.println(" KB");
  Serial.print("Free PSRAM: ");
  Serial.print(ESP.getFreePsram() / 1024);
  Serial.println(" KB");
  Serial.print("Used PSRAM: ");
  Serial.print((ESP.getPsramSize() - ESP.getFreePsram()) / 1024);
  Serial.println(" KB");
  Serial.print("PSRAM Available: ");
  Serial.println(ESP.getPsramSize() > 0 ? "YES" : "NO");
  
  if (ESP.getPsramSize() > 0) {
    Serial.println("PSRAM is working correctly!");
    // Test PSRAM allocation
    void* psram_test = ps_malloc(1024);
    if (psram_test != NULL) {
      Serial.println("PSRAM allocation test: SUCCESS");
      free(psram_test);
    } else {
      Serial.println("PSRAM allocation test: FAILED");
    }
  } else {
    Serial.println("WARNING: PSRAM not detected or not working!");
  }
  Serial.println("========================");
  
  Serial0.print("Chip: ");
  Serial0.println(ESP.getChipModel());
  Serial0.print("Free heap: ");
  Serial0.println(ESP.getFreeHeap());
  Serial0.print("PSRAM Size: ");
  Serial0.print(ESP.getPsramSize() / 1024);
  Serial0.println(" KB");
  Serial0.print("PSRAM Available: ");
  Serial0.println(ESP.getPsramSize() > 0 ? "YES" : "NO");
  
  Serial.println("Setup complete!");
  Serial0.println("Setup complete!");
}

void loop() {
  // Send a heartbeat message every 5 seconds to verify communication
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 5000) {
    Serial.println("Heartbeat - System running...");
    lastHeartbeat = millis();
  }
  
  delay(100); // Small delay to prevent overwhelming the serial
}