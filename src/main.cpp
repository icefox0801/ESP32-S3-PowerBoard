#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// Backlight pin for ESP32-S3 5-inch display
#define TFT_BL 2

// Manual RGB panel configuration for ESP32-S3 5-inch display
// Based on the working demo configuration
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
  // Initialize USB Serial only
  Serial.begin(115200);    // USB Serial
  
  // Wait for initialization
  delay(3000);
  
  Serial.println("=== ESP32-S3 PowerBoard with Display Test ===");
  
  // Initialize display backlight
  Serial.println("Turning on backlight...");
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Turn on backlight
  
  // Initialize display
  Serial.println("Initializing display...");
  gfx->begin();
  
  // Clear screen and show simple text
  Serial.println("Drawing on display...");
  gfx->fillScreen(BLACK);
  
  // Display simple text
  gfx->setTextColor(WHITE);
  gfx->setTextSize(3);
  gfx->setCursor(50, 100);
  gfx->println("Hello PowerBoard!");
  
  gfx->setTextColor(CYAN);
  gfx->setTextSize(2);
  gfx->setCursor(100, 200);
  gfx->println("Display Working!");
  
  gfx->setTextColor(YELLOW);
  gfx->setTextSize(1);
  gfx->setCursor(150, 300);
  gfx->print("Resolution: ");
  gfx->print(gfx->width());
  gfx->print(" x ");
  gfx->println(gfx->height());
  
  // Draw a simple rectangle
  gfx->drawRect(50, 350, 200, 50, GREEN);
  gfx->fillRect(55, 355, 190, 40, RED);
  
  Serial.println("Display setup complete!");
  
  // Send to Serial interface
  Serial.println("Hello World from USB Serial!");
  
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
  
  Serial.println("Setup complete!");
}

void loop() {
  // Send a heartbeat message every 5 seconds to verify communication
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 5000) {
    Serial.println("Heartbeat - System running...");
    
    // Update display with current uptime
    gfx->fillRect(300, 400, 200, 30, BLACK); // Clear previous text
    gfx->setTextColor(WHITE);
    gfx->setTextSize(1);
    gfx->setCursor(300, 410);
    gfx->print("Uptime: ");
    gfx->print(millis() / 1000);
    gfx->println(" sec");
    
    lastHeartbeat = millis();
  }
  
  delay(100); // Small delay to prevent overwhelming the serial
}