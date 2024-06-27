#include <esp_bt_device.h>
#include <Adafruit_NeoPixel.h>

#include "include/BLEServerManager.hpp"

#define LED_PIN 25

#define FREQUENCY 2 // in Hz

#define DEFAULT_DEVICE_NAME "Hirondelle Server"

Adafruit_NeoPixel pixels(24, LED_PIN);

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
        for (uint8_t pixel = 0; pixel < 13; pixel++) {
            if (pixel <= value) {
                pixels.setPixelColor(pixel, 255, 0, 0);
                pixels.setPixelColor(getSymmetrical(pixel), 255, 0, 0);
            } else {
                pixels.setPixelColor(pixel, 0, 0, 0);
                pixels.setPixelColor(getSymmetrical(pixel), 0, 0, 0);
            }
        }

        pixels.show();
    }
};

class DistanceCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String value = pCharacteristic->getValue();
        if (value.length() > 0) {
            Serial.println("*********");
            Serial.print("New value: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);
            Serial.println();
            Serial.println("*********");
        }
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
    server.addCharacteristic("Brightness", "0", new BrightnessCallbacks());
    server.addCharacteristic("Distance", "0", new DistanceCallbacks());

    server.start();
}

void loop() {
    delay(1000 / FREQUENCY);
}
