# 🎛️ SquachTester — Cheap Yellow Display (CYD) Edition

[![Platform: ESP32](https://shields.io)](https://wikipedia.org)
[![Co-Processor: nRF52](https://shields.io)](https://nordicsemi.com)
[![License: MIT](https://shields.io)](https://opensource.org)

Welcome to **SquachTester**, a multi-protocol simulation, wireless telemetry evaluation, and hardware validation suite designed specifically for the **2.8" Cheap Yellow Display (CYD)** (ESP32-2432S028R). 

This platform allows you to evaluate wireless network frame structures, simulate asset tracking beacons, and cross-verify physical module signaling sequences using an interactive touchscreen control grid.

---

## 🏗️ Hardware Architecture & Configuration Modes

The application can be deployed in two distinct hardware configurations depending on your evaluation requirements:

### 📱 Mode A: Standalone Mode (Default / No Extra Hardware)
Runs exclusively on the stock Cheap Yellow Display unit. All Wi-Fi, BLE, and 802.11 frame diagnostic profiles run directly on the internal ESP32 SoC chip.
* > ⚠️ **CRITICAL NOTE:** The **AirTag emulation function is hardware-dependent and will NOT function in this mode.** Attempting to activate it without the co-processor attached will trigger a software safety lock and log a warning to the Serial interface without flashing invalid frames.

### 🔌 Mode B: Co-Processor Expanded Mode (Optional Add-on)
Enables valid cryptographic Apple Find My / OpenHaystack beacon testing by linking an external **nRF52 controller** board (such as an Adafruit Feather nRF52 or Seeed Xiao nRF52) to the CYD module.
* > ⚠️ **Caution:** - **Designed for the ProMicro NRF52840.** You may have to make some changes to fit your chosen NRF board.

```text
  +-----------------------+               +-----------------------+

  |    Cheap Yellow       |               |    Optional nRF52     |
  |    Display (ESP32)    |               |    Co-Processor       |
  |                       |               |   (Required for AT)   |
  |  [CS PIN / GPIO 5] ---+---------------> [PIN 12 / TRIGGER]    |
  |                       |               |                       |
  |  [GND] ---------------+---------------+ [GND]                 |
  +-----------------------+               +-----------------------+
```

* **The Signaling Wire:** Connect your designated display control pin (`AIRTAG_NRF_CS_PIN`, typically **GPIO 5** or **GPIO 22** on the P3 rear expansion headers) directly to the nRF52's physical **Pin 12**.
* **The Common Ground Loop:** You **MUST** connect a shared ground wire between `GND` pins on both boards to prevent floating signal voltages from causing erratic false triggers.

---

## 🎛️ Master Display Configuration (`config.h`)

All system configurations are centralized inside a single file: `config.h`. Drop this file directly into your root directory next to your main sketch file. You can adapt interface drivers, change backlight mappings, or adjust optional co-processor signaling lines instantly by toggling preprocessor flags:

```cpp
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
// #define FIX_INVERTED_COLORS 

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
// 4. AIRTAG NRF MODULE PIN CONFIGURATION (OPTIONAL Add-on)
// ============================================================================
// Set the CE and CS pin numbers for your optional AirTag NRF module connection.
// IMPORTANT: The AirTag tracking engine requires external nRF52 hardware 
// wired to these pins. Without it, the function remains disabled in software.
#define AIRTAG_NRF_CE_PIN   4        // Chip Enable (CE) Pin
#define AIRTAG_NRF_CS_PIN   5        // Chip Select (CS / SS) Pin

// ============================================================================
// 5. HARDWARE PIN MAPPING (Automated)
// ============================================================================
#define TFT_MISO   12
#define TFT_MOSI   13
#define TFT_SCLK   14
#define TFT_CS     15
#define TFT_DC      2
#define TFT_RST    -1

#define TOUCH_SCK  25
#define TOUCH_MOSI 32
#define TOUCH_MISO 39
#define TOUCH_CS   33
#define TOUCH_IRQ  36

#if defined(CYD_28_SINGLE_USB)
    #define TFT_BL 21
#elif defined(CYD_28_DUAL_USB)
    #define TFT_BL 22
#endif

#endif // CONFIG_H
```

---

## 📡 Optional Co-Processor Firmware (`NRF_AirTag.ino`)

If you decide to utilize **Mode B** to unlock AirTag verification routines, flash the following firmware to your external nRF52 module board. It monitors physical Pin 12. When it encounters an execution pulse pulled low by the display unit, it activates a cryptographic tracking framework via the Apple Find My / OpenHaystack payload scheme.

```cpp
// Set this to match the physical wire connection from your CYD display's AIRTAG_NRF_CS_PIN
const int TRIGGER_INPUT_PIN = 12; 

```

---

## 🎛️ Diagnostic Feature Grid Summary

The core application firmware drives an adaptive touchscreen grid layout supporting 17 structural testing cells:

| **Feature ID** | **Graphic Tag** | **Hardware Target** | **Hardware Dependencies** | **Functional Description** |
| :--- | :--- | :--- | :--- | :--- |
| **FLOCK** | `FLOCK` | Master ESP32 | *None (Standalone)* | Simulates neighborhood automated license-plate-reader frames. |
| **AXON** | `AXON` | Master ESP32 | *None (Standalone)* | Simulates bodycam system hotspot routing loops. |
| **META** | `GLASS` | Master ESP32 | *None (Standalone)* | Emulates smart camera glasses BLE discovery frames. |
| **SKIMMER** | `SKIM` | Master ESP32 | *None (Standalone)* | Replicates legacy card reader exfiltration patterns. |
| **RAVEN** | `RAV` | Master ESP32 | *None (Standalone)* | Simulates rooftop gun-shot detector beacon outputs. |
| **AIRTAG** | `TRACKER` | External Unit | 🔴 **Requires Optional nRF52** | **Emulates an AirTag *May not be needed*.** |
| **DRONE** | `DRONE` | Master ESP32 | *None (Standalone)* | Evaluates compliance of FAA standard Remote ID pings. |
| **SAMSUNG** | `STAG` | Master ESP32 | *None (Standalone)* | Simulates item locator maps on Galaxy SmartTag protocols. |
| **GOOGLE** | `GTAG` | Master ESP32 | *None (Standalone)* | Evaluates generic tracker arrays inside Android networks. |
| **TILE** | `TILE` | Master ESP32 | *None (Standalone)* | Replicates standard legacy vendor location finding tags. |
| **RING** | `RING` | Master ESP32 | *None (Standalone)* | Masks Wi-Fi identification blocks as an active video doorbell. |
| **IBEACON** | `BEACON` | Master ESP32 | *None (Standalone)* | Runs multi-unit micro-location proximity identification. |
| **HACKER** | `HACK` | Master ESP32 | *None (Standalone)* | Fires advertising bursts to challenge system defensive layers. |
| **EVILTWIN** | `TWIN` | Master ESP32 | *None (Standalone)* | Audits access point mismatches against rogue structures. |
| **DEAUTH** | `JAM` | Master ESP32 | *None (Standalone)* | Transmits raw management frames validating client handshakes. |

---

## ⚙️ Compilation & Deployment Instructions

### 1. Workspace Libraries
Ensure you install the following dependencies into your development environment (Arduino IDE or PlatformIO):
* **TFT_eSPI** (Centralized drivers handle hardware configuration variables dynamically)
* **XPT2046_Touchscreen** (Translates SPI panel touch coordinates fields)
* **NimBLE-Arduino** (Optimized, lightweight Bluetooth Low Energy mesh provider)

### 2. Peripheral Interlock Isolation
When selecting the `AIRTAG` menu item on the control screen, the master display checks the physical voltage state of the configuration pin. If the optional add-on module is absent or disconnected, the action is safety-locked, printing `[BLOCKED] Touch action ignored. AirTag module is missing.` over the Serial interface to keep the application from entering an unstable routine state.

### 3. Firmware Flash
1. Configure your target board settings in `config.h` (e.g., driver type and pin setups).
2. Open `SquachTester.ino` and target the **ESP32 Dev Module** parameters.
3. Compile and flash the payload to your board.