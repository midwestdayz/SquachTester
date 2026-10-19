#include "config.h" // Access centralized user configurations
#include <SPI.h>
#include <TFT_eSPI.h>
#include "ui.h"
#include "theme.h"
#include "icons.h"
#include "emulate.h"
#include <stdio.h>
#include <string.h>

int s_scannedCount = 0;
int s_scrollOffset = 0;
int s_selectedIndex = -1;
bool s_lastKbCallerIsMac = false;

namespace {

    Screen     s_screen   = Screen::GRID;
    int8_t     s_openCell = -1;
    bool       s_dirty    = true;
    bool       s_altMode  = false; 

    enum class KbPage : uint8_t { UPPER, LOWER, SYMBOLS };
    KbPage s_kbPage = KbPage::UPPER;

    char       s_inputBuf[32] = "";
    uint8_t    s_inputIdx = 0;

    const int TOP = 16;

    void geom(int w, int h, int& gx, int& gy, int& cw, int& ch) {
        const Theme::ButtonBarGeom bar = Theme::computeButtonBar(w, h);
        const int top    = TOP + Theme::listHeadingH();
        const int bottom = bar.y - 6;
        const int pad = 6, gap = 4;
        cw = (w - 2 * pad - 3 * gap) / 4;
        ch = (bottom - top - 3 * gap) / 4;
        gx = (w - (4 * cw + 3 * gap)) / 2;
        gy = top;
    }

    void kbGeom(int w, int h, int& kbx, int& kby, int& kw, int& kh) {
        const int pad = 6, gap = 4;
        const int top = 54;
        const int bottom = h - 6;
        kw = (w - 2 * pad - 5 * gap) / 6; 
        kh = (bottom - top - 6 * gap) / 7;
        kbx = pad;
        kby = top;
    }

    const char* kKeysUpper[30] = {
        "A", "B", "C", "D", "E", "F",
        "G", "H", "I", "J", "K", "L",
        "M", "N", "O", "P", "Q", "R",
        "S", "T", "U", "V", "W", "X",
        "Y", "Z", "_", "-", ".", "/"
    };

    const char* kKeysLower[30] = {
        "a", "b", "c", "d", "e", "f",
        "g", "h", "i", "j", "k", "l",
        "m", "n", "o", "p", "q", "r",
        "s", "t", "u", "v", "w", "x",
        "y", "z", " ", " ", " ", " "
    };

    const char* kKeysSymbols[30] = {
        "1", "2", "3", "4", "5", "6",
        "7", "8", "9", "0", "!", "@",
        "#", "$", "%", "^", "&", "*",
        "(", ")", "+", "=", "[", "]",
        "{", "}", "<", ">", "\"", "\\"
    };

