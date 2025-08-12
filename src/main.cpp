#include <Arduino.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <esp_heap_caps.h>
#include "display_config.h"

// Function declarations
void createUI();
void timer_callback(lv_timer_t *timer);

// Global variables for timer
static lv_obj_t *timer_label = nullptr;
static int timer_seconds = 0;

// Create RGB Panel databus and display using the configuration
Arduino_ESP32RGBPanel *rgbBus = new Arduino_ESP32RGBPanel(
    TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK,
    TFT_R0, TFT_R1, TFT_R2, TFT_R3, TFT_R4,
    TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5,
    TFT_B0, TFT_B1, TFT_B2, TFT_B3, TFT_B4,
    HSYNC_POLARITY, HSYNC_FRONT_PORCH, HSYNC_PULSE_WIDTH, HSYNC_BACK_PORCH,
    VSYNC_POLARITY, VSYNC_FRONT_PORCH, VSYNC_PULSE_WIDTH, VSYNC_BACK_PORCH,
    PCLK_ACTIVE_NEG, PREFER_SPEED, true /* auto_flush */
);
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    PANEL_WIDTH, PANEL_HEIGHT, rgbBus, 0 /* rotation */, true /* auto_flush */
);
;

// LVGL display buffer - allocated dynamically from SPIRAM
static lv_color_t *disp_draw_buf1 = nullptr; // Primary buffer
static lv_color_t *disp_draw_buf2 = nullptr; // Secondary buffer (optional)

lv_display_t *lvgl_display;

// LVGL tick function - essential for LVGL timing
uint32_t my_tick_function(void)
{
  return millis();
}

// LVGL display flush callback for RGB parallel
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  // Reduce debug output frequency to minimize performance impact with larger buffers
  static int flush_count = 0;
  if (flush_count++ % 500 == 0)
  {
    Serial.printf("Flush #%d: x1=%d, y1=%d, x2=%d, y2=%d, w=%d, h=%d\n",
                  flush_count, area->x1, area->y1, area->x2, area->y2, w, h);
  }

  // LVGL v9+ with LV_COLOR_DEPTH 24 uses RGB888 format (3 bytes per pixel)
  // Convert RGB888 to RGB565 for the RGB parallel interface
  uint32_t pixel_count = w * h;

  // Use MALLOC_CAP_DMA for better performance with larger buffers
  uint16_t *rgb565_buffer = (uint16_t *)heap_caps_malloc(pixel_count * sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

  if (!rgb565_buffer)
  {
    // Fallback to regular malloc if DMA allocation fails
    rgb565_buffer = (uint16_t *)malloc(pixel_count * sizeof(uint16_t));
  }

  if (!rgb565_buffer)
  {
    Serial.printf("ERROR: Failed to allocate RGB565 buffer for %d pixels!\n", pixel_count);
    lv_display_flush_ready(disp);
    return;
  }

  uint8_t *rgb888 = color_p;
  // Optimized conversion loop for better performance with larger buffers
  for (uint32_t i = 0; i < pixel_count; i++)
  {
    // Convert RGB888 to RGB565 with high precision for font rendering
    // Use standard RGB order for correct color representation
    uint8_t r = rgb888[i * 3 + 0]; // Red
    uint8_t g = rgb888[i * 3 + 1]; // Green
    uint8_t b = rgb888[i * 3 + 2]; // Blue

    // High-precision conversion for maximum clarity
    // Use rounding for best color accuracy with larger display areas
    uint16_t r5 = (r * 31 + 127) / 255; // Precise 5-bit conversion
    uint16_t g6 = (g * 63 + 127) / 255; // Precise 6-bit conversion
    uint16_t b5 = (b * 31 + 127) / 255; // Precise 5-bit conversion

    rgb565_buffer[i] = (r5 << 11) | (g6 << 5) | b5;
  }

  // Send to display with larger buffer chunks
  gfx->draw16bitRGBBitmap(area->x1, area->y1, rgb565_buffer, w, h);

  // Free the temporary buffer efficiently
  if (heap_caps_get_allocated_size(rgb565_buffer) > 0)
  {
    heap_caps_free(rgb565_buffer);
  }
  else
  {
    free(rgb565_buffer);
  }

  // Tell LVGL that flushing is done
  lv_display_flush_ready(disp);
}

