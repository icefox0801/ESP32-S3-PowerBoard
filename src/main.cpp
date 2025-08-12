/*******************************************************************************
 * LVGL Hello World Demo
 * Based on the LvglWidgets example but simplified to show "Hello World"
 * 
 * This demo uses LVGL - Light and Versatile Graphics Library
 * without touch features for the ESP32-S3 PowerBoard
 * 
 * Uses the exact Arduino GFX setup from the working main.cpp
 ******************************************************************************/
#include <Arduino.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include "wifi/wifi_manager.h"

// Backlight pin for ESP32-S3 5-inch display (maps to LCD Pin 2 - LEDA)
#define TFT_BL 2
// Note: LCD Pin 1 (LEDK - LED Backlight Cathode) is connected to GND
// Note: LCD Pin 31 (DISP - Display On/Off) may be hardwired or not connected on this board

// Screen dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

/*******************************************************************************
 * Arduino_GFX Display Configuration (optimized with pin assignments from datasheet)
 ******************************************************************************/
// ESP32-S3 PowerBoard RGB configuration optimized based on LCD pin assignments
// Control signals mapping:
// ESP32 Pin 40 → LCD Pin 34 (DEN - Data Enable)
// ESP32 Pin 41 → LCD Pin 33 (VSYNC - Vertical Sync) 
// ESP32 Pin 39 → LCD Pin 32 (HSYNC - Horizontal Sync)
// ESP32 Pin 42 → LCD Pin 30 (CLK - Clock Signal)
// ESP32 Pin 2  → LCD Pin 2  (LEDA - LED Backlight Anode via TFT_BL)
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
    0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
    0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
    1 /* pclk_active_neg */, 16000000 /* prefer_speed - minimum frequency from datasheet for stability */);

// Create the display object with official pin mapping and minimum frequency
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(SCREEN_WIDTH, SCREEN_HEIGHT, rgbpanel, 0 /* rotation */, true /* auto_flush */);

/*******************************************************************************
 * LVGL Configuration Variables - LVGL v9 API
 ******************************************************************************/
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_display_t *disp;
static lv_color_t *disp_draw_buf;

// UI elements that need to be updated
static lv_obj_t *wifi_status_label;
static lv_obj_t *wifi_ip_label;
static lv_obj_t *tick_count_label;

// Tick counter for display
static uint32_t lvgl_tick_count = 0;
static unsigned long last_tick_display_update = 0;

/*******************************************************************************
 * LVGL Tick Function - Required for LVGL v9
 ******************************************************************************/
uint32_t my_get_millis(void)
{
    return millis();
}

/*******************************************************************************
 * LVGL Display Flush Function - LVGL v9 API
 ******************************************************************************/
void my_disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // Boundary checks
    if (area->x2 >= SCREEN_WIDTH || area->y2 >= SCREEN_HEIGHT) {
        lv_display_flush_ready(disp_drv);
        return;
    }

    // Direct pixel transfer using LVGL v9 API
#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
#endif

    lv_display_flush_ready(disp_drv);
}

/*******************************************************************************
 * Create Hello World UI
 ******************************************************************************/
