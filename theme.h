#pragma once
#include "cyd_includes.h"
#include <stdint.h>

namespace Theme {

    extern uint16_t BG;
    extern uint16_t PURPLE;
    extern uint16_t CYAN;
    extern uint16_t GREEN;
    extern uint16_t AMBER;
    extern uint16_t RED;
    extern uint16_t WHITE;

    constexpr uint16_t W95_LIGHT  = 0xDEFB;
    constexpr uint16_t W95_SHADOW = 0x8410;

    struct ButtonBarGeom {
        int x[3], w[3], y, h;
    };

    ButtonBarGeom computeButtonBar(int screenW, int screenH);
    void drawTitleBar(TFT_eSPI& t, const char* title);
    void drawListHeading(TFT_eSPI& t, const char* text, uint16_t color);
    int  listHeadingH();
    void drawButton(TFT_eSPI& t, int x, int y, int w, int h,
    const char* label, bool pressed);
    uint8_t wrapText(TFT_eSPI& t, const char* text, int maxW,
    char lines[][48], uint8_t maxLines);
}
