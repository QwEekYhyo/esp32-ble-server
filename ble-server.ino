#include <esp_bt_device.h>
#include "Wire.h"

#include "include/BLEServerManager.hpp"
#include "include/LEDManager.hpp"

#define DEFAULT_DEVICE_NAME "Cool device v2"

float map_cool(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint64_t millis64() {
    static uint32_t low32, high32;
    uint32_t new_low32 = millis();
    if (new_low32 < low32) high32++;
    low32 = new_low32;
    return (uint64_t) high32 << 32 | low32;
}

uint64_t lastBrightnessChange = 0;
bool isBrightnessChanging = false;
bool wasPreviouslyCharging = false;
int animOffset = 0;
LEDManager ledManager;

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
            ledManager.setColor(colorString.c_str());
    }
};

class BrightnessCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        if (value <= 0) value = 1;
        else if (value > 255) value = 255;

        ledManager.setBrightness(value);
        isBrightnessChanging = true;
        lastBrightnessChange = millis64();
    }
};

class DistanceCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        ledManager.displayDistance(value);
    }
};

void setup() {
    // Arduino specific soy dev things
    Wire.begin(13,12);
    ledManager.turnOff();

    BLEServerManager server(DEFAULT_DEVICE_NAME);

    server.addCharacteristic("Name", DEFAULT_DEVICE_NAME, new NameCallbacks());
    char colorString[7];
    server.addCharacteristic("Color", ledManager.getColor().toString(colorString), new ColorCallbacks());
    server.addCharacteristic("Brightness", "50", new BrightnessCallbacks());
    ledManager.setBrightness(50);
    server.addCharacteristic("Distance", "0", new DistanceCallbacks());

    server.start();
}

uint8_t iterationCounter = 0;
int16_t V;
int16_t I;
double current, voltage;

void loop() {
    Wire.beginTransmission(0x70);
    Wire.write(0);
    Wire.write(16);
    Wire.endTransmission();

    Wire.beginTransmission(0x70);
    Wire.write(6);
    Wire.endTransmission();
    Wire.requestFrom(0x70, 2);
    I = Wire.read() | (Wire.read() << 8);

    Wire.beginTransmission(0x70);
    Wire.write(8);
    Wire.endTransmission();
    Wire.requestFrom(0x70,2);
    V = Wire.read() | (Wire.read() << 8);

    if (I & 0b0010000000000000) {
        I = (~I);
        I = I + 0b1;
        I = I & 0b0011111111111111;
        I = I + 1;
        I = -I;
    }

    current = (float(I) * 0.00001177) / 0.030;
    voltage = float(V) * 0.00244;

    if (current >= 0) {
        if (!wasPreviouslyCharging) {
            wasPreviouslyCharging = true;
            ledManager.setColor("00FF00");
        }
        size_t line_length = (size_t) map_cool(voltage, 3.0, 4.2, 1.0, 21.0);
        iterationCounter++;
        if (iterationCounter >= 25) {
            animOffset++;
            iterationCounter = 0;
        }
        ledManager.line(animOffset % 2 == 0 ? line_length : line_length - 1);
        delay(20);
    } else {
        if (wasPreviouslyCharging) {
            wasPreviouslyCharging = false;
            ledManager.turnOff();
        }
        if (isBrightnessChanging) {
            iterationCounter++;
            if (iterationCounter >= 3) {
                animOffset++;
                iterationCounter = 0;
            }
            ledManager.rainbow(animOffset);
            delay(20);
            if (millis64() - lastBrightnessChange >= 1500) {
                isBrightnessChanging = false;
                ledManager.turnOff();
                animOffset = 0;
            }
        }
    }
}
