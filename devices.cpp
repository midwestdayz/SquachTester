#include "devices.h"

const char* deviceExplain(DeviceType t) {
    switch (t) {
        case DeviceType::FLOCK:
        return "Flock Safety makes automated license-plate-reader cameras, usually mounted on poles at neighborhood entrances. They log every plate that passes, suspect or not.";
        case DeviceType::AXON:
        return "Axon makes body cameras and TASERs for law enforcement.";
        case DeviceType::META:
        return "Camera glasses such as Ray-Ban Meta, Snap Spectacles and the like. They record video and photos, and the little LED that's supposed to warn you is easy to miss and easier to cover. Attention: Meta puts the same Bluetooth ID on it's Quest headsets.";
        case DeviceType::SKIMMER:
        return "A Bluetooth card skimmer, usually wired into an ATM or gas pump reader. It quietly exfiltrates stolen card data over BLE instead of needing physical pickup.";
        case DeviceType::RAVEN:
        return "A Raven gunshot-detection sensor, usually mounted on a streetlight or rooftop. It listens constantly, not just after something happens.";
        case DeviceType::AIRTAG:
        return "An Apple AirTag, riding Apple's Find My network. Legitimate for keys and luggage -- also a known method for tracking a person or vehicle without consent.";
        case DeviceType::DRONE:
        return "A drone broadcasting Remote ID, the wireless 'license plate' the FAA requires most drones to transmit.";
        case DeviceType::ALPR:
        return "An automated license-plate reader from a vendor other than Flock. Same idea: logs every plate that passes, usually feeding a shared database.";
        case DeviceType::CAMERA:
        return "A camera-brand WiFi or Bluetooth radio, matched by hardware vendor rather than a specific known network. Could be a doorbell, a security cam, almost anything with a lens.";
        case DeviceType::SAMSUNG_TAG:
        return "A Samsung Galaxy SmartTag, riding Samsung's own item-finder network. Same tracking-without-consent concern as an AirTag, different ecosystem.";
        case DeviceType::GOOGLE_TAG:
        return "A tracker on Google's Find My Device network -- Chipolo, Pebblebee, Moto Tag and others all ride the same system. Android's answer to Find My.";
        case DeviceType::TILE:
        return "A Tile Bluetooth tracker, one of the original item-finders. Independent of Apple/Google/Samsung's networks, same basic capability either way.";
        case DeviceType::RING:
        return "A Ring doorbell or camera, Amazon's video doorbell line. Often networked into neighborhood-wide sharing through the Neighbors app.";
        case DeviceType::IBEACON:
        return "A proximity beacon, found in most public buildings. It does not track you by itself, it shouts an ID, and an app you you may have installed notices and reports back. The number shown is which deployment and which unit. one shop can have many beacons.";
        case DeviceType::HACKER:
        return "Not a device -- a burst of WiFi deauthentication frames, the kind used to forcibly knock devices off a network. One frame is normal traffic; a flood like this is an attack.";
        case DeviceType::EVILTWIN:
        return "Two different boxes are broadcasting the same network name, and they disagree about security -- one wants a password, the other is wide open. That's how a fake hotspot lures you on. A mesh system never argues with itself about encryption.";
        default:
        return "Unknown device.";
    }
}

bool canEmulate(DeviceType t) {
    return t != DeviceType::EVILTWIN;
}