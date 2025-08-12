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

// Backlight pin for ESP32-S3 5-inch display
#define TFT_BL 2

// Screen dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

/*******************************************************************************
 * Arduino_GFX Display Configuration (from working main.cpp)
 ******************************************************************************/
// Manual RGB panel configuration for ESP32-S3 5-inch display
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
    0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
    0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
    1 /* pclk_active_neg */, 16000000 /* prefer_speed */);

// Create the display object
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(SCREEN_WIDTH, SCREEN_HEIGHT, rgbpanel);

/*******************************************************************************
 * LVGL Configuration Variables
 ******************************************************************************/
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_display_t *disp;
static lv_color_t *disp_draw_buf;

// UI elements that need to be updated
static lv_obj_t *wifi_status_label;
static lv_obj_t *wifi_ip_label;

/*******************************************************************************
 * LVGL Display Flush Function - Updated for LVGL v9
 ******************************************************************************/
void my_disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // Debug output every few flushes to track activity
    static int flush_count = 0;
    flush_count++;
    if (flush_count % 100 == 0) {  // Every 100 flushes
        Serial.print("LVGL Flush #");
        Serial.print(flush_count);
        Serial.print(": ");
        Serial.print(w);
        Serial.print("x");
        Serial.println(h);
    }

    // Boundary checks
    if (area->x2 >= SCREEN_WIDTH || area->y2 >= SCREEN_HEIGHT) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    // Direct pixel transfer without additional processing
#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
#endif

    lv_disp_flush_ready(disp_drv);
}

/*******************************************************************************
 * Create Hello World UI
 ******************************************************************************/
