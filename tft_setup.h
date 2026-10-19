#pragma once

// Pull in the centralized hardware configuration selected by the user
#include "config.h"

// =========================================================================
// AUTOMATED DRIVER TRANSLATION LAYER
// =========================================================================
// This matches user selections in config.h and exports TFT_eSPI macros

#if defined(DRIVER_ST7789)
  #define ST7789_DRIVER 1
  #define TFT_RGB_ORDER TFT_BGR
#elif defined(DRIVER_ILI9341)
  #define ILI9341_DRIVER 1
  #define TFT_RGB_ORDER TFT_RGB
#endif

// Standard 2.8" panel geometric layout specifications
#define TFT_WIDTH   240
#define TFT_HEIGHT  320

// Hardware performance timing maps
#define SPI_FREQUENCY         40000000
#define SPI_READ_FREQUENCY    20000000

// Core graphic fonts configurations 
#define LOAD_GLCD
#define LOAD_FONT2

// =========================================================================
// COLOR INVERSION PASSTHROUGH FIX
// =========================================================================
// If the panel defaults are inverted, this handles configuration at compile-time.
#if defined(FIX_INVERTED_COLORS)
  #define TFT_INVERSION_ON
#else
  #define TFT_INVERSION_OFF
#endif
