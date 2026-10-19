#include "theme.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <string.h>

namespace Theme {

    uint16_t BG     = 0x0801;
    uint16_t PURPLE = 0xAC1F;
    uint16_t CYAN   = 0x07FF;
    uint16_t GREEN  = 0x07E0;
    uint16_t AMBER  = 0xFD20;
    uint16_t RED    = 0xF800;
    uint16_t WHITE  = 0xFFFF;

    ButtonBarGeom computeButtonBar(int screenW, int screenH) {
        ButtonBarGeom g;
        g.h = 22;
        const int margin = 6, gap = 6;
        g.y = screenH - g.h - 6;
        g.w[0] = 65; 
        g.w[2] = 65; 
        g.w[1] = screenW - (2 * margin) - (2 * gap) - g.w[0] - g.w[2];
        g.x[0] = margin;
        g.x[1] = g.x[0] + g.w[0] + gap;
        g.x[2] = g.x[1] + g.w[1] + gap;
        return g;
    }

    int listHeadingH() { return 14; }

    void drawTitleBar(TFT_eSPI& t, const char* title) {
        const int w = t.width();
        t.fillRect(0, 0, w, 14, BG);
        t.setTextFont(1);
        t.setTextSize(1);
        t.setTextColor(CYAN, BG);
        const int tw = t.textWidth(title);
        t.setCursor((w - tw) / 2, 3);
        t.print(title);
        t.drawFastHLine(0, 13, w, PURPLE);
    }

    void drawListHeading(TFT_eSPI& t, const char* text, uint16_t color) {
        t.setTextFont(1);
        t.setTextSize(1);
        t.setTextColor(color, BG);
        t.setCursor(8, 16);
        t.print(text);
    }

    void drawButton(TFT_eSPI& t, int x, int y, int w, int h,
    const char* label, bool pressed) {
        const uint16_t fill = pressed ? PURPLE : BG;
        const uint16_t fg   = pressed ? WHITE : CYAN;
        t.fillRect(x, y, w, h, fill);
        t.drawRect(x, y, w, h, PURPLE);
        t.setTextFont(1);
        t.setTextSize(1);
        t.setTextColor(fg, fill);
        const int tw = t.textWidth(label);
        const int th = t.fontHeight();
        t.setCursor(x + (w - tw) / 2, y + (h - th) / 2);
        t.print(label);
    }

    uint8_t wrapText(TFT_eSPI& t, const char* text, int maxW,
    char lines[][48], uint8_t maxLines) {
        if (!text || maxLines == 0) return 0;
        uint8_t n = 0;
        const char* p = text;
        while (*p && n < maxLines) {
            while (*p == ' ') p++;
            if (!*p) break;
            int best = 0, lastSpace = -1;
            char buf[48];
            int bi = 0;
            for (int i = 0; p[i] && bi < 46; i++) {
                buf[bi] = p[i];
                buf[bi + 1] = 0;
                if (t.textWidth(buf) > maxW) break;
                best = i + 1;
                if (p[i] == ' ') lastSpace = i;
                bi++;
            }
            if (best == 0) best = 1;
            if (lastSpace > 0 && p[best] && p[best] != ' ') best = lastSpace;
            int len = best;
            if (len > 47) len = 47;
            memcpy(lines[n], p, len);
            lines[n][len] = 0;
            while (len > 0 && lines[n][len - 1] == ' ') lines[n][--len] = 0;
            p += best;
            n++;
        }
        return n;
    }
}