void create_hello_world_ui()
{
    Serial.println("Creating Hello World UI...");
    
    // Create a screen
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x003a57), LV_PART_MAIN);
    Serial.println("Screen background set");
    
    // Create main title label
    lv_obj_t *title_label = lv_label_create(scr);
    lv_label_set_text(title_label, "Hello World!");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(title_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, -100);
    Serial.println("Title label created");
    
    // Create subtitle label
    lv_obj_t *subtitle_label = lv_label_create(scr);
    lv_label_set_text(subtitle_label, "ESP32-S3 PowerBoard with LVGL");
    lv_obj_set_style_text_font(subtitle_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(subtitle_label, lv_color_hex(0x00ff88), LV_PART_MAIN);
    lv_obj_align(subtitle_label, LV_ALIGN_CENTER, 0, -70);
    Serial.println("Subtitle label created");
    
    // Create WiFi status label
    wifi_status_label = lv_label_create(scr);
    lv_label_set_text(wifi_status_label, "WiFi: Disconnected");
    lv_obj_set_style_text_font(wifi_status_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(wifi_status_label, lv_color_hex(0xffaa00), LV_PART_MAIN);
    lv_obj_align(wifi_status_label, LV_ALIGN_CENTER, 0, -30);
    Serial.println("WiFi status label created");
    
    // Create WiFi IP label
    wifi_ip_label = lv_label_create(scr);
    lv_label_set_text(wifi_ip_label, "IP: Not Connected");
    lv_obj_set_style_text_font(wifi_ip_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(wifi_ip_label, lv_color_hex(0xcccccc), LV_PART_MAIN);
    lv_obj_align(wifi_ip_label, LV_ALIGN_CENTER, 0, 0);
    Serial.println("WiFi IP label created");
    
    // Create info label
    lv_obj_t *info_label = lv_label_create(scr);
    lv_label_set_text(info_label, "800x480 RGB Display\nRunning LVGL Graphics Library");
    lv_obj_set_style_text_font(info_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(info_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(info_label, LV_ALIGN_CENTER, 0, 50);
    Serial.println("Info label created");
    
    // Create a simple static arc (no animation to prevent flickering)
    lv_obj_t *arc = lv_arc_create(scr);
    lv_obj_set_size(arc, 80, 80);
    lv_arc_set_rotation(arc, 0);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_angles(arc, 0, 270); // Static 3/4 circle
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x00ff88), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 6, LV_PART_INDICATOR);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   // Remove the knob
    lv_obj_align(arc, LV_ALIGN_CENTER, 0, 120);
    Serial.println("Static arc created");
    
    // Force a screen refresh
    lv_obj_invalidate(scr);
    
    Serial.println("Hello World UI created successfully!");
}

/*******************************************************************************
 * Update WiFi Status Display - Optimized to reduce string operations
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
    Serial.println("Initializing display...");

    // Initialize display backlight (exact same as working main.cpp)
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // Initialize display (exact same as working main.cpp)
    gfx->begin();
    gfx->fillScreen(BLACK);

    // Test display with color fills (like in LvglWidgets example)
    gfx->fillScreen(RED);
    delay(500);
    gfx->fillScreen(GREEN);
    delay(500);
    gfx->fillScreen(BLUE);
    delay(500);
    gfx->fillScreen(BLACK);
    delay(500);

    // Initialize LVGL
    lv_init();
    delay(10);

    // Get screen dimensions
    screenWidth = gfx->width();
    screenHeight = gfx->height();

    Serial.print("Screen size: ");
    Serial.print(screenWidth);
    Serial.print("x");
    Serial.println(screenHeight);

    // Allocate LVGL draw buffer - simplified allocation with LV_MEM_CUSTOM 0
    // Using 1/20th of screen size to minimize memory usage and reduce display updates
    uint32_t buf_size = screenWidth * screenHeight / 20;
    
    disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * buf_size);

    if (!disp_draw_buf)
    {
        Serial.print("LVGL buffer allocation failed! Tried to allocate: ");
        Serial.print(buf_size * sizeof(lv_color_t));
        Serial.println(" bytes");
        
        // Try even smaller buffer
        buf_size = screenWidth * screenHeight / 30;
        Serial.print("Trying smaller buffer: ");
        Serial.print(buf_size * sizeof(lv_color_t));
        Serial.println(" bytes");
        
        disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * buf_size);
    }
    
    if (!disp_draw_buf)
    {
        Serial.println("LVGL buffer allocation completely failed!");
        return;
    }
    else
    {
        Serial.print("LVGL buffer allocated successfully: ");
        Serial.print(buf_size * sizeof(lv_color_t));
        Serial.println(" bytes");
        
        // Initialize LVGL display for v9
        disp = lv_display_create(screenWidth, screenHeight);
        lv_display_set_flush_cb(disp, my_disp_flush);
        lv_display_set_buffers(disp, disp_draw_buf, NULL, buf_size * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);

        // Create Hello World UI
        create_hello_world_ui();

        // Initialize WiFi
        Serial.println("Initializing WiFi...");
        wifiManager.begin();

        Serial.println("LVGL Hello World Demo Setup Complete!");
    }
}

/*******************************************************************************
 * Loop Function
 ******************************************************************************/
void loop()
{
    // Always call LVGL timer first for smooth UI updates
    lv_timer_handler();
    
    // Update WiFi manager (check every 500ms to reduce overhead)
    static unsigned long last_wifi_update = 0;
    if (millis() - last_wifi_update > 500) {
        wifiManager.update();
        last_wifi_update = millis();
    }
    
    // Update WiFi status display
    static unsigned long last_display_update = 0;
    static WiFiStatus last_wifi_status = WIFI_DISCONNECTED;
    static String last_ip = "0.0.0.0";
    
    WiFiStatus current_status = wifiManager.getStatus();
    String current_ip = wifiManager.getLocalIP();
    
    // Update display when status/IP changes OR every 2 seconds for periodic refresh
    bool status_changed = (current_status != last_wifi_status || current_ip != last_ip);
    bool time_to_refresh = (millis() - last_display_update > 2000);
    
    if (status_changed || time_to_refresh) {
        update_wifi_status_display();
        last_display_update = millis();
        last_wifi_status = current_status;
        last_ip = current_ip;
        
        // Debug info
        Serial.print("Display updated - Status: ");
        Serial.print(wifiManager.getStatusString());
        Serial.print(" IP: ");
        Serial.println(current_ip);
    }
    
    delay(5); // Reduced delay for more responsive UI
}
