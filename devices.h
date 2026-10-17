#pragma once
#include <stdint.h>

enum class DeviceType : uint8_t {
    FLOCK = 0,
    AXON,
    META,
    SKIMMER,
    RAVEN,
    AIRTAG,
    DRONE,
    ALPR,
    CAMERA,
    SAMSUNG_TAG,
    GOOGLE_TAG,
    TILE,
    RING,
    IBEACON,
    HACKER,
    EVILTWIN,
    DEAUTH,
    COUNT
};

static const uint8_t kGridCells = 16;

inline const char* deviceName(DeviceType t) {
    switch (t) {
        case DeviceType::FLOCK:       return "FLOCK";
        case DeviceType::AXON:        return "AXON";
        case DeviceType::META:        return "META";
        case DeviceType::SKIMMER:     return "SKIMMER";
        case DeviceType::RAVEN:       return "RAVEN";
        case DeviceType::AIRTAG:      return "AIRTAG";
        case DeviceType::DRONE:       return "DRONE";
        case DeviceType::ALPR:        return "ALPR";
        case DeviceType::CAMERA:      return "CAMERA";
        case DeviceType::SAMSUNG_TAG: return "SAMSUNG_TAG";
        case DeviceType::GOOGLE_TAG:  return "GOOGLE_TAG";
        case DeviceType::TILE:        return "TILE";
        case DeviceType::RING:        return "RING";
        case DeviceType::IBEACON:     return "IBEACON";
        case DeviceType::HACKER:      return "HACKER";
        case DeviceType::EVILTWIN:    return "TWIN";
        default:                      return "?";
    }
}

inline const char* shortName(DeviceType t) {
    switch (t) {
        case DeviceType::FLOCK:       return "FLOCK";
        case DeviceType::AXON:        return "AXON";
        case DeviceType::META:        return "GLASS";
        case DeviceType::SKIMMER:     return "SKIM";
        case DeviceType::RAVEN:       return "RAV";
        case DeviceType::AIRTAG:      return "TRACKER";
        case DeviceType::DRONE:       return "DRONE";
        case DeviceType::ALPR:        return "ALPR";
        case DeviceType::CAMERA:      return "CAM";
        case DeviceType::SAMSUNG_TAG: return "STAG";
        case DeviceType::GOOGLE_TAG:  return "GTAG";
        case DeviceType::TILE:        return "TILE";
        case DeviceType::RING:        return "RING";
        case DeviceType::IBEACON:     return "BEACON";
        case DeviceType::HACKER:      return "HACK";
        case DeviceType::EVILTWIN:    return "TWIN";
        default:                      return "?";
    }
}

inline DeviceType typeAt(uint8_t i) {
    static const DeviceType card[kGridCells] = {
        DeviceType::FLOCK, DeviceType::AXON, DeviceType::META, DeviceType::SKIMMER,
        DeviceType::RAVEN, DeviceType::AIRTAG, DeviceType::DRONE, DeviceType::ALPR,
        DeviceType::CAMERA, DeviceType::SAMSUNG_TAG, DeviceType::GOOGLE_TAG, DeviceType::TILE,
        DeviceType::RING, DeviceType::IBEACON, DeviceType::HACKER, DeviceType::EVILTWIN,
    };
    return i < kGridCells ? card[i] : DeviceType::FLOCK;
}

const char* deviceExplain(DeviceType t);

bool canEmulate(DeviceType t);