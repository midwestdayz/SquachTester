#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// 1. DISPLAY DRIVER CHIP SELECTION
// ============================================================================
// Uncomment ONLY ONE line below to match your display controller chip.
#define DRIVER_ILI9341      // Classic 2.8" CYD driver
// #define DRIVER_ST7789       // Used by alternative/newer 2.8" CYD boards

// ============================================================================
// 2. DISPLAY SETUP & INVERSION FIX
// ============================================================================
// If your screen shows inverted colors (e.g. white text instead of black, 
// mismatched colors), UNCOMMENT the line below to swap them back.
#define FIX_INVERTED_COLORS 

// If your landscape mode text and UI appear upside down, UNCOMMENT the line 
// below to rotate the entire display 180 degrees.
// #define FLIP_LANDSCAPE_MODE

// ============================================================================
// 3. USB PORT / BOARD VARIANT SELECTION
// ============================================================================
// Uncomment ONLY ONE line below to set your exact backlight mapping pin.
#define CYD_28_SINGLE_USB   // Standard ESP32-2432S028R (Single Micro-USB)
// #define CYD_28_DUAL_USB     // Variant with Type-C + Micro-USB ports

// ============================================================================
// 4. AIRTAG NRF MODULE PIN CONFIGURATION
// ============================================================================
// Set the CE and CS pin numbers for your AirTag NRF module connection here.
#define AIRTAG_NRF_CE_PIN   4        // Chip Enable (CE) Pin
#define AIRTAG_NRF_CS_PIN   22        // Chip Select (CS / SS) Pin

// ============================================================================
// 5. HARDWARE PIN MAPPING (Automated)
// ============================================================================
// Standard physical pin maps for 2.8" CYD boards.
#define TFT_MISO   12
#define TFT_MOSI   13
#define TFT_SCLK   14
#define TFT_CS     15
#define TFT_DC      2
#define TFT_RST    -1  // Tied to ESP32 Reset

#define TOUCH_SCK  25
#define TOUCH_MOSI 32
#define TOUCH_MISO 39
#define TOUCH_CS   33
#define TOUCH_IRQ  36

#if defined(CYD_28_SINGLE_USB)
    #define TFT_BL 21  // Backlight pin for single USB boards
#elif defined(CYD_28_DUAL_USB)
    #define TFT_BL 22  // Backlight pin for dual USB boards
#endif

#endif // CONFIG_H
