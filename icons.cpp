#include "icons.h"
#include "theme.h"
#include "ui.h"

namespace {

    uint16_t SHELL, SHELL_HI, SHELL_LO, ICO_INK, ICO_GLASS, ICO_LENS, ICO_METAL,
    ICO_METAL2, ICO_WARN, ICO_LED, ICO_APPLE, ICO_APPLE_HI,
    ICO_APPLE_LO, ICO_STEM, ICO_LEAF, ICO_SHEEN, ICO_PLASTIC;
    bool s_ready = false;

    void icoPalette() {
        if (s_ready) return;
        SHELL       = 0x6B70; SHELL_HI    = 0x9D38;
        SHELL_LO    = 0x39EA; ICO_INK     = 0x18C4;
        ICO_GLASS   = 0x2350; ICO_LENS    = 0x069B;
        ICO_METAL   = 0x94B4; ICO_METAL2  = 0xD6BC;
        ICO_WARN    = 0xF9E5; ICO_LED     = 0xFEE7;
        ICO_APPLE   = 0xE165; ICO_APPLE_HI= 0xFBEC;
        ICO_APPLE_LO= 0x90A3; ICO_STEM    = 0x7AA5;
        ICO_LEAF    = 0x3E0A; ICO_SHEEN   = 0xFFFF;
        ICO_PLASTIC = 0xEF3A;
        s_ready = true;
    }

    void housing(TFT_eSPI& t, int x, int y, int w, int h) {
        t.fillRect(x, y, w, h, SHELL);
        t.fillRect(x, y, w, (h / 6) ? h / 6 : 1, SHELL_HI);
        t.fillRect(x, y + h - ((h / 8) ? h / 8 : 1), w, (h / 8) ? h / 8 : 1, SHELL_LO);
    }

    void lens(TFT_eSPI& t, int cx, int cy, int r) {
        t.fillCircle(cx, cy, r, ICO_INK);
        t.fillCircle(cx, cy, (r * 2) / 3, ICO_GLASS);
        t.fillCircle(cx - r / 3, cy - r / 3, (r / 4) ? r / 4 : 1, ICO_SHEEN);
    }

}