    void drawKeyboard(TFT_eSPI& t) {
        const int w = t.width(), h = t.height();
        t.fillRect(0, 0, w, h, Theme::BG);
        Theme::drawTitleBar(t, "ENTER BROADCAST SSID");

        t.drawRect(6, 20, w - 12, 26, Theme::PURPLE);
        t.setTextColor(Theme::CYAN, Theme::BG);
        t.setCursor(12, 25);
        t.setTextFont(2);
        t.print(s_inputBuf[0] != '\0' ? s_inputBuf : "SSID Name...");

        int kbx, kby, kw, kh;
        kbGeom(w, h, kbx, kby, kw, kh);

        t.setTextFont(2);

        const char* tabs[3] = { "ABC", "abc", "123" };
        int tabW = (w - 12 - 2 * 4) / 3;
        for (uint8_t i = 0; i < 3; i++) {
            int tx = kbx + i * (tabW + 4);
            bool activeTab = (s_kbPage == (KbPage)i);
            t.drawRect(tx, kby, tabW, kh, activeTab ? Theme::AMBER : Theme::PURPLE);
            if (activeTab) t.fillRect(tx + 2, kby + 2, tabW - 4, kh - 4, Theme::PURPLE & 0x7BEF);
        
            t.setTextColor(Theme::WHITE, activeTab ? (Theme::PURPLE & 0x7BEF) : Theme::BG);
            t.setCursor(tx + (tabW - t.textWidth(tabs[i])) / 2, kby + (kh - t.fontHeight()) / 2);
            t.print(tabs[i]);
        }

        for (uint8_t i = 0; i < 30; i++) {
            int x = kbx + (i % 6) * (kw + 4);
            int y = kby + (1 + (i / 6)) * (kh + 4);
        
            const char* keyLabel = "";
            if (s_kbPage == KbPage::UPPER) keyLabel = kKeysUpper[i];
            else if (s_kbPage == KbPage::LOWER) keyLabel = kKeysLower[i];
            else keyLabel = kKeysSymbols[i];

            if (keyLabel[0] == '\0' || (s_kbPage == KbPage::LOWER && i >= 26)) {
                continue; 
            }

            t.drawRect(x, y, kw, kh, Theme::PURPLE);
            t.setTextColor(Theme::WHITE, Theme::BG);
            t.setCursor(x + (kw - t.textWidth(keyLabel)) / 2, y + (kh - t.fontHeight()) / 2);
            t.print(keyLabel);
        }

        const char* actions[4] = { "CLR", "SPC", "BKSP", "DONE" };
        int yAction = kby + 6 * (kh + 4);
        int actW = (w - 12 - 3 * 4) / 4;
        for (uint8_t i = 0; i < 4; i++) {
            int x = kbx + i * (actW + 4);
            t.drawRect(x, yAction, actW, kh, Theme::AMBER);
            t.setTextColor(Theme::WHITE, Theme::BG);
            t.setCursor(x + (actW - t.textWidth(actions[i])) / 2, yAction + (kh - t.fontHeight()) / 2);
            t.print(actions[i]);
        }
    }
    void drawGrid(TFT_eSPI& t) {
        const int w = t.width(), h = t.height();
        int gx, gy, cw, ch;
        geom(w, h, gx, gy, cw, ch);

        const bool live = Emulate::active();
        char head[40];
        if (live)
            snprintf(head, sizeof head, "LIVE: %s", shortName(Emulate::current()));
        else
            snprintf(head, sizeof head, "TAP A TARGET TO BROADCAST");
        Theme::drawListHeading(t, head, live ? Theme::AMBER : Theme::CYAN);

        t.setTextFont(1); 
        for (uint8_t i = 0; i < kGridCells; i++) {
            const int x = gx + (i % 4) * (cw + 4);
            const int y = gy + (i / 4) * (ch + 4);
            DeviceType type = typeAt(i);
            
            DeviceType checkType = (type == DeviceType::EVILTWIN && s_altMode) ? DeviceType::DEAUTH : type;
            bool on = live && Emulate::current() == checkType;

            const char* name = shortName(type);
            if (type == DeviceType::EVILTWIN && s_altMode) {
                name = "DEAUTH"; 
            }

            uint16_t edge = on ? Theme::AMBER : Theme::PURPLE;
            t.fillRect(x, y, cw, ch, Theme::BG);
            t.drawRect(x, y, cw, ch, edge);

            // CORRECTION: Keep 'type' tracking here so DEAUTH switches properly on the display grid 
            const int s = (cw < ch ? cw : ch) / 2 - 5;
            if (s >= 4) {
                drawTypeIcon(t, type, x + cw / 2, y + ch / 2 - 4, s);
            }

            t.setTextColor(on ? Theme::AMBER : Theme::WHITE, Theme::BG);
            t.setCursor(x + (cw - t.textWidth(name)) / 2, y + ch - t.fontHeight() - 2);
            t.print(name);
        }

        Theme::ButtonBarGeom bar = Theme::computeButtonBar(w, h);
        bar.y = h - bar.h - 4;

        t.fillRect(0, bar.y - 2, w, h - (bar.y - 2), Theme::BG);
        t.drawFastHLine(0, bar.y - 2, w, Theme::PURPLE);

        Theme::drawButton(t, bar.x[0], bar.y, bar.w[0], bar.h, "Config", false);
        Theme::drawButton(t, bar.x[2], bar.y, bar.w[2], bar.h, "ALT", s_altMode);

        char txtSsid[48];
        snprintf(txtSsid, sizeof txtSsid, "SSID: %s", Emulate::getCustomSsid());
        t.setTextColor(Theme::WHITE, Theme::BG);
        t.setCursor(bar.x[1] + (bar.w[1] - t.textWidth(txtSsid)) / 2, bar.y + 4);
        t.print(txtSsid);

        char txtMac[48];
        const uint8_t* rawMac = Emulate::getTargetDeauthMac();
        snprintf(txtMac, sizeof txtMac, "MAC: %02X:%02X:%02X:%02X:%02X:%02X", 
                 rawMac[0], rawMac[1], rawMac[2], rawMac[3], rawMac[4], rawMac[5]);
        
        t.setTextColor(Theme::AMBER, Theme::BG);
        t.setCursor(bar.x[1] + (bar.w[1] - t.textWidth(txtMac)) / 2, bar.y + 14);
        t.print(txtMac);
    }

