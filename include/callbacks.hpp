#ifndef CALLBACKS_HPP
#define CALLBACKS_HPP

#include "BLEServer.h"
#include "BLEDevice.h"

#include "LEDManager.hpp"
#include "utils.hpp"

extern uint64_t lastBrightnessChange;
extern bool isBrightnessChanging;

class BrightnessCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        if (value <= 0) value = 1;
        else if (value > 255) value = 255;

        LEDManager::instance().setBrightness(value);
        isBrightnessChanging = true;
        lastBrightnessChange = millis64();
    }
};

class ColorCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String colorString = pCharacteristic->getValue();
        if (colorString.length() == 6)
            LEDManager::instance().setColor(colorString.c_str());
    }
};

class DistanceCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        LEDManager::instance().displayDistance(value);
    }
};

class NameCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String value = pCharacteristic->getValue();
        if (value.length() > 0)
            esp_ble_gap_set_device_name(value.c_str());
    }
};

#endif
