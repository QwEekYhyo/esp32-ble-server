#include <esp_bt_device.h>
#include <Adafruit_NeoPixel.h>

#include "include/BLEServerManager.hpp"
#include "include/color.hpp"

#define LED_PIN 5
#define FREQUENCY 2 // in Hz
#define DELAYED_SECONDS 1000 / FREQUENCY
#define DEFAULT_DEVICE_NAME "Procédure complexe"
#define NUM_LED 37
#define HEIGHT 7

Adafruit_NeoPixel pixels(NUM_LED, LED_PIN);
Color currentColor(255, 0, 0);

uint8_t shape[HEIGHT] = {
    4,
    5,
    6,
    7,
    6,
    5,
    4,
};

size_t getLedIndex(size_t x, size_t y) {
    size_t result = 0;
    for (size_t row = x + 1; row < HEIGHT; row++) {
        result += shape[row];
    }
    return result + shape[x] - y - 1;
}

void circle(uint8_t radius) {
    if (radius == 0) {
        for (size_t i = 0; i < NUM_LED; i++) {
            pixels.setPixelColor(i, 0, 0, 0);
        }
    } else {
        uint8_t outerRadius = 4 - radius;
        for (size_t x = 0; x < HEIGHT; x++) {
            for (size_t y = 0; y < shape[x]; y++) {
                if (x >= outerRadius && x < HEIGHT - outerRadius && y >= outerRadius && y < shape[x] - outerRadius) {
                    pixels.setPixelColor(getLedIndex(x, y), currentColor.r, currentColor.g, currentColor.b);
                } else {
                    pixels.setPixelColor(getLedIndex(x, y), 0, 0, 0);
                }
            }
        }
    }
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

class ColorCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String colorString = pCharacteristic->getValue();
        if (colorString.length() == 6)
            currentColor.setColor(colorString.c_str());
    }
};

class BrightnessCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        if (value <= 0) value = 1;
        else if (value > 255) value = 255;

        pixels.setBrightness(value);
    }
};

class DistanceCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        if (value <= 400) {
            circle(4 - value / 100);
        } else {
            for (uint8_t pixel = 0; pixel < NUM_LED; pixel++) {
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
#ifdef DEBUG_MODE
    Serial.begin(9600);
#endif
    // NeoPixel
    pixels.begin();
    for (uint8_t pixel = 0; pixel < NUM_LED; pixel++) {
        pixels.setPixelColor(pixel, 0, 0, 0);
    }
    pixels.show();

    BLEServerManager server(DEFAULT_DEVICE_NAME);

    server.addCharacteristic("Name", DEFAULT_DEVICE_NAME, new NameCallbacks());
    char colorString[7];
    server.addCharacteristic("Color", currentColor.toString(colorString), new ColorCallbacks());
    server.addCharacteristic("Brightness", "255", new BrightnessCallbacks());
    server.addCharacteristic("Distance", "0", new DistanceCallbacks());

    server.start();
}

void loop() {
    delay(DELAYED_SECONDS);
}
