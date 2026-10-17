#pragma once

// =========================================================================
// UNCOMMENT ONLY ONE TO MATCH YOUR TARGET BOARD BATCH
// =========================================================================
#define BATCH_ST7789
// #define BATCH_ILI9341

#ifdef BATCH_ST7789
  #define ST7789_DRIVER 1
  #define TFT_RGB_ORDER TFT_BGR
#else
  #define ILI9341_DRIVER 1
  #define TFT_RGB_ORDER TFT_RGB
#endif

#define TFT_WIDTH   240
#define TFT_HEIGHT  320

#define TFT_MISO  12
#define TFT_MOSI  13
#define TFT_SCLK  14
#define TFT_CS    15
#define TFT_DC     2
#define TFT_RST   -1
#define TFT_BL    21
#define TFT_BACKLIGHT_ON   1

#define SPI_FREQUENCY         40000000
#define SPI_READ_FREQUENCY    20000000

#define LOAD_GLCD
#define LOAD_FONT2
#define TFT_INVERSION_OFF