#ifndef CALLBACKS_HPP
#define CALLBACKS_HPP

#include "BLEServer.h"
#include "BLEDevice.h"

#include "LEDManager.hpp"

class NameCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String value = pCharacteristic->getValue();
        if (value.length() > 0)
            esp_ble_gap_set_device_name(value.c_str());
    }
};

class ColorCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String colorString = pCharacteristic->getValue();
        if (colorString.length() == 6)
            LEDManager::instance.setColor(colorString.c_str());
    }
};

class DistanceCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        LEDManager::instance.displayDistance(value);
    }
};

#endif
