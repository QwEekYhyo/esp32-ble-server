#include <esp_bt_device.h>
#include "Wire.h"

#include "include/BLEServerManager.hpp"
#include "include/LEDManager.hpp"
#include "include/callbacks.hpp"
#include "include/utils.hpp"

#define DEFAULT_DEVICE_NAME "Cool device v2"

uint64_t lastBrightnessChange = 0;
bool isBrightnessChanging = false;
bool wasPreviouslyCharging = false;
int animOffset = 0;
BLEServerManager* server;

class BrightnessCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        if (value <= 0) value = 1;
        else if (value > 255) value = 255;

        LEDManager::instance.setBrightness(value);
        isBrightnessChanging = true;
        lastBrightnessChange = millis64();
    }
};

void setup() {
    // Arduino specific soy dev things
    Wire.begin(13,12);
    LEDManager::instance.turnOff();

    server = new BLEServerManager(DEFAULT_DEVICE_NAME);

    server->addCharacteristic("Name", DEFAULT_DEVICE_NAME, new NameCallbacks());
    char colorString[7];
    server->addCharacteristic("Color", LEDManager::instance.getColor().toString(colorString), new ColorCallbacks());
    server->addBrightnessCharacteristic("50", new BrightnessCallbacks());
    LEDManager::instance.setBrightness(50);
    server->addCharacteristic("Distance", "0", new DistanceCallbacks());

    server->start();
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
            LEDManager::instance.setColor("00FF00");
            LEDManager::instance.setBrightness(10);
            server->stop();
        }
        size_t line_length = (size_t) map_cool(voltage, 3.0, 4.2, 1.0, 21.0);
        iterationCounter++;
        if (iterationCounter >= 25) {
            animOffset++;
            iterationCounter = 0;
        }
        LEDManager::instance.line(animOffset % 2 == 0 ? line_length : line_length - 1);
        delay(20);
    } else {
        if (wasPreviouslyCharging) {
            wasPreviouslyCharging = false;
            LEDManager::instance.turnOff();
            LEDManager::instance.setBrightness(server->getCurrentBrightness());
            server->start();
        }
        if (isBrightnessChanging) {
            iterationCounter++;
            if (iterationCounter >= 3) {
                animOffset++;
                iterationCounter = 0;
            }
            LEDManager::instance.rainbow(animOffset);
            delay(20);
            if (millis64() - lastBrightnessChange >= 1500) {
                isBrightnessChanging = false;
                LEDManager::instance.turnOff();
                animOffset = 0;
            }
        }
    }
}
