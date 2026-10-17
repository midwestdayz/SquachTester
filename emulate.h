#pragma once
#include "devices.h"

namespace Emulate {
  void begin();
  void stop();
  bool start(DeviceType t);
  void update();
  bool active();
  DeviceType current();
    
  void setCustomSsid(const char* ssid);
  const char* getCustomSsid();
  int scanNetworks(); 
  const char* getScannedMacStr(int index);
  const char* getScannedSsidStr(int index);
  void setTargetMacFromScan(int index);
  const uint8_t* getTargetDeauthMac();
}