    void detailGeom(int screenW, int screenH,
    int& px, int& py, int& pw, int& ph,
    int& emuX, int& emuY, int& emuW, int& emuH,
    int& backX, int& backY, int& backW, int& backH) {
        pw = screenW - 24; if (pw > 280) pw = 280;
        ph = screenH - 28; if (ph > 200) ph = 200;
        px = (screenW - pw) / 2;
        py = (screenH - ph) / 2;
        const int margin = 10, gap = 8, btnH = 24;
        backY = py + ph - btnH - margin;
        backH = btnH;
        backX = px + margin;
        backW = pw - 2 * margin;
        emuY  = backY - gap - btnH;
        emuH  = btnH;
        emuX  = px + margin;
        emuW  = pw - 2 * margin;
    }

    void drawDetail(TFT_eSPI& t) {
        const int w = t.width(), h = t.height();
        int px, py, pw, ph, emuX, emuY, emuW, emuH, backX, backY, backW, backH;
        detailGeom(w, h, px, py, pw, ph, emuX, emuY, emuW, emuH, backX, backY, backW, backH);

        const DeviceType type = typeAt((uint8_t)s_openCell);
        t.fillRoundRect(px, py, pw, ph, 6, Theme::BG);
        t.drawRoundRect(px, py, pw, ph, 6, Theme::PURPLE);

        DeviceType checkType = (type == DeviceType::EVILTWIN && s_altMode) ? DeviceType::DEAUTH : type;
        drawTypeIcon(t, checkType, px + 28, py + 28, 18);
        t.setTextColor(Theme::CYAN, Theme::BG);
        t.setCursor(px + 52, py + 12);
        
        t.setTextFont(1);
        t.print(deviceName(checkType));
        
        t.setTextColor(Theme::WHITE, Theme::BG);
        char lines[8][48];
        const char* explainStr = deviceExplain(checkType);

        const uint8_t n = Theme::wrapText(t, explainStr, pw - 20, lines, 8);
        int ly = py + 48;
        for (uint8_t i = 0; i < n && ly + 12 < emuY - 4; i++) {
            t.setCursor(px + 10, ly);
            t.print(lines[i]);
            ly += 12;
        }

        const bool on = Emulate::active() && Emulate::current() == checkType;

        if (checkType == DeviceType::AIRTAG && !on) {
            // CORRECTION: Query AIRTAG_NRF_CS_PIN dynamically out of config.h
            pinMode(AIRTAG_NRF_CS_PIN, INPUT_PULLDOWN);
            delayMicroseconds(50);
            int hardwareDetection = digitalRead(AIRTAG_NRF_CS_PIN);
        
            pinMode(AIRTAG_NRF_CS_PIN, OUTPUT);
            digitalWrite(AIRTAG_NRF_CS_PIN, HIGH);
            
            if (hardwareDetection == HIGH) {
                Theme::drawButton(t, emuX, emuY, emuW, emuH, "[ EMULATE ]", false);
            } else {
                t.fillRoundRect(emuX, emuY, emuW, emuH, 4, Theme::BG);
                t.drawRoundRect(emuX, emuY, emuW, emuH, 4, TFT_RED);
                t.setTextColor(TFT_RED, Theme::BG);
                t.drawCentreString("[ UNAVAILABLE ]", emuX + (emuW / 2), emuY + (emuH / 2) - 4, 1);
            }
        } else {
            Theme::drawButton(t, emuX, emuY, emuW, emuH, on ? "[ STOP ]" : "[ EMULATE ]", on);
        }
        
        Theme::drawButton(t, backX, backY, backW, backH, "[ BACK ]", false);
    }
    void drawConfigPopup(TFT_eSPI& t) {
        const int w = t.width(), h = t.height();
        int pw = w - 40; if (pw > 240) pw = 240;
        int ph = h - 20; if (ph > 180) ph = 180;
        int px = (w - pw) / 2;
        int py = (h - ph) / 2;

        t.fillRoundRect(px, py, pw, ph, 6, Theme::BG);
        t.drawRoundRect(px, py, pw, ph, 6, Theme::PURPLE);

        Theme::drawListHeading(t, "CONFIGURATION MENU", Theme::CYAN);

        int btnW = pw - 30;
        int btnH = 20;
        int btnX = px + 15;
        
        int btnY1 = py + 24;
        int btnY2 = btnY1 + btnH + 5;
        int btnY3 = btnY2 + btnH + 5;
        int btnY4 = btnY3 + btnH + 5;
        int btnY5 = btnY4 + btnH + 5;

        Theme::drawButton(t, btnX, btnY1, btnW, btnH, "Set SSID (Manual)", false);
        Theme::drawButton(t, btnX, btnY2, btnW, btnH, "Select SSID (Live Scan)", false);
        Theme::drawButton(t, btnX, btnY3, btnW, btnH, "Set MAC (Manual)", false);
        Theme::drawButton(t, btnX, btnY4, btnW, btnH, "Select MAC (Live Scan)", false);
        Theme::drawButton(t, btnX, btnY5, btnW, btnH, "[ CLOSE ]", false);
    }