void drawTypeIcon(TFT_eSPI& t, DeviceType type, int cx, int cy, int s) {
    icoPalette();
    const uint16_t BG = Theme::BG;
    bool s_altMode = uiGetAltMode();

    switch (type) {
        case DeviceType::FLOCK:
        t.fillRect(cx - 3, cy - s / 4, 6, s * 5 / 4, SHELL_LO);
        t.fillRect(cx - 3, cy - s / 4, 2, s * 5 / 4, SHELL);
        housing(t, cx - s, cy - s * 3 / 4, s * 2, s);
        lens(t, cx - s / 2, cy - s / 4, s / 3);
        for (int i = 0; i < 3; i++) t.fillCircle(cx + s / 4 + i * s / 4, cy - s / 4, s / 10, ICO_WARN);
        t.fillRect(cx - s - 2, cy - s * 3 / 4 - s / 3, s * 2 + 4, s / 4, ICO_METAL);
        t.fillRect(cx - s - 2, cy - s * 3 / 4 - s / 3, s * 2 + 4, s / 12, ICO_METAL2);
        break;
        case DeviceType::AXON:
        housing(t, cx - s * 2 / 3, cy - s * 3 / 4, s * 4 / 3, s * 3 / 2);
        lens(t, cx, cy - s / 4, s / 2);
        t.fillRect(cx - s / 3, cy + s / 3, s * 2 / 3, s / 6, ICO_INK);
        t.fillCircle(cx + s / 3, cy + s * 2 / 3, s / 8, ICO_WARN);
        t.fillRect(cx - s / 2, cy - s * 3 / 4 - s / 4, s, s / 4, ICO_METAL);
        t.fillRect(cx - s / 2, cy - s * 3 / 4 - s / 4, s, s / 12, ICO_METAL2);
        break;
        case DeviceType::META:
        t.fillRect(cx - s * 3 / 2, cy - s / 2, s * 3, s / 4, SHELL_HI);
        t.fillRect(cx - s * 3 / 2, cy - s / 2, s * 3, s / 12, ICO_SHEEN);
        t.fillRect(cx - s * 3 / 2, cy - s / 4, s * 5 / 4, s * 5 / 9, SHELL);
        t.fillRect(cx + s / 4, cy - s / 4, s * 5 / 4, s * 5 / 9, SHELL);
        t.fillRect(cx - s * 3 / 2 + 2, cy - s / 4 + 2, s * 5 / 4 - 4, s * 5 / 9 - 4, ICO_INK);
        t.fillRect(cx + s / 4 + 2, cy - s / 4 + 2, s * 5 / 4 - 4, s * 5 / 9 - 4, ICO_INK);
        t.drawLine(cx - s * 5 / 4, cy + s / 6, cx - s * 3 / 4, cy - s / 8, SHELL_HI);
        t.drawLine(cx - s, cy + s / 6, cx - s * 2 / 3, cy, SHELL_HI);
        t.drawLine(cx + s / 2, cy + s / 6, cx + s, cy - s / 8, SHELL_HI);
        t.drawLine(cx + s * 3 / 4, cy + s / 6, cx + s * 13 / 12, cy, SHELL_HI);
        t.fillRect(cx - s / 4, cy - s / 4, s / 2, s / 5, SHELL_HI);
        t.fillRect(cx - s * 7 / 4, cy - s / 2, s / 3, s / 5, SHELL);
        t.fillRect(cx + s * 3 / 2 - 2, cy - s / 2, s / 3, s / 5, SHELL);
        t.fillCircle(cx - s * 3 / 2 + s / 5, cy, s / 7, ICO_WARN);
        t.fillCircle(cx - s * 3 / 2 + s / 5, cy, s / 14, ICO_LED);
        break;
        case DeviceType::SKIMMER:
        t.fillRect(cx - s / 2, cy - s * 5 / 4, s * 3 / 2, s * 2 / 3, ICO_LED);
        t.fillRect(cx - s / 2, cy - s * 5 / 4, s * 3 / 2, s / 8, ICO_SHEEN);
        t.fillRect(cx - s / 2, cy - s * 5 / 4 + s / 3, s * 3 / 2, s / 6, ICO_INK);
        housing(t, cx - s, cy - s / 2, s * 2, s);
        t.fillRect(cx - s + 4, cy - s / 4, s * 2 - 8, s / 4, ICO_INK);
        break;
        case DeviceType::RAVEN:
        t.fillTriangle(cx + s / 2, cy + s / 4, cx + s * 7 / 5, cy + s, cx + s / 3, cy + s * 4 / 5, SHELL_LO);
        t.fillEllipse(cx + s / 6, cy + s / 4, s * 7 / 10, s * 3 / 5, SHELL);
        t.fillEllipse(cx + s / 6, cy + s / 8, s * 7 / 10, s * 2 / 5, SHELL_HI);
        t.fillEllipse(cx + s / 4, cy + s / 3, s * 2 / 5, s * 2 / 5, SHELL_LO);
        t.fillCircle(cx - s / 2, cy - s / 3, s / 2, SHELL);
        t.fillCircle(cx - s / 2, cy - s / 2, s / 3, SHELL_HI);
        t.fillTriangle(cx - s * 9 / 10, cy - s * 2 / 5, cx - s * 8 / 5, cy - s / 5, cx - s * 9 / 10, cy, SHELL_LO);
        t.fillTriangle(cx - s * 9 / 10, cy - s * 2 / 5, cx - s * 8 / 5, cy - s / 5, cx - s * 9 / 10, cy - s / 5, SHELL);
        t.fillTriangle(cx - s / 2, cy, cx + s / 8, cy + s / 3, cx - s * 3 / 5, cy + s / 3, SHELL_LO);
        t.fillCircle(cx - s * 3 / 5, cy - s * 2 / 5, s / 8, ICO_SHEEN);
        t.fillCircle(cx - s * 3 / 5, cy - s * 2 / 5, s / 16, ICO_INK);
        t.fillRect(cx, cy + s * 3 / 4, 3, s / 3, SHELL_LO);
        t.fillRect(cx + s / 3, cy + s * 3 / 4, 3, s / 3, SHELL_LO);
        break;
        case DeviceType::AIRTAG:
        t.fillCircle(cx - s / 3, cy + s / 6, s, ICO_APPLE);
        t.fillCircle(cx + s / 3, cy + s / 6, s, ICO_APPLE);
        t.fillRect(cx - s / 3, cy - s * 2 / 3, s * 2 / 3, s, ICO_APPLE);
        t.fillCircle(cx + s / 2, cy + s / 2, s / 2, ICO_APPLE_LO);
        t.fillCircle(cx - s / 2, cy - s / 6, s / 3, ICO_APPLE_HI);
        t.fillCircle(cx - s * 7 / 12, cy - s / 4, s / 8, ICO_SHEEN);
        t.fillCircle(cx + s * 11 / 12, cy - s / 3, s / 2, BG);
        t.fillCircle(cx + s / 2, cy - s * 5 / 6, s / 6, BG);
        t.fillCircle(cx + s * 7 / 6, cy + s / 12, s / 6, BG);
        t.fillRect(cx - 2, cy - s - s / 3, 4, s / 2, ICO_STEM);
        t.fillTriangle(cx + 2, cy - s - s / 6, cx + s, cy - s - s / 2, cx + s / 2, cy - s + 2, ICO_LEAF);
        break;
        case DeviceType::DRONE:
        for (int k = 0; k < 4; k++) {
            const int dx = (k & 1) ? s : -s, dy = (k & 2) ? s : -s;
            t.drawLine(cx, cy, cx + dx, cy + dy, SHELL_LO);
            t.fillEllipse(cx + dx, cy + dy, s / 2, s / 6, ICO_METAL2);
            t.fillCircle(cx + dx, cy + dy, s / 8, SHELL);
        }
        t.fillEllipse(cx, cy, s * 2 / 3, s / 2, SHELL);
        t.fillEllipse(cx - s / 5, cy - s / 6, s / 4, s / 6, SHELL_HI);
        lens(t, cx, cy + s / 3, s / 4);
        break;
        case DeviceType::ALPR:
        housing(t, cx - s, cy - s, s * 2, s * 3 / 4);
        lens(t, cx - s / 2, cy - s * 5 / 8, s / 4);
        for (int i = 0; i < 3; i++) t.fillCircle(cx + s / 4 + i * s / 4, cy - s * 5 / 8, s / 12, ICO_WARN);
        t.fillRect(cx - s, cy + s / 6, s * 2, s * 3 / 4, ICO_PLASTIC);
        t.fillRect(cx - s, cy + s / 6, s * 2, s / 12, ICO_SHEEN);
        t.drawRect(cx - s, cy + s / 6, s * 2, s * 3 / 4, ICO_INK);
        for (int i = 0; i < 5; i++) t.fillRect(cx - s + 5 + i * (s * 2 - 10) / 5, cy + s / 3, 3, s * 2 / 5, ICO_INK);
        break;
        case DeviceType::CAMERA: {
            const int py = cy - s * 2 / 3;
            t.fillCircle(cx, cy, s, SHELL);
            t.fillRect(cx - s - 1, cy - s - 1, s * 2 + 2, (cy - py), BG);
            t.fillCircle(cx - s / 2, cy + s / 6, s / 3, SHELL_HI);
            lens(t, cx + s / 5, cy + s / 12, s * 2 / 5);
            t.fillRect(cx - s * 5 / 4, py, s * 5 / 2, s / 4, ICO_METAL);
            t.fillRect(cx - s * 5 / 4, py, s * 5 / 2, s / 12, ICO_METAL2);
            break;
        }
        case DeviceType::SAMSUNG_TAG:
        t.fillEllipse(cx, cy, s * 3 / 4, s, ICO_PLASTIC);
        t.fillEllipse(cx - s / 4, cy - s / 3, s / 4, s / 3, ICO_SHEEN);
        t.fillCircle(cx, cy - s * 2 / 3, s / 5, BG);
        t.fillRect(cx - s / 3, cy + s / 6, s * 2 / 3, s / 4, ICO_GLASS);
        break;
        case DeviceType::GOOGLE_TAG:
        t.fillCircle(cx, cy - s / 4, s * 3 / 4, ICO_LEAF);
        t.fillTriangle(cx - s * 5 / 8, cy + s / 8, cx + s * 5 / 8, cy + s / 8, cx, cy + s, ICO_LEAF);
        t.fillCircle(cx - s / 4, cy - s / 2, s / 5, ICO_SHEEN);
        t.fillCircle(cx, cy - s / 4, s / 3, BG);
        break;
        case DeviceType::TILE:
        t.fillRect(cx - s * 3 / 4, cy - s * 3 / 4, s * 3 / 2, s * 3 / 2, ICO_PLASTIC);
        t.fillRect(cx - s * 3 / 4, cy - s * 3 / 4, s * 3 / 2, s / 6, ICO_SHEEN);
        t.fillCircle(cx + s / 2, cy - s / 2, s / 5, BG);
        t.fillRect(cx - s / 4, cy - s / 8, s / 2, s / 4, ICO_GLASS);
        t.drawRect(cx - s * 3 / 4, cy - s * 3 / 4, s * 3 / 2, s * 3 / 2, SHELL_LO);
        break;
        case DeviceType::RING:
        housing(t, cx - s * 2 / 3, cy - s, s * 4 / 3, s * 2);
        lens(t, cx, cy - s / 2, s / 2);
        t.fillCircle(cx, cy + s / 2, s / 2, ICO_INK);
        t.fillCircle(cx, cy + s / 2, s / 3, ICO_LENS);
        t.fillCircle(cx, cy + s / 2, s / 5, ICO_INK);
        break;
        case DeviceType::IBEACON:
        t.fillRect(cx - s, cy + s / 2, s * 2, s / 3, ICO_METAL);
        t.fillRect(cx - s, cy + s / 2, s * 2, s / 12, ICO_METAL2);
        t.fillEllipse(cx, cy + s / 4, s * 2 / 3, s / 3, SHELL);
        t.fillEllipse(cx, cy + s / 6, s * 2 / 3, s / 3, ICO_PLASTIC);
        t.fillEllipse(cx, cy + s / 6, s / 3, s / 6, ICO_GLASS);
        for (int i = 1; i <= 3; i++) t.drawCircle(cx, cy + s / 6, s / 2 + i * s / 3, ICO_LENS);
        break;
        case DeviceType::HACKER:
        t.fillRect(cx - s, cy - s * 2 / 3, s * 2, s * 4 / 3, ICO_LED);
        t.fillRect(cx - s, cy - s * 2 / 3, s * 2, s / 6, ICO_SHEEN);
        t.fillRect(cx - s + 3, cy - s / 2, s + 4, s * 3 / 4, ICO_INK);
        t.fillRect(cx - s + 5, cy - s / 2 + 2, s, s / 4, ICO_LEAF);
        t.fillCircle(cx + s / 2, cy + s / 4, s / 3, ICO_STEM);
        t.fillRect(cx + s / 2 - s / 5, cy + s / 4 - 3, s * 2 / 5, 6, ICO_INK);
        t.fillRect(cx + s / 2 - 3, cy + s / 4 - s / 5, 6, s * 2 / 5, ICO_INK);
        break;
        case DeviceType::EVILTWIN:
        if (s_altMode) {
            t.fillRect(cx - s / 6, cy - s / 4, s / 3, s * 5 / 4, ICO_METAL);
            t.fillRect(cx - s / 6, cy - s / 4, s / 8, s * 5 / 4, ICO_METAL2);
            t.fillTriangle(cx + s / 4, cy - s, cx + s * 3 / 4, cy - s / 2, cx + s / 2, cy - s / 4, ICO_METAL);
            for (int i = 1; i <= 3; i++) {
                t.drawCircle(cx - s / 12, cy - s / 3, i * s / 3, ICO_WARN);
            }
            t.fillTriangle(cx - s / 2, cy - s / 2, cx - s / 6, cy - s, cx - s / 8, cy - s / 3, ICO_LED);
            t.fillTriangle(cx - s / 3, cy - s / 3, cx, cy - s * 3 / 4, cx + s / 12, cy - s / 6, ICO_LED);
        } else {
            housing(t, cx - s, cy - s / 2, s * 5 / 6, s / 2);
            t.drawRect(cx + s / 6, cy - s / 2, s * 5 / 6, s / 2, ICO_WARN);
            t.drawRect(cx + s / 6 + 2, cy - s / 2 + 2, s * 5 / 6 - 4, s / 2 - 4, ICO_WARN);
            for (int i = 1; i <= 2; i++) {
                t.drawCircle(cx - s * 7 / 12, cy - s / 2, i * s / 3, ICO_LENS);
                t.drawCircle(cx + s * 7 / 12, cy - s / 2, i * s / 3, ICO_WARN);
            }
            t.fillRect(cx - s, cy + s / 2, s * 2, s / 3, ICO_PLASTIC);
            t.fillRect(cx - s + 3, cy + s / 2 + 3, s * 2 - 6, s / 8, SHELL_LO);
        }
        break;
        default:
        t.drawCircle(cx, cy, s, ICO_METAL);
        break;
    }
}