// Timer callback function - called every second
void timer_callback(lv_timer_t *timer)
{
  timer_seconds++;

  // Update the timer label text
  if (timer_label)
  {
    char time_str[32];
    int hours = timer_seconds / 3600;
    int minutes = (timer_seconds % 3600) / 60;
    int seconds = timer_seconds % 60;

    sprintf(time_str, "Timer: %02d:%02d:%02d", hours, minutes, seconds);

    // Use LVGL's efficient text update - only invalidates the label area
    lv_label_set_text(timer_label, time_str);

    // Optional: Force only the label area to be redrawn to minimize flickering
    lv_obj_invalidate(timer_label);
  }
}

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

  // Allocate LVGL buffers from SPIRAM - with double buffering for smooth updates
  size_t buffer_size = LVGL_BUFFER_SIZE * sizeof(lv_color_t);
  disp_draw_buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!disp_draw_buf1)
  {
    Serial.println("Failed to allocate display buffer 1 from SPIRAM, trying internal RAM...");
    disp_draw_buf1 = (lv_color_t *)malloc(buffer_size);
  }

  // Allocate second buffer for double buffering to reduce flickering
  disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!disp_draw_buf2)
  {
    Serial.println("Failed to allocate display buffer 2 from SPIRAM, trying internal RAM...");
    disp_draw_buf2 = (lv_color_t *)malloc(buffer_size);
  }

  if (!disp_draw_buf1)
  {
    Serial.println("CRITICAL: Failed to allocate LVGL display buffer 1!");
    while (1)
      ;
  }
  Serial.printf("LVGL buffer 1 allocated: %d bytes\n", buffer_size);
  if (disp_draw_buf2)
  {
    Serial.printf("LVGL buffer 2 allocated: %d bytes (double buffering enabled)\n", buffer_size);
  }
  else
  {
    Serial.println("LVGL buffer 2 allocation failed - using single buffer");
  }
  Serial.printf("LVGL_BUFFER_SIZE: %d pixels\n", LVGL_BUFFER_SIZE);
  Serial.printf("LVGL_BUFFER_LINES: %d lines\n", LVGL_BUFFER_LINES);
  Serial.printf("Color depth: %d bytes per pixel\n", sizeof(lv_color_t));

  // Initialize LVGL
  lv_init();

  // Set up LVGL tick function - CRITICAL for LVGL timing!
  lv_tick_set_cb(my_tick_function);

  // Create LVGL display
  lvgl_display = lv_display_create(PANEL_WIDTH, PANEL_HEIGHT);
  lv_display_set_flush_cb(lvgl_display, my_disp_flush);
  lv_display_set_buffers(lvgl_display, disp_draw_buf1, disp_draw_buf2, LVGL_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

  Serial.println("LVGL initialized with RGB parallel display driver!");

  // Create UI
  createUI();

  Serial.println("Setup complete! Enjoying 40MHz RGB parallel performance!");
}

void loop()
{
  // Handle LVGL tasks - reduced delay for smoother updates
  lv_timer_handler();
  delay(200); // Reduced from 5ms to 200ms for smoother timer updates
}

void createUI()
{
  Serial.println("Creating simple UI...");

  // Get the active screen
  lv_obj_t *scr = lv_screen_active();

  // Set background to white
  lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);

  // Create a simple label with larger font for better clarity
  lv_obj_t *label = lv_label_create(scr);
  lv_label_set_text(label, "Hello LVGL v9!\nESP32-S3 PowerBoard\nRGB Parallel Display");
  lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_18, LV_PART_MAIN); // Use larger font
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 30);

  // Create timer label with larger font
  timer_label = lv_label_create(scr);
  lv_label_set_text(timer_label, "Timer: 00:00:00");
  lv_obj_set_style_text_color(timer_label, lv_color_hex(0xFF0000), LV_PART_MAIN); // Red color
  lv_obj_set_style_text_align(timer_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(timer_label, &lv_font_montserrat_20, LV_PART_MAIN); // Use larger font for timer
  lv_obj_align(timer_label, LV_ALIGN_CENTER, 0, -50);

  // Create LVGL timer that calls timer_callback every 1000ms (1 second)
  lv_timer_t *timer = lv_timer_create(timer_callback, 1000, NULL);

  // Add a simple colored rectangle to test colors
  lv_obj_t *rect = lv_obj_create(scr);
  lv_obj_set_size(rect, 200, 60);
  lv_obj_align(rect, LV_ALIGN_CENTER, 0, 100);
  lv_obj_set_style_bg_color(rect, lv_color_hex(0x0000FF), LV_PART_MAIN); // Blue
  lv_obj_set_style_border_width(rect, 0, LV_PART_MAIN);

  // Add text to the rectangle with larger font
  lv_obj_t *rect_label = lv_label_create(rect);
  lv_label_set_text(rect_label, "Blue Rectangle");
  lv_obj_set_style_text_color(rect_label, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_text_font(rect_label, &lv_font_montserrat_16, LV_PART_MAIN); // Use larger font
  lv_obj_center(rect_label);

  Serial.println("Simple UI created successfully with timer and improved fonts!");
}
