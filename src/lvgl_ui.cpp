#include "lvgl_ui.h"
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <esp_heap_caps.h>
#include "display_config.h"

static lv_obj_t *timer_label = nullptr;
static int timer_seconds = 0;

// Store the display pointer for flush callback
static Arduino_RGB_Display *s_gfx = nullptr;

// LVGL display buffer - allocated dynamically from SPIRAM
static lv_color_t *disp_draw_buf1 = nullptr; // Primary buffer
static lv_color_t *disp_draw_buf2 = nullptr; // Secondary buffer (optional)
lv_display_t *lvgl_display;

// LVGL tick function - essential for LVGL timing
static uint32_t my_tick_function(void)
{
  return millis();
}

// Updated flush callback for LVGL v9: no user_data argument, get user data from display
void gfx_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p)
{
  Arduino_RGB_Display *gfx = static_cast<Arduino_RGB_Display *>(lv_display_get_user_data(disp));
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  uint16_t *rgb565_data = (uint16_t *)color_p;
#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, rgb565_data, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, rgb565_data, w, h);
#endif
  lv_display_flush_ready(disp);
}

void lvgl_setup(Arduino_RGB_Display *gfx)
{
  s_gfx = gfx;
  // Allocate LVGL buffers for smooth rendering - use larger buffer for RGB parallel
  size_t buffer_size = LVGL_BUFFER_SIZE * sizeof(lv_color_t);
  disp_draw_buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!disp_draw_buf1)
    disp_draw_buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!disp_draw_buf2)
    disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!disp_draw_buf1)
  {
    Serial.println("CRITICAL: Failed to allocate LVGL display buffer 1!");
    while (1)
      ;
  }
  Serial.printf("LVGL buffer 1 allocated: %d bytes\n", buffer_size);
  if (disp_draw_buf2)
    Serial.printf("LVGL buffer 2 allocated: %d bytes (double buffering enabled)\n", buffer_size);
  else
    Serial.println("LVGL buffer 2 allocation failed - using single buffer (may cause flickering)");
  Serial.printf("LVGL_BUFFER_SIZE: %d pixels\n", LVGL_BUFFER_SIZE);
  Serial.printf("LVGL_BUFFER_LINES: %d lines\n", LVGL_BUFFER_LINES);
  Serial.printf("Color depth: %d bytes per pixel\n", sizeof(lv_color_t));

  lv_init();
  lv_tick_set_cb(my_tick_function);
  lvgl_display = lv_display_create(PANEL_WIDTH, PANEL_HEIGHT);
  lv_display_set_flush_cb(lvgl_display, gfx_disp_flush);
  lv_display_set_user_data(lvgl_display, gfx);
  lv_display_set_buffers(lvgl_display, disp_draw_buf1, disp_draw_buf2, LVGL_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);
  Serial.println("LVGL initialized with RGB parallel display driver (PARTIAL render mode for RGB parallel stability)!");
}

static void timer_callback(lv_timer_t *timer)
{
  timer_seconds++;
  if (timer_label)
  {
    char time_str[32];
    int hours = timer_seconds / 3600;
    int minutes = (timer_seconds % 3600) / 60;
    int seconds = timer_seconds % 60;
    int milliseconds = (timer_seconds % 1) * 1000;
    sprintf(time_str, "Timer: %02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
    lv_label_set_text(timer_label, time_str);
  }
}

void createUI()
{
  lv_obj_t *scr = lv_screen_active();
  lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);
  lv_obj_t *label = lv_label_create(scr);
  lv_label_set_text(label, "Hello LVGL v9!\nESP32-S3 PowerBoard\nRGB Parallel Display");
  lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_24, LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 30);
  timer_label = lv_label_create(scr);
  lv_label_set_text(timer_label, "Timer: 00:00:00");
  lv_obj_set_style_text_color(timer_label, lv_color_hex(0xFF0000), LV_PART_MAIN);
  lv_obj_set_style_text_align(timer_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_font(timer_label, &lv_font_montserrat_28, LV_PART_MAIN);
  lv_obj_align(timer_label, LV_ALIGN_CENTER, 0, -50);
  lv_timer_t *timer = lv_timer_create(timer_callback, 1000, NULL);
  lv_obj_t *rect = lv_obj_create(scr);
  lv_obj_set_size(rect, 250, 70);
  lv_obj_align(rect, LV_ALIGN_CENTER, 0, 100);
  lv_obj_set_style_bg_color(rect, lv_color_hex(0x0000FF), LV_PART_MAIN);
  lv_obj_set_style_border_width(rect, 0, LV_PART_MAIN);
  lv_obj_t *rect_label = lv_label_create(rect);
  lv_label_set_text(rect_label, "Blue Rectangle");
  lv_obj_set_style_text_color(rect_label, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_text_font(rect_label, &lv_font_montserrat_22, LV_PART_MAIN);
  lv_obj_center(rect_label);
}

void lvgl_ui_loop()
{
  lv_timer_handler();
}
