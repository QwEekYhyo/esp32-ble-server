#include <esp_bt_device.h>
#include <Adafruit_NeoPixel.h>

#include "include/BLEServerManager.hpp"

#define LED_PIN 25
#define FREQUENCY 2 // in Hz
#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_DEVICE_NAME "Hirondelle Server"

Adafruit_NeoPixel pixels(24, LED_PIN);
uint8_t currentBrightness = DEFAULT_BRIGHTNESS;

uint8_t getSymmetrical(uint8_t pixel) {
    return pixel == 0 ? 0 : 24 - pixel;
}

class NameCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String value = pCharacteristic->getValue();
        if (value.length() > 0) {
            esp_ble_gap_set_device_name(value.c_str());
#ifdef DEBUG_MODE
            Serial.println("Changed device name");
#endif
        }
    }
};

class BrightnessCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        if (value <= 0) currentBrightness = 1;
        else if (value > 255) currentBrightness = 255;
        else currentBrightness = value;
    }
};

class DistanceCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        if (value <= 500) {
            value = 13 - ((value * 13) / 500);
            for (uint8_t pixel = 0; pixel < 13; pixel++) {
                if (pixel <= value) {
                    pixels.setPixelColor(pixel, currentBrightness, 0, 0);
                    pixels.setPixelColor(getSymmetrical(pixel), currentBrightness, 0, 0);
                } else {
                    pixels.setPixelColor(pixel, 0, 0, 0);
                    pixels.setPixelColor(getSymmetrical(pixel), 0, 0, 0);
                }
            }
        } else {
            for (uint8_t pixel = 0; pixel < 24; pixel++) {
                pixels.setPixelColor(pixel, 0, 0, 0);
            }
        }

        pixels.show();
#ifdef DEBUG_MODE
        String stringValue = pCharacteristic->getValue();
        if (stringValue.length() > 0) {
            Serial.println("*********");
            Serial.print("New value: ");
            for (int i = 0; i < stringValue.length(); i++)
                Serial.print(stringValue[i]);
            Serial.println();
            Serial.println("*********");
        }
#endif
    }
};

void setup() {
    // Arduino specific soy dev things
    Serial.begin(9600);
    // NeoPixel
    pixels.begin();
    for (uint8_t pixel = 0; pixel < 24; pixel++) {
        pixels.setPixelColor(pixel, 0, 0, 0);
    }
    pixels.show();


    BLEServerManager server(DEFAULT_DEVICE_NAME);

    server.addCharacteristic("Name", DEFAULT_DEVICE_NAME, new NameCallbacks());
    server.addCharacteristic("Brightness", String(currentBrightness).c_str(), new BrightnessCallbacks());
    server.addCharacteristic("Distance", "0", new DistanceCallbacks());

    server.start();
}

void loop() {
    delay(1000 / FREQUENCY);
}