    void drawScanList(TFT_eSPI& t) {
        const int w = t.width(), h = t.height();
        t.fillRect(0, 0, w, h, Theme::BG);
        Theme::drawListHeading(t, "SELECT TARGET NETWORK MAC", Theme::AMBER);

        t.setTextFont(1);

        for (int i = 0; i < 6; i++) {
            int itemIndex = s_scrollOffset + i;
            if (itemIndex >= s_scannedCount) break;

            int y = 24 + i * 22;
            bool isSelected = (itemIndex == s_selectedIndex);

            if (isSelected) {
                t.fillRect(0, y, w, 20, Theme::PURPLE);
            }
            t.drawFastHLine(0, y + 20, w, isSelected ? Theme::AMBER : Theme::BG);
            
            t.setTextColor(isSelected ? Theme::AMBER : Theme::CYAN, isSelected ? Theme::PURPLE : Theme::BG);
            t.setCursor(6, y + 4);
            t.print(Emulate::getScannedSsidStr(itemIndex));

            t.setTextColor(isSelected ? Theme::WHITE : Theme::WHITE, isSelected ? Theme::PURPLE : Theme::BG);
            t.setCursor(w - t.textWidth("00:00:00:00:00:00") - 6, y + 4);
            t.print(Emulate::getScannedMacStr(itemIndex));
        }

        int btnW = (w - 18) / 2;
        int btnH = 24;
        int btnY = h - btnH - 6;
        int leftBtnX = 6;
        int rightBtnX = leftBtnX + btnW + 6;

        Theme::drawButton(t, leftBtnX, btnY, btnW, btnH, "[ BACK ]", false);
        Theme::drawButton(t, rightBtnX, btnY, btnW, btnH, "[ CONFIRM ]", s_selectedIndex != -1);

        if (s_scrollOffset > 0) {
            t.setTextColor(Theme::AMBER, Theme::BG);
            t.setCursor(w / 2 - 4, 16); t.print("^");
        }
        if (s_scrollOffset + 6 < s_scannedCount) {
            t.setTextColor(Theme::AMBER, Theme::BG);
            t.setCursor(w / 2 - 4, btnY - 12); t.print("v");
        }
    }

    void drawScanSsidList(TFT_eSPI& t) {
        const int w = t.width(), h = t.height();
        t.fillRect(0, 0, w, h, Theme::BG);
        Theme::drawListHeading(t, "SELECT TARGET NETWORK SSID", Theme::CYAN);

        t.setTextFont(1);

        for (int i = 0; i < 6; i++) {
            int itemIndex = s_scrollOffset + i;
            if (itemIndex >= s_scannedCount) break;

            int y = 24 + i * 22;
            bool isSelected = (itemIndex == s_selectedIndex);

            if (isSelected) {
                t.fillRect(0, y, w, 20, Theme::PURPLE);
            }
            t.drawFastHLine(0, y + 20, w, isSelected ? Theme::AMBER : Theme::BG);
            
            t.setTextColor(isSelected ? Theme::AMBER : Theme::WHITE, isSelected ? Theme::PURPLE : Theme::BG);
            t.setCursor(12, y + 4);
            t.print(Emulate::getScannedSsidStr(itemIndex));
        }

        int btnW = (w - 18) / 2;
        int btnH = 24;
        int btnY = h - btnH - 6;
        int leftBtnX = 6;
        int rightBtnX = leftBtnX + btnW + 6;

        Theme::drawButton(t, leftBtnX, btnY, btnW, btnH, "[ BACK ]", false);
        Theme::drawButton(t, rightBtnX, btnY, btnW, btnH, "[ CONFIRM ]", s_selectedIndex != -1);

        if (s_scrollOffset > 0) {
            t.setTextColor(Theme::AMBER, Theme::BG);
            t.setCursor(w / 2 - 4, 16); t.print("^");
        }
        if (s_scrollOffset + 6 < s_scannedCount) {
            t.setTextColor(Theme::AMBER, Theme::BG);
            t.setCursor(w / 2 - 4, btnY - 12); t.print("v");
        }
    }

} // This correctly closes the internal private namespace wrapper

