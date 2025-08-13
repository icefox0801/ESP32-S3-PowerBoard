#pragma once
#include <lvgl.h>
#include <Arduino_GFX_Library.h>

void lvgl_setup(Arduino_RGB_Display *gfx);
void createUI();
void lvgl_ui_loop();
void gfx_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p);
