#pragma once
#include <TFT_eSPI.h>
#include "devices.h"

enum class Screen : uint8_t { 
    GRID, 
    DETAIL,
    KEYBOARD,
    CONFIG,
    SCAN_LIST,
    SCAN_SSID_LIST
};

void uiInit(TFT_eSPI& t);
void uiDraw(TFT_eSPI& t);
void uiHandleTap(TFT_eSPI& t, int x, int y);

Screen uiScreen();
DeviceType uiOpenType();
bool uiGetAltMode();