#define CYD 1
#define SERIAL_BAUD 2000000

#define CONFIG_BT_NIMBLE_ROLE_CENTRAL_DISABLED 1
#define CONFIG_BT_NIMBLE_ROLE_PERIPHERAL_DISABLED 1
#define CONFIG_BT_NIMBLE_MAX_CONNECTIONS 1
#define CONFIG_BT_NIMBLE_MAX_BONDS 1

#define TFT_BL    21
#define PWM_FREQ  5000

#include "cyd_includes.h"

#include "theme.h"
#include "ui.h"
#include "emulate.h"

#define TOUCH_SCK  25
#define TOUCH_MOSI 32
#define TOUCH_MISO 39
#define TOUCH_CS   33
#define TOUCH_IRQ  36

TFT_eSPI tft;
SPIClass touchSpi(VSPI);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

static uint16_t RAW_X_MIN = 200, RAW_X_MAX = 3800;
static uint16_t RAW_Y_MIN = 200, RAW_Y_MAX = 3800;
static uint32_t s_lastTap = 0;

static void setBacklight(uint8_t duty) {
    #if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(TFT_BL, duty);
    #else
    ledcWrite(0, duty);
    #endif
}

static bool mapTouch(int16_t rawX, int16_t rawY, int& sx, int& sy) {
    const int w = tft.width();
    const int h = tft.height();
    if (RAW_X_MAX <= RAW_X_MIN || RAW_Y_MAX <= RAW_Y_MIN) return false;
    sx = (int)((int32_t)(rawX - RAW_X_MIN) * w / (RAW_X_MAX - RAW_X_MIN));
    sy = (int)((int32_t)(rawY - RAW_Y_MIN) * h / (RAW_Y_MAX - RAW_Y_MIN));
    if (sx < 0) sx = 0;
    if (sy < 0) sy = 0;
    if (sx >= w) sx = w - 1;
    if (sy >= h) sy = h - 1;
    return true;
}

static void pollTouch() {
    if (!touch.tirqTouched() || !touch.touched()) return;
    const uint32_t now = millis();
    if (now - s_lastTap < 250) return;
    TS_Point p = touch.getPoint();
    int sx, sy;
    if (!mapTouch(p.x, p.y, sx, sy)) return;
    s_lastTap = now;
    uiHandleTap(tft, sx, sy);
    uiDraw(tft);
}

void setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println("--- SquachTester — Console Active ---");

    #if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(TFT_BL, PWM_FREQ, 8);
    #else
    ledcSetup(0, PWM_FREQ, 8);
    ledcAttachPin(TFT_BL, 0);
    #endif
    setBacklight(255);

    tft.init();

    #ifdef USE_ST7789_BATCH
    tft.setRotation(1);
    tft.setSwapBytes(false);
    tft.invertDisplay(false); 
    #else
    tft.setRotation(1);  
    tft.invertDisplay(true);
    #endif
    
    tft.fillScreen(TFT_BLACK);
    tft.fillScreen(Theme::BG);

    touchSpi.begin(TOUCH_SCK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
    touch.begin(touchSpi);
    touch.setRotation(1);

    Emulate::begin();
    uiInit(tft);
    uiDraw(tft);
}

// Explicit timestamp placeholder tracker allocation
unsigned long globalLastScanTime = 0;

void loop() {
    pollTouch();

    if (Emulate::active()) {
        Emulate::update();
    }

    // Standard drawing handler call rules apply cleanly now
    if (uiScreen() != Screen::SCAN_LIST && uiScreen() != Screen::SCAN_SSID_LIST && uiScreen() != Screen::CONFIG) {
        // Base menu grid drawings continue to apply smoothly
    } else {
        uiDraw(tft);
    }
    
    delay(10); 
}