void create_hello_world_ui()
{
    // Create a screen
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x003a57), LV_PART_MAIN);
    
    // Create main title label
    lv_obj_t *title_label = lv_label_create(scr);
    lv_label_set_text(title_label, "Hello World!");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(title_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, -100);
    
    // Create subtitle label
    lv_obj_t *subtitle_label = lv_label_create(scr);
    lv_label_set_text(subtitle_label, "ESP32-S3 PowerBoard with LVGL");
    lv_obj_set_style_text_font(subtitle_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(subtitle_label, lv_color_hex(0x00ff88), LV_PART_MAIN);
    lv_obj_align(subtitle_label, LV_ALIGN_CENTER, 0, -70);
    
    // Create WiFi status label
    wifi_status_label = lv_label_create(scr);
    lv_label_set_text(wifi_status_label, "WiFi: Disconnected");
    lv_obj_set_style_text_font(wifi_status_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(wifi_status_label, lv_color_hex(0xffaa00), LV_PART_MAIN);
    lv_obj_align(wifi_status_label, LV_ALIGN_CENTER, 0, -30);
    
    // Create WiFi IP label
    wifi_ip_label = lv_label_create(scr);
    lv_label_set_text(wifi_ip_label, "IP: Not Connected");
    lv_obj_set_style_text_font(wifi_ip_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(wifi_ip_label, lv_color_hex(0xcccccc), LV_PART_MAIN);
    lv_obj_align(wifi_ip_label, LV_ALIGN_CENTER, 0, 0);
    
    // Create tick count label
    tick_count_label = lv_label_create(scr);
    lv_label_set_text(tick_count_label, "Ticks: 0");
    lv_obj_set_style_text_font(tick_count_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(tick_count_label, lv_color_hex(0x88ddff), LV_PART_MAIN);
    lv_obj_align(tick_count_label, LV_ALIGN_CENTER, 0, 30);
    
    // Create info label
    lv_obj_t *info_label = lv_label_create(scr);
    lv_label_set_text(info_label, "800x480 RGB Display\nRunning LVGL Graphics Library");
    lv_obj_set_style_text_font(info_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(info_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_label, LV_ALIGN_CENTER, 0, 70);
    
    // Create a simple static arc (no animation to prevent flickering)
    lv_obj_t *arc = lv_arc_create(scr);
    lv_obj_set_size(arc, 80, 80);
    lv_arc_set_rotation(arc, 0);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_angles(arc, 0, 270); // Static 3/4 circle
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x00ff88), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 6, LV_PART_INDICATOR);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   // Remove the knob
    lv_obj_align(arc, LV_ALIGN_CENTER, 0, 140);
    
    // Force a screen refresh
    lv_obj_invalidate(scr);
}

/*******************************************************************************
 * Update Tick Count Display
 ******************************************************************************/
void update_tick_count_display()
{
    if (tick_count_label) {
        char tick_text[32];
        snprintf(tick_text, sizeof(tick_text), "Seconds: %lu", lvgl_tick_count);
        lv_label_set_text(tick_count_label, tick_text);
        lv_obj_invalidate(tick_count_label);
    }
}

/*******************************************************************************
 * Update WiFi Status Display
 ******************************************************************************/
void update_wifi_status_display()
{
    if (wifi_status_label && wifi_ip_label) {
        // Get current status
        WiFiStatus current_status = wifiManager.getStatus();
        
        // Update status label with simple text based on status
        const char* status_text;
        uint32_t status_color;
        uint32_t ip_color;
        
        switch (current_status) {
            case WIFI_CONNECTED:
                status_text = "WiFi: Connected";
                status_color = 0x00ff88;
                ip_color = 0x00ff88;
                break;
            case WIFI_CONNECTING:
                status_text = "WiFi: Connecting...";
                status_color = 0xffaa00;
                ip_color = 0xcccccc;
                break;
            case WIFI_DISCONNECTED:
                status_text = "WiFi: Disconnected";
                status_color = 0xff4444;
                ip_color = 0xcccccc;
                break;
            case WIFI_CONNECTION_FAILED:
                status_text = "WiFi: Failed";
                status_color = 0xff4444;
                ip_color = 0xcccccc;
                break;
            default:
                status_text = "WiFi: Unknown";
                status_color = 0xcccccc;
                ip_color = 0xcccccc;
                break;
        }
        
        lv_label_set_text(wifi_status_label, status_text);
        lv_obj_set_style_text_color(wifi_status_label, lv_color_hex(status_color), LV_PART_MAIN);
        
        // Update IP label
        if (current_status == WIFI_CONNECTED) {
            // Only get IP string when connected to avoid unnecessary WiFi calls
            String ip_text = "IP: " + wifiManager.getLocalIP();
            lv_label_set_text(wifi_ip_label, ip_text.c_str());
        } else {
            lv_label_set_text(wifi_ip_label, "IP: Not Connected");
        }
        lv_obj_set_style_text_color(wifi_ip_label, lv_color_hex(ip_color), LV_PART_MAIN);
    }
}

/*******************************************************************************
 * Setup Function - Using exact Arduino GFX setup from working main.cpp
 ******************************************************************************/
void setup()
{
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=== ESP32-S3 PowerBoard LVGL Hello World Demo ===");

    // Initialize display backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // Initialize display
    gfx->begin();
    gfx->fillScreen(BLACK);

    // Initialize LVGL
    lv_init();
    delay(10);
    
    // Set tick callback - CRITICAL for LVGL v9
    lv_tick_set_cb(my_get_millis);

    // Get screen dimensions
    screenWidth = gfx->width();
    screenHeight = gfx->height();

    // Allocate LVGL draw buffer
    uint32_t buf_size = screenWidth * screenHeight / 50;
    
    disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * buf_size);

    if (!disp_draw_buf)
    {
        // Try even smaller buffer
        buf_size = screenWidth * screenHeight / 60;
        disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * buf_size);
    }
    
    if (!disp_draw_buf)
    {
        Serial.println("LVGL buffer allocation failed!");
        return;
    }
    else
    {
        // Initialize LVGL display using v9 API
        disp = lv_display_create(screenWidth, screenHeight);
        lv_display_set_flush_cb(disp, my_disp_flush);
        lv_display_set_buffers(disp, disp_draw_buf, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
        
        // Create Hello World UI
        create_hello_world_ui();

        // Initialize WiFi
        wifiManager.begin();

        // Initialize tick counter display
        update_tick_count_display();

        Serial.println("Setup complete!");
    }
}

/*******************************************************************************
 * Loop Function
 ******************************************************************************/
void loop()
{
    // Simple tick counter that updates every second
    if (millis() - last_tick_display_update > 1000) {
        lvgl_tick_count++;
        update_tick_count_display();
        last_tick_display_update = millis();
    }
    
    // Call LVGL timer at 5 FPS (200ms intervals)
    static unsigned long last_lvgl_update = 0;
    if (millis() - last_lvgl_update > 200) {
        lv_timer_handler();
        last_lvgl_update = millis();
    }
    
    // Update WiFi manager every second
    static unsigned long last_wifi_update = 0;
    if (millis() - last_wifi_update > 1000) {
        wifiManager.update();
        last_wifi_update = millis();
    }
    
    // Update WiFi status display every 5 seconds
    static unsigned long last_display_check = 0;
    static WiFiStatus last_wifi_status = WIFI_DISCONNECTED;
    static String last_ip = "0.0.0.0";
    
    if (millis() - last_display_check > 5000) {
        WiFiStatus current_status = wifiManager.getStatus();
        String current_ip = wifiManager.getLocalIP();
        
        // Update display when status/IP changes OR on periodic check
        bool status_changed = (current_status != last_wifi_status || current_ip != last_ip);
        
        if (status_changed) {
            update_wifi_status_display();
            last_wifi_status = current_status;
            last_ip = current_ip;
        }
        
        last_display_check = millis();
    }
    
    delay(100);
}
