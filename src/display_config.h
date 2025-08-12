/**
 * @file display_config.h
 * @brief Display configuration for ESP32-S3 PowerBoard 5.0" RGB Parallel Display
 *
 * Pin definitions and display settings based on the official ESP32-S3 PowerBoard
 * RGB parallel interface specifications.
 */

#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include <Arduino_GFX_Library.h>

// =============================================================================
// Display Hardware Configuration
// =============================================================================

// Backlight pin
#define TFT_BL 2

// RGB Panel Pin Configuration (corrected based on HelloWorld.ino)
// Control signals
#define TFT_DE 40    // Data Enable
#define TFT_VSYNC 41 // Vertical Sync
#define TFT_HSYNC 39 // Horizontal Sync
#define TFT_PCLK 42  // Pixel Clock

// RGB Data Lines (5-6-5 bit configuration)
// Red channel (5 bits)
#define TFT_R0 45 // Red bit 0
#define TFT_R1 48 // Red bit 1
#define TFT_R2 47 // Red bit 2
#define TFT_R3 21 // Red bit 3
#define TFT_R4 14 // Red bit 4

// Green channel (6 bits)
#define TFT_G0 5  // Green bit 0
#define TFT_G1 6  // Green bit 1
#define TFT_G2 7  // Green bit 2
#define TFT_G3 15 // Green bit 3
#define TFT_G4 16 // Green bit 4
#define TFT_G5 4  // Green bit 5

// Blue channel (5 bits)
#define TFT_B0 8  // Blue bit 0
#define TFT_B1 3  // Blue bit 1
#define TFT_B2 46 // Blue bit 2
#define TFT_B3 9  // Blue bit 3
#define TFT_B4 1  // Blue bit 4

// =============================================================================
// Display Timing Configuration
// =============================================================================

// Panel resolution
#define PANEL_WIDTH 800
#define PANEL_HEIGHT 480

// Horizontal timing parameters
#define HSYNC_POLARITY 0    // Horizontal sync polarity
#define HSYNC_FRONT_PORCH 8 // Horizontal front porch
#define HSYNC_PULSE_WIDTH 4 // Horizontal sync pulse width
#define HSYNC_BACK_PORCH 8  // Horizontal back porch

// Vertical timing parameters
#define VSYNC_POLARITY 0    // Vertical sync polarity
#define VSYNC_FRONT_PORCH 8 // Vertical front porch
#define VSYNC_PULSE_WIDTH 4 // Vertical sync pulse width
#define VSYNC_BACK_PORCH 8  // Vertical back porch

// Clock configuration
#define PCLK_ACTIVE_NEG 1     // Pixel clock active negative
#define PREFER_SPEED 16000000 // Preferred pixel clock speed (16MHz)

// =============================================================================
// LVGL Configuration
// =============================================================================

// Display buffer configuration - optimized to eliminate screen tearing
#define LVGL_BUFFER_LINES 120    // Increased to 120 lines to minimize tearing with fewer flushes
#define LVGL_DOUBLE_BUFFER false // Single buffer to conserve internal RAM

// Buffer size calculations for 24-bit color (3 bytes per pixel)
#define LVGL_BUFFER_SIZE (PANEL_WIDTH * LVGL_BUFFER_LINES)

#endif // DISPLAY_CONFIG_H
