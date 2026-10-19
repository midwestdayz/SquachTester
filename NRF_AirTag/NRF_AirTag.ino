#include <bluefruit.h>

const uint8_t openHaystackPublicKey[28] = {
  0x0E, 0x32, 0xE8, 0x22, 0xBE, 0xB9, 0x7F, 0x43, 0xBE, 0x57, 0x44, 0xB3, 0x69, 0x3C, 0xFC, 0xD2,
  0x93, 0x76, 0x34, 0x75, 0x51, 0x0A, 0xD0, 0x12, 0xF3, 0x18, 0x4F, 0xC6
};

bool isAdvertising = false;
int lastPinState = -1; 

const int TRIGGER_INPUT_PIN = 12; 

void setup() {
  Serial.begin(115200);
  
  uint32_t startupTimeout = millis();
  while (!Serial && (millis() - startupTimeout < 4000)) { delay(10); }

  pinMode(TRIGGER_INPUT_PIN, INPUT_PULLUP);

  Bluefruit.begin();
  Bluefruit.setTxPower(4); 

  Serial.println("=========================================");
  Serial.println("   VERIFIED PIN 12 POLLING LINK ACTIVE   ");
  Serial.println("=========================================");
  Serial.println("[STATUS] Waiting for valid CYD emulation inputs...");
  
  lastPinState = digitalRead(TRIGGER_INPUT_PIN);
}

void armOpenHaystackPayload() {
  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();

  ble_gap_addr_t randomAddr;
  randomAddr.addr_type = BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE;
  memcpy(randomAddr.addr, openHaystackPublicKey, 6);
  randomAddr.addr[5] |= 0x40; 
  randomAddr.addr[5] &= 0x7F;
  Bluefruit.setAddr(&randomAddr);

  uint8_t rawPayload[31] = {
    0x1E, 0xFF, 0x4C, 0x00, 0x12, 0x19, 0x10,                                           
    0x32, 0xE8, 0x22, 0xBE, 0xB9, 0x7F, 0x43, 0xBE, 0x57, 0x44, 0xB3, 0x69, 0x3C, 0xFC, 0xD2, 0x93, 0x76, 0x34, 0x75, 0x51, 0x0A, 0xD0,
    (uint8_t)((openHaystackPublicKey[0] >> 6) & 0x03), 0x00                                            
  };

  Bluefruit.Advertising.setData(rawPayload, sizeof(rawPayload));
  Bluefruit.Advertising.setInterval(512, 512); 
}

void loop() {
  int currentPinState = digitalRead(TRIGGER_INPUT_PIN);

  if (currentPinState != lastPinState) {
    delay(15);
    currentPinState = digitalRead(TRIGGER_INPUT_PIN); 
    
    if (currentPinState != lastPinState) {
      if (currentPinState == LOW && !isAdvertising) {
        armOpenHaystackPayload();
        Bluefruit.Advertising.start(0); 
        isAdvertising = true;
        Serial.println("[STATUS] State Flipped -> Pin 008: LOW (0V) | AirTag Active: YES");
      } 
      else if (currentPinState == HIGH && isAdvertising) {
        Bluefruit.Advertising.stop();
        isAdvertising = false;
        Serial.println("[STATUS] State Flipped -> Pin 008: HIGH (3.3V) | AirTag Active: NO");
      }
      lastPinState = currentPinState;
    }
  }
  delay(10); 
}