void uiInit(TFT_eSPI& t) {
    s_screen   = Screen::GRID;
    s_openCell = -1;
    s_dirty    = true;
    s_altMode  = false;
    t.fillScreen(Theme::BG);
}

void uiDraw(TFT_eSPI& t) {
    if (!s_dirty) return;
    s_dirty = false;

    if (s_screen == Screen::KEYBOARD) {
        drawKeyboard(t);
    } 
    else if (s_screen == Screen::SCAN_LIST) {
        drawScanList(t);
    } 
    else if (s_screen == Screen::CONFIG) {
        t.fillScreen(Theme::BG);
        Theme::drawTitleBar(t, ">> SquachWatch Tester V.8.3 <<");
        drawGrid(t);
        drawConfigPopup(t); 
    } 
    else if (s_screen == Screen::SCAN_SSID_LIST) {
        drawScanSsidList(t);
    }
    else if (s_screen == Screen::GRID) {
        t.fillScreen(Theme::BG);
        Theme::drawTitleBar(t, ">> SquachWatch Tester V.8.3 <<");
        drawGrid(t);
    } 
    else if (s_screen == Screen::DETAIL) {
        t.fillScreen(Theme::BG);
        Theme::drawTitleBar(t, ">> SquachWatch Tester V.8.3 <<");
        drawGrid(t);
        if (s_openCell >= 0) {
            drawDetail(t);
        }
    }
}
void uiHandleTap(TFT_eSPI& t, int x, int y) {
    const int w = t.width(), h = t.height();
    const Theme::ButtonBarGeom bar = Theme::computeButtonBar(w, h);

    if (s_screen == Screen::KEYBOARD) {
        int kbx, kby, kw, kh;
        kbGeom(w, h, kbx, kby, kw, kh);

        if (y >= kby && y < kby + kh) {
            int tabW = (w - 12 - 2 * 4) / 3;
            for (uint8_t i = 0; i < 3; i++) {
                int tx = kbx + i * (tabW + 4);
                if (x >= tx && x < tx + tabW) {
                    s_kbPage = (KbPage)i;
                    s_dirty = true;
                    return;
                }
            }
            return;
        }

        int yAction = kby + 6 * (kh + 4);
        int actW = (w - 12 - 3 * 4) / 4;
        if (y >= yAction && y < yAction + kh) {
            for (uint8_t i = 0; i < 4; i++) {
                int kx = kbx + i * (actW + 4);
                if (x >= kx && x < kx + actW) {
                    if (i == 0) {
                        s_inputIdx = 0;
                        s_inputBuf[0] = '\0';
                    } else if (i == 1) {
                        if (s_inputIdx < 31) s_inputBuf[s_inputIdx++] = ' ';
                        s_inputBuf[s_inputIdx] = '\0';
                    } else if (i == 2) {
                        if (s_inputIdx > 0) s_inputBuf[--s_inputIdx] = '\0';
                    } else if (i == 3) {
                        if (s_inputIdx > 0) {
                            Emulate::setCustomSsid(s_inputBuf);
                        }
                        s_screen = Screen::GRID;
                    }
                    s_dirty = true;
                    return;
                }
            }
            return;
        }

        for (uint8_t i = 0; i < 30; i++) {
            int kx = kbx + (i % 6) * (kw + 4);
            int ky = kby + (1 + (i / 6)) * (kh + 4);
            
            if (s_kbPage == KbPage::LOWER && i >= 26) continue;

            if (x >= kx && x < kx + kw && y >= ky && y < ky + kh) {
                const char* targetChar = "";
                if (s_kbPage == KbPage::UPPER) targetChar = kKeysUpper[i];
                else if (s_kbPage == KbPage::LOWER) targetChar = kKeysLower[i];
                else targetChar = kKeysSymbols[i];

                if (s_inputIdx < 31 && targetChar && targetChar[0] != '\0') {
                    s_inputBuf[s_inputIdx++] = targetChar[0];
                    s_inputBuf[s_inputIdx] = '\0';
                }
                s_dirty = true;
                return;
            }
        }
        return;
    }

    if (s_screen == Screen::CONFIG) {
        int pw = w - 40; if (pw > 240) pw = 240;
        int ph = h - 20; if (ph > 180) ph = 180;
        int px = (w - pw) / 2;
        int py = (h - ph) / 2;
        
        int btnW = pw - 30;
        int btnH = 20;
        int btnX = px + 15;
        int btnY1 = py + 24;
        int btnY2 = btnY1 + btnH + 5;
        int btnY3 = btnY2 + btnH + 5;
        int btnY4 = btnY3 + btnH + 5;
        int btnY5 = btnY4 + btnH + 5;

        if (x >= btnX && x <= btnX + btnW && y >= btnY1 && y <= btnY1 + btnH) {
            s_lastKbCallerIsMac = false;
            s_screen = Screen::KEYBOARD;
            s_inputIdx = 0;
            s_inputBuf[0] = '\0';
            s_dirty = true;
            return;
        }

        if (x >= btnX && x <= btnX + btnW && y >= btnY2 && y <= btnY2 + btnH) {
            t.fillRect(0, 0, w, h, Theme::BG);
            Theme::drawListHeading(t, "SCANNING... PLEASE WAIT", Theme::CYAN);
            
            s_scannedCount = Emulate::scanNetworks(); 
            s_scrollOffset = 0;
            s_selectedIndex = -1;
            
            extern unsigned long globalLastScanTime;
            globalLastScanTime = millis(); 
            
            s_screen = Screen::SCAN_SSID_LIST;
            s_dirty = true;
            return;
        }

        if (x >= btnX && x <= btnX + btnW && y >= btnY3 && y <= btnY3 + btnH) {
            s_lastKbCallerIsMac = true;
            s_screen = Screen::KEYBOARD;
            s_inputIdx = 0;
            s_inputBuf[0] = '\0';
            s_dirty = true;
            return;
        }

        if (x >= btnX && x <= btnX + btnW && y >= btnY4 && y <= btnY4 + btnH) {
            t.fillRect(0, 0, w, h, Theme::BG);
            Theme::drawListHeading(t, "SCANNING... PLEASE WAIT", Theme::AMBER);
            
            s_scannedCount = Emulate::scanNetworks(); 
            s_scrollOffset = 0;
            s_selectedIndex = -1;
            
            extern unsigned long globalLastScanTime;
            globalLastScanTime = millis(); 
            
            s_screen = Screen::SCAN_LIST;
            s_dirty = true;
            return;
        }

        if (x >= btnX && x <= btnX + btnW && y >= btnY5 && y <= btnY5 + btnH) {
            s_screen = Screen::GRID;
            s_dirty = true;
            return;
        }
        return;
    }

    if (s_screen == Screen::SCAN_SSID_LIST) {
        int btnW = (w - 18) / 2;
        int btnH = 24;
        int btnY = h - btnH - 6;

        if (y >= btnY && y <= btnY + btnH) {
            if (x >= 6 && x <= 6 + btnW) {
                s_screen = Screen::CONFIG;
                s_dirty = true;
                return;
            }
            if (x >= 6 + btnW + 6 && x <= w - 6) {
                if (s_selectedIndex >= 0 && s_selectedIndex < s_scannedCount) {
                    Emulate::setCustomSsid(Emulate::getScannedSsidStr(s_selectedIndex));
                }
                s_screen = Screen::GRID; 
                s_dirty = true;
                return;
            }
            return;
        }

        if (s_scannedCount > 0 && y > 24 && y < btnY - 12) {
            if (y < 24 + 30 && s_scrollOffset > 0) {
                s_scrollOffset--; s_dirty = true; return;
            }
            if (y > btnY - 42 && (s_scrollOffset + 6 < s_scannedCount)) {
                s_scrollOffset++; s_dirty = true; return;
            }

            int rowClicked = (y - 24) / 22;
            if (rowClicked >= 0 && rowClicked < 6) {
                int targetIndex = s_scrollOffset + rowClicked;
                if (targetIndex < s_scannedCount) {
                    s_selectedIndex = targetIndex;
                }
                s_dirty = true;
                return;
            }
        }
        return;
    }

    if (s_screen == Screen::SCAN_LIST) {
        int btnW = (w - 18) / 2;
        int btnH = 24;
        int btnY = h - btnH - 6;

        if (y >= btnY && y <= btnY + btnH) {
            if (x >= 6 && x <= 6 + btnW) {
                s_screen = Screen::CONFIG;
                s_dirty = true;
                return;
            }
            if (x >= 6 + btnW + 6 && x <= w - 6) {
                if (s_selectedIndex >= 0 && s_selectedIndex < s_scannedCount) {
                    Emulate::setTargetMacFromScan(s_selectedIndex);
                }
                s_screen = Screen::GRID; 
                s_dirty = true;
                return;
            }
            return;
        }

        if (s_scannedCount > 0 && y > 24 && y < btnY - 12) {
            if (y < 24 + 30 && s_scrollOffset > 0) {
                s_scrollOffset--; s_dirty = true; return;
            }
            if (y > btnY - 42 && (s_scrollOffset + 6 < s_scannedCount)) {
                s_scrollOffset++; s_dirty = true; return;
            }

            int rowClicked = (y - 24) / 22;
            if (rowClicked >= 0 && rowClicked < 6) {
                int targetIndex = s_scrollOffset + rowClicked;
                if (targetIndex < s_scannedCount) {
                    s_selectedIndex = targetIndex;
                }
                s_dirty = true;
                return;
            }
        }
        return;
    }

    if (y >= bar.y && y <= bar.y + bar.h && s_screen == Screen::GRID) {
        if (x >= bar.x[0] && x <= bar.x[0] + bar.w[0]) {
            s_screen = Screen::CONFIG;
            s_dirty = true;
            return;
        }
        if (x >= bar.x[2] && x <= bar.x[2] + bar.w[2]) {
            s_altMode = !s_altMode;
            s_dirty = true;
            return;
        }
    }

    if (s_screen == Screen::DETAIL) {
        int px, py, pw, ph, emuX, emuY, emuW, emuH, backX, backY, backW, backH;
        detailGeom(w, h, px, py, pw, ph, emuX, emuY, emuW, emuH, backX, backY, backW, backH);
        const DeviceType type = typeAt((uint8_t)s_openCell);
        
        DeviceType checkType = (type == DeviceType::EVILTWIN && s_altMode) ? DeviceType::DEAUTH : type;

        if (x >= emuX && x <= emuX + emuW && y >= emuY && y <= emuY + emuH) {
            if (Emulate::active() && Emulate::current() == checkType) {
                Emulate::stop();
            } else {
                if (checkType == DeviceType::AIRTAG) {
                    pinMode(AIRTAG_NRF_CS_PIN, INPUT_PULLDOWN);
                    delayMicroseconds(50);
                    int hardwareDetection = digitalRead(AIRTAG_NRF_CS_PIN);
                    
                    pinMode(AIRTAG_NRF_CS_PIN, OUTPUT);
                    digitalWrite(AIRTAG_NRF_CS_PIN, HIGH);
                    
                    if (hardwareDetection == HIGH) {
                        Emulate::start(checkType);
                    } else {
                        Serial.println("[BLOCKED] Touch action ignored. AirTag module is missing.");
                        return;
                    }
                } else {
                    Emulate::start(checkType);
                }              
            }
            s_dirty = true;
            return;
        }
        if (x >= backX && x <= backX + backW && y >= backY && y <= backY + backH) {
            s_screen   = Screen::GRID;
            s_openCell = -1;
            s_dirty    = true;
            return;
        }
        return;
    }

    if (s_screen == Screen::GRID) {
        int gx, gy, cw, ch;
        geom(w, h, gx, gy, cw, ch);
        for (uint8_t i = 0; i < kGridCells; i++) {
            const int cx = gx + (i % 4) * (cw + 4);
            const int cy = gy + (i / 4) * (ch + 4);
            if (x >= cx && x < cx + cw && y >= cy && y < cy + ch) {
                s_openCell = (int8_t)i;
                s_screen   = Screen::DETAIL;
                s_dirty    = true;
                return;
            }
        }
    }
}

Screen uiScreen() { return s_screen; }
DeviceType uiOpenType() { return s_openCell >= 0 ? typeAt((uint8_t)s_openCell) : DeviceType::FLOCK; }
bool uiGetAltMode() { return s_altMode; }
