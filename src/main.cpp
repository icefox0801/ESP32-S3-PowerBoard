#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <esp_heap_caps.h>
#include "display_config.h"


// Create RGB Panel databus and display with auto_flush enabled for stable sync
Arduino_ESP32RGBPanel *rgbBus = new Arduino_ESP32RGBPanel(
    TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK,
    TFT_R0, TFT_R1, TFT_R2, TFT_R3, TFT_R4,
    TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5,
    TFT_B0, TFT_B1, TFT_B2, TFT_B3, TFT_B4,
    HSYNC_POLARITY, HSYNC_FRONT_PORCH, HSYNC_PULSE_WIDTH, HSYNC_BACK_PORCH,
    VSYNC_POLARITY, VSYNC_FRONT_PORCH, VSYNC_PULSE_WIDTH, VSYNC_BACK_PORCH,
    PCLK_ACTIVE_NEG, PREFER_SPEED, true /* auto_flush enabled for stable horizontal sync */
);
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    PANEL_WIDTH, PANEL_HEIGHT, rgbBus, 0 /* rotation */, true /* auto_flush enabled */
);

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting ESP32-S3 PowerBoard with RGB Parallel Display...");

  // Initialize backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Turn on backlight

  // Initialize RGB parallel display
  if (!gfx->begin())
  {
    Serial.println("Failed to initialize RGB parallel display!");
    return;
  }

  Serial.println("RGB parallel display initialized successfully!");
  Serial.printf("Display: %dx%d RGB parallel @ %d MHz\n", PANEL_WIDTH, PANEL_HEIGHT, PREFER_SPEED / 1000000);

  // Test display with a simple fill
  gfx->fillScreen(BLACK);
  delay(500);
  gfx->fillScreen(RED);
  delay(500);
  gfx->fillScreen(GREEN);
  delay(500);
  gfx->fillScreen(BLUE);
  delay(500);
  gfx->fillScreen(BLACK);

  Serial.println("Display test complete!");

  // Draw initial timer value
  gfx->setTextColor(WHITE, BLACK);
  gfx->setTextSize(3);
  gfx->setCursor(100, 100);
  gfx->print("Timer: 00:00:00");
  Serial.println("Setup complete! Timer started!");
}


unsigned long lastUpdate = 0;
unsigned long startMillis = 0;
int lastSeconds = -1;

void loop()
{
  if (startMillis == 0) startMillis = millis();
  unsigned long elapsed = (millis() - startMillis) / 1000;
  if ((int)elapsed != lastSeconds) {
    lastSeconds = (int)elapsed;
    int hours = elapsed / 3600;
    int minutes = (elapsed % 3600) / 60;
    int seconds = elapsed % 60;
    char buf[32];
    sprintf(buf, "Timer: %02d:%02d:%02d", hours, minutes, seconds);
    // Clear previous timer area
    gfx->fillRect(100, 100, 300, 40, BLACK);
    gfx->setTextColor(WHITE, BLACK);
    gfx->setTextSize(3);
    gfx->setCursor(100, 100);
    gfx->print(buf);
  }
  delay(10);